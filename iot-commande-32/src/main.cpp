#include <Arduino.h>
#include <WiFiManager.h>
#include <Firebase_ESP_Client.h>
#include "PumpController.h"
#include "config.h"

WiFiManager wifiManager;
FirebaseData fbdo;
FirebaseAuth auth;
FirebaseConfig config;
PumpController pumpController(RELAY_PIN_1, RELAY_PIN_2);

bool wifiConnected = false;
unsigned long lastFirebaseReconnectAttempt = 0;
const unsigned long FIREBASE_RECONNECT_INTERVAL = 30000;

void configModeCallback(WiFiManager *myWiFiManager) {
    Serial.println("[WIFI] Mode configuration AP actif");
    Serial.print("[WIFI] SSID: ");
    Serial.println(myWiFiManager->getConfigPortalSSID());
    Serial.print("[WIFI] IP: ");
    Serial.println(WiFi.softAPIP());
}

void firebaseStreamCallback(FirebaseStream data) {
    Serial.printf("[FIREBASE] Stream reçu, chemin: %s\n", data.streamPath().c_str());
    
    if (data.dataType() == "json") {
        FirebaseJson *json = data.to<FirebaseJson *>();
        FirebaseJsonData result;
        
        bool p1State = false;
        bool p2State = false;
        
        if (json->get(result, "p1")) {
            p1State = result.to<bool>();
        }
        if (json->get(result, "p2")) {
            p2State = result.to<bool>();
        }
        
        pumpController.updateState(p1State, p2State);
        
        Serial.printf("[COMMAND] Pompe 1: %s, Pompe 2: %s\n", 
                     p1State ? "ON" : "OFF", p2State ? "ON" : "OFF");
        
        if (json->get(result, "msg")) {
            Serial.printf("[COMMAND] Message: %s\n", result.to<String>().c_str());
        }
        
        Firebase.RTDB.setInt(&fbdo, "/status/last_command_received", millis() / 1000);
    }
}

void firebaseStreamTimeoutCallback(bool timeout) {
    if (timeout) {
        Serial.println("[FIREBASE] Stream timeout, reconnexion...");
    }
}

void setupWiFi() {
    Serial.println("[WIFI] Démarrage configuration WiFi...");
    
    wifiManager.setAPCallback(configModeCallback);
    wifiManager.setConfigPortalTimeout(180);
    
    WiFiManagerParameter custom_firebase_host("host", "Firebase Host", FIREBASE_HOST, 100);
    wifiManager.addParameter(&custom_firebase_host);
    
    if (!wifiManager.autoConnect("Irrigation-ESP32")) {
        Serial.println("[WIFI] Échec de connexion, redémarrage...");
        delay(3000);
        ESP.restart();
    }
    
    wifiConnected = true;
    Serial.println("[WIFI] Connecté!");
    Serial.print("[WIFI] IP: ");
    Serial.println(WiFi.localIP());
}

void setupFirebase() {
    Serial.println("[FIREBASE] Configuration...");
    
    config.api_key = FIREBASE_API_KEY;
    config.database_url = FIREBASE_DATABASE_URL;
    
    auth.user.email = "";
    auth.user.password = "";
    
    Firebase.reconnectWiFi(true);
    fbdo.setBSSLBufferSize(4096, 1024);
    fbdo.setResponseSize(2048);
    
    Firebase.begin(&config, &auth);
    
    Serial.print("[FIREBASE] Connexion");
    while (!Firebase.ready()) {
        Serial.print(".");
        delay(100);
    }
    Serial.println("\n[FIREBASE] Connecté!");
    
    if (!Firebase.RTDB.beginStream(&fbdo, "/commands")) {
        Serial.printf("[FIREBASE] Erreur stream: %s\n", fbdo.errorReason().c_str());
    } else {
        Firebase.RTDB.setStreamCallback(&fbdo, firebaseStreamCallback, firebaseStreamTimeoutCallback);
        Serial.println("[FIREBASE] Stream démarré sur /commands");
    }
}

void setup() {
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n===== SYSTEME IRRIGATION ESP32 =====");
    Serial.println("Initialisation...");
    
    pumpController.begin();
    setupWiFi();
    setupFirebase();
    
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH);
    
    Serial.println("[SYSTEME] Prêt!");
}

void loop() {
    if (pumpController.checkSafetyTimeout()) {
        Serial.println("[SAFETY] Timeout sécurité déclenché!");
        digitalWrite(LED_BUILTIN, LOW);
    }
    
    if (WiFi.status() != WL_CONNECTED) {
        wifiConnected = false;
        digitalWrite(LED_BUILTIN, LOW);
        Serial.println("[WIFI] Déconnecté, tentative reconnexion...");
        
        WiFi.reconnect();
        delay(5000);
        
        if (WiFi.status() == WL_CONNECTED) {
            wifiConnected = true;
            digitalWrite(LED_BUILTIN, HIGH);
            Serial.println("[WIFI] Reconnexion réussie!");
            setupFirebase();
        }
    }
    
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck > 30000) {
        lastCheck = millis();
        
        if (wifiConnected && !Firebase.ready()) {
            Serial.println("[FIREBASE] Perte de connexion, reconnexion...");
            setupFirebase();
        }
    }
    
    delay(100);
}