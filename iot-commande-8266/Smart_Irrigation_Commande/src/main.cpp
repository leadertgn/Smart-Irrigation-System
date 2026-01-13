#include <time.h>
#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <Firebase_ESP_Client.h>
#include <WiFiManager.h>
#include "config.h"
#include "WateringSystem.h"

// --- Objets Globaux ---
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
WateringSystem irrigation;

// --- Variables d'état ---
bool z1_target = false;
bool z2_target = false;
unsigned long lastStreamCheck = 0;
const unsigned long STREAM_RECONNECT_INTERVAL = 30000; // 30 secondes

// --- Fonction : Formatage du Temps ISO8601 ---
String getISO8601Time() {
    time_t now;
    time(&now);
    struct tm * timeinfo;
    timeinfo = localtime(&now); // Utilise le fuseau défini par TZ dans le setup

    if (timeinfo->tm_year < (2025 - 1900)) return "1970-01-01T00:00:00Z";

    char buffer[25];
    // On retire le 'Z' car techniquement on est à +1, 
    // mais si ton backend en a besoin, on peut le laisser
    strftime(buffer, sizeof(buffer), "%Y-%m-%dT%H:%M:%SZ", timeinfo);
    return String(buffer);
}
// --- Fonction : Initialisation du Stream Firebase ---
void startFirebaseStream() {
    Serial.println("\n📡 Stream Firebase en mode manuel/site...");
    if (!Firebase.RTDB.beginStream(&fbdo, "/zones")) {
        Serial.printf("❌ Erreur: %s\n", fbdo.errorReason().c_str());
    } else {
        Firebase.RTDB.setStreamCallback(&fbdo, [](FirebaseStream data) {
            String path = data.dataPath();
            Serial.printf("\n📥 Flux reçu sur: %s | Type: %s\n", path.c_str(), data.dataType().c_str());

            // --- CAS 1 : Le site modifie tout l'objet state ---
            if (data.dataType() == "json") {
                FirebaseJson &json = data.jsonObject();
                FirebaseJsonData result;
                
                // On cherche pump_status dans le JSON, peu importe où il est
                if (json.get(result, "z1/state/pump_status") || json.get(result, "pump_status")) {
                    if (result.success) z1_target = result.boolValue;
                }
                // Si le chemin est spécifiquement z2 ou contient z2
                if (json.get(result, "z2/state/pump_status") || (path.indexOf("z2") != -1 && json.get(result, "pump_status"))) {
                    if (result.success) z2_target = result.boolValue;
                }
            } 
            // --- CAS 2 : Modification directe dans la console (Booléen simple) ---
            else {
                if (path.indexOf("z1") != -1 && path.indexOf("pump_status") != -1) z1_target = data.boolData();
                if (path.indexOf("z2") != -1 && path.indexOf("pump_status") != -1) z2_target = data.boolData();
            }

            Serial.printf("⚙️ ACTION -> Z1: %s | Z2: %s\n", z1_target ? "ON" : "OFF", z2_target ? "ON" : "OFF");
            irrigation.updateZones(z1_target, z2_target);
            
        }, nullptr);
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    // 1. Initialisation Hardware
    irrigation.begin();
    Serial.println("\n--- Système de Commande Smart Irrigation ---");

    // 2. Wi-Fi Manager (Point d'accès si pas de Wi-Fi connu)
    WiFiManager wm;
    WiFi.setAutoReconnect(true);
    WiFi.persistent(true);
    
    if (!wm.autoConnect("SmartIrrigation_AP")) {
        Serial.println("❌ Échec de connexion Wi-Fi. Redémarrage...");
        delay(3000);
        ESP.restart();
    }
    Serial.println("🌐 Wi-Fi Connecté !");

    // 3. Configuration Firebase
    config.host = FIREBASE_HOST;
    config.signer.tokens.legacy_token = FIREBASE_AUTH;
    Firebase.begin(&config, &auth);
    Firebase.reconnectWiFi(true);

    // 4. Configuration Temps (NTP)
    configTime(GMT_OFFSET_SEC, DAYLIGHT_OFFSET_SEC, NTP_SERVER);
    Serial.print("⏳ Synchronisation heure NTP...");
    setenv("TZ", "WAT-1", 1);
    tzset();
    time_t now = time(nullptr);
    int retry = 0;
    while (now < 8 * 3600 * 2 && retry < 20) { // Max 10 secondes d'attente
        delay(500);
        Serial.print(".");
        now = time(nullptr);
        retry++;
    }
    Serial.println("\n🕒 Heure actuelle: " + getISO8601Time());

    // 5. Lancer l'écoute en temps réel
    startFirebaseStream();
}

void loop() {
    // A. Vérification de la santé du Stream toutes les 30s
    if (millis() - lastStreamCheck > STREAM_RECONNECT_INTERVAL) {
        lastStreamCheck = millis();
        if (!fbdo.httpConnected()) {
            Serial.println("⚠️ Connexion Stream perdue. Reconnecion...");
            startFirebaseStream();
        }
    }

    // B. Sécurité : Timeout de 5 minutes
    if (irrigation.getElapsed() > GLOBAL_TIMEOUT_MS) {
        Serial.println("🚨 ALERTE : Sécurité Timeout (5 min) ! Coupure forcée.");
        
        // Arrêt local
        irrigation.stopAll();
        z1_target = false;
        z2_target = false;

        // Synchronisation Firebase (Update des status et du timestamp)
        String nowTime = getISO8601Time();
        
        Firebase.RTDB.setBool(&fbdo, "/zones/z1/state/pump_status", false);
        Firebase.RTDB.setBool(&fbdo, "/zones/z2/state/pump_status", false);
        Firebase.RTDB.setString(&fbdo, "/zones/z1/state/last_irrigation", nowTime);
        Firebase.RTDB.setString(&fbdo, "/zones/z2/state/last_irrigation", nowTime);
        
        Serial.println("✔️ Firebase synchronisé après timeout.");
    }

    // Maintenir les tâches Firebase en arrière-plan
    delay(500); 
}