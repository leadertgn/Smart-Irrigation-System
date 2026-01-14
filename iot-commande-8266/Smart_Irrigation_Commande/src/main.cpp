#include <time.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <WiFiManager.h>
#include "config.h"
#include "WateringSystem.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
WateringSystem irrigation;

bool z1_target = false, z2_target = false;
int z1_duration = 30, z2_duration = 30; // Durées en secondes
unsigned long lastStreamCheck = 0;

String getISO8601Time() {
    time_t now; time(&now);
    struct tm * timeinfo = localtime(&now);
    if (timeinfo->tm_year < (2025 - 1900)) return "1970-01-01T00:00:00Z";
    char buffer[25];
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", timeinfo);
    return String(buffer);
}

void syncSafetyToFirebase() {
    String nowTime = getISO8601Time();
    Firebase.RTDB.setBool(&fbdo, "/zones/z1/state/pump_status", false);
    Firebase.RTDB.setBool(&fbdo, "/zones/z2/state/pump_status", false);
    Firebase.RTDB.setString(&fbdo, "/zones/z1/state/last_irrigation", nowTime);
    Firebase.RTDB.setString(&fbdo, "/zones/z2/state/last_irrigation", nowTime);
    Serial.println("🔥 Sécurité synchronisée sur Firebase.");
}

void startFirebaseStream() {
    if (!Firebase.RTDB.beginStream(&fbdo, "/zones")) {
        Serial.printf("❌ Erreur Stream: %s\n", fbdo.errorReason().c_str());
        return;
    }

    Firebase.RTDB.setStreamCallback(&fbdo, [](FirebaseStream data) {
        FirebaseJsonData res;
        if (data.dataType() == "json") {
            FirebaseJson &json = data.jsonObject();
            // Status
            if (json.get(res, "z1/state/pump_status")) z1_target = res.boolValue;
            if (json.get(res, "z2/state/pump_status")) z2_target = res.boolValue;
            // Durées (en secondes)
            if (json.get(res, "z1/config/watering_duration_sec")) z1_duration = res.intValue;
            if (json.get(res, "z2/config/watering_duration_sec")) z2_duration = res.intValue;
        } 
        else if (data.dataType() == "boolean") {
            if (data.dataPath().indexOf("z1") != -1) z1_target = data.boolData();
            if (data.dataPath().indexOf("z2") != -1) z2_target = data.boolData();
        }

        // Mise à jour et vérification sécurité
        if (irrigation.updateZones(z1_target, z2_target, z1_duration, z2_duration)) {
            z1_target = z2_target = false;
            syncSafetyToFirebase();
        }
    }, nullptr);
}

void setup() {
    Serial.begin(115200);
    irrigation.begin();
    WiFiManager wm;
    if (!wm.autoConnect("SmartIrrigation_AP")) ESP.restart();

    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    setenv("TZ", "WAT-1", 1); tzset();
    
    startFirebaseStream();
}

void loop() {
    if (millis() - lastStreamCheck > 30000) {
        lastStreamCheck = millis();
        if (!fbdo.httpConnected()) startFirebaseStream();
    }
    
    // Vérification de sécurité continue dans la loop aussi
    if (irrigation.updateZones(z1_target, z2_target, z1_duration, z2_duration)) {
        z1_target = z2_target = false;
        syncSafetyToFirebase();
    }
    delay(500);
}