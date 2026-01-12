#include <Arduino.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include "config.h"
#include "PumpController.h"

FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig fbConfig;
PumpController pumps(RELAY_PIN_1, RELAY_PIN_2, RELAY_ACTIVE_LOW);

bool p1 = false;
bool p2 = false;

void streamCallback(FirebaseStream data) {
    // Parsing précis selon ton JSON
    if (data.dataPath() == "/z1/state/pump_status") {
        p1 = data.boolData();
    }
    if (data.dataPath() == "/z2/state/pump_status") {
        p2 = data.boolData();
    }

    pumps.update(p1, p2);

    // Feedback de confirmation
    Firebase.RTDB.setInt(&fbdo, "/system_settings/last_cmd_unix", millis() / 1000);
}

void setup() {
    Serial.begin(115200);
    pumps.begin();

    WiFiManager wm; // gestion du WiFi via WiFiManager
    if (!wm.autoConnect("Irrigation-Commande-AP")) {
        ESP.restart();
    }

    fbConfig.api_key = FIREBASE_API_KEY; // config Firebase
    fbConfig.database_url = FIREBASE_DATABASE_URL;
    Firebase.begin(&fbConfig, &auth);
    Firebase.reconnectWiFi(true);

    // Écoute temps réel
    if (Firebase.RTDB.beginStream(&fbdo, "/zones")) {
        Firebase.RTDB.setStreamCallback(&fbdo, streamCallback, [](bool) {});
    }
}

void loop() {
    // Sécurité Failsafe 20 min
    if (pumps.isTimeoutActive()) {
        pumps.safetyStop();
    }
}