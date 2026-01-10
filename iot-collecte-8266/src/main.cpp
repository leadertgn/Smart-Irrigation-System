// main.cpp - IoT Collecte ESP8266
// Développeur: Ozias
// Conforme au cahier des charges et contrat d'interface

#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "SoilSensor.h"
#include "EnvSensor.h"
#include "config.h"

// ==================== OBJETS GLOBAUX ====================
SoilSensor soilSensor(SOIL_SENSOR_PIN);
EnvSensor envSensor(DHT_SENSOR_PIN, DHT_TYPE);

WiFiManager wifiManager;
char serverUrl[100] = DEFAULT_BACKEND_URL;

unsigned long lastSendTime = 0;
bool systemInitialized = false;
int sendAttemptCount = 0;

// ==================== FONCTIONS UTILITAIRES ====================
void blinkLED(int count, int duration = 200) {
    #ifdef LED_DEBUG
    for(int i = 0; i < count; i++) {
        digitalWrite(LED_BUILTIN, LOW);
        delay(duration);
        digitalWrite(LED_BUILTIN, HIGH);
        delay(duration);
    }
    #endif
}

void printSystemInfo() {
    Serial.println("\n" + String(80, '='));
    Serial.println("SYSTÈME IRRIGATION INTELLIGENT - IoT COLLECTE");
    Serial.println(String(80, '='));
    Serial.println("Développeur: Ozias");
    Serial.println("Microcontrôleur: ESP8266 NodeMCU");
    Serial.println("Rôle: Collecte données capteurs → Backend Node.js");
    Serial.print("Version: ");
    Serial.println(__DATE__);
    Serial.println(String(80, '-'));
    
    if (WiFi.status() == WL_CONNECTED) {
        Serial.print("WiFi: ");
        Serial.println(WiFi.SSID());
        Serial.print("IP: ");
        Serial.println(WiFi.localIP());
        Serial.print("Signal: ");
        Serial.print(WiFi.RSSI());
        Serial.println(" dBm");
    }
    
    Serial.print("URL Backend: ");
    Serial.println(serverUrl);
    Serial.print("Node ID: ");
    Serial.println(NODE_ID);
    Serial.print("Intervalle: ");
    Serial.print(SEND_INTERVAL / 60000);
    Serial.println(" minutes");
    Serial.println(String(80, '=') + "\n");
}

// ==================== FONCTION D'ENVOI PRINCIPALE ====================
void sendSensorDataToBackend() {
    sendAttemptCount++;
    
    Serial.println("\n" + String(60, '-'));
    Serial.println("ENVOI #" + String(sendAttemptCount) + " - " + String(millis() / 1000) + "s");
    Serial.println(String(60, '-'));
    
    // 1. LECTURE DES CAPTEURS
    Serial.println("[1/4] Lecture capteurs...");
    
    int soilRaw = soilSensor.readRaw();
    float airTemp = envSensor.readTemperature();
    float airHum = envSensor.readHumidity();
    
    // Vérifier la validité des données DHT
    if (isnan(airTemp) || isnan(airHum)) {
        Serial.println("❌ ERREUR: Données DHT invalides!");
        blinkLED(3, 500);
        return;
    }
    
    // 2. CONSTRUCTION DU JSON (CONFORME CONTRAT PAGE 5)
    Serial.println("[2/4] Construction JSON...");
    
    String payload = "{";
    payload += "\"node_id\":\"" + String(NODE_ID) + "\",";
    payload += "\"soil_raw\":" + String(soilRaw) + ",";
    payload += "\"air_temp\":" + String(airTemp, 1) + ",";  // 1 décimale
    payload += "\"air_hum\":" + String(airHum, 1);         // 1 décimale
    // Timestamp optionnel - géré par le backend (info d'Emeric)
    payload += "}";
    
    Serial.print("Payload: ");
    Serial.println(payload);
    
    // 3. PRÉPARATION REQUÊTE HTTP
    Serial.println("[3/4] Préparation requête...");
    
    WiFiClient client;  // HTTP pour test local
    // Pour HTTPS plus tard: WiFiClientSecure client;
    // client.setInsecure(); // Pour développement seulement
    
    HTTPClient http;
    
    #ifdef SERIAL_DEBUG
    Serial.print("Connexion à: ");
    Serial.println(serverUrl);
    #endif
    
    if (!http.begin(client, serverUrl)) {
        Serial.println("❌ ERREUR: Impossible de commencer HTTP");
        blinkLED(5, 100);
        return;
    }
    
    // Headers conformes au contrat
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Device-Token", DEVICE_TOKEN);
    http.addHeader("User-Agent", "ESP8266-Irrigation-Collect/1.0");
    
    // 4. ENVOI DE LA REQUÊTE
    Serial.println("[4/4] Envoi POST...");
    
    int httpCode = http.POST(payload);
    
    // 5. TRAITEMENT DE LA RÉPONSE
    if (httpCode > 0) {
        Serial.printf("Code HTTP: %d\n", httpCode);
        
        if (httpCode == HTTP_CODE_OK || httpCode == HTTP_CODE_CREATED) {
            String response = http.getString();
            Serial.print("Réponse backend: ");
            Serial.println(response);
            Serial.println("✅ SUCCÈS: Données envoyées au backend");
            digitalWrite(LED_BUILTIN, HIGH); // LED éteinte
        } else {
            Serial.printf("ERREUR backend: %s\n",
                        http.errorToString(httpCode).c_str());
            digitalWrite(LED_BUILTIN, LOW); // LED allumée
            blinkLED(2, 300);
        }
    } else {
        Serial.printf("ERREUR connexion: %s\n",
                    http.errorToString(httpCode).c_str());
        digitalWrite(LED_BUILTIN, LOW); // LED allumée
        blinkLED(5, 100);
    }
    
    http.end();
    Serial.println(String(60, '-') + "\n");
}

// ==================== SETUP ====================
void setup() {
    // 1. INITIALISATION SÉRIE
    Serial.begin(115200);
    delay(1000); // Attente stabilisation
    
    Serial.println("\n\n" + String(80, '='));
    Serial.println("DÉMARRAGE SYSTÈME IRRIGATION - IoT COLLECTE");
    Serial.println(String(80, '='));
    
    // 2. CONFIGURATION LED
    pinMode(LED_BUILTIN, OUTPUT);
    digitalWrite(LED_BUILTIN, HIGH); // Éteindre
    
    // 3. SÉQUENCE DÉMARRAGE
    blinkLED(2, 100);
    
    // 4. WIFIMANAGER (configuration réseau)
    Serial.println("\n[1/5] Configuration WiFi...");
    
    WiFiManagerParameter custom_url("backend_url",
                                  "URL du Backend",
                                  serverUrl,
                                  sizeof(serverUrl));
    
    wifiManager.addParameter(&custom_url);
    
    wifiManager.setConfigPortalTimeout(180); // 3 minutes
    wifiManager.setConnectTimeout(30);      // 30 secondes
    wifiManager.setDebugOutput(SERIAL_DEBUG);
    
    // Nom du point d'accès de configuration
    if (!wifiManager.autoConnect("SmartIrrigation-Config", "config123")) {
        Serial.println("❌ Échec configuration WiFi");
        Serial.println("Redémarrage dans 5 secondes...");
        delay(5000);
        ESP.restart();
    }
    
    // Récupérer l'URL configurée
    strcpy(serverUrl, custom_url.getValue());
    Serial.print("✓ WiFi connecté: ");
    Serial.println(WiFi.SSID());
    
    // 5. INITIALISATION CAPTEURS
    Serial.println("[2/5] Initialisation capteurs...");
    soilSensor.begin();
    envSensor.begin();
    
    // 6. AFFICHAGE INFOS SYSTÈME
    Serial.println("[3/5] Vérification système...");
    printSystemInfo();
    
    // 7. TEST INITIAL CAPTEURS
    Serial.println("[4/5] Test capteurs...");
    soilSensor.readRaw();
    envSensor.readTemperature();
    envSensor.readHumidity();
    
    // 8. PRÉPARATION PREMIER ENVOI
    Serial.println("[5/5] Préparation terminée");
    systemInitialized = true;
    
    blinkLED(3, 150);
    Serial.println("\n✅ SYSTÈME PRÊT - Premier envoi dans 10 minutes\n");
}

// ==================== LOOP PRINCIPALE ====================
void loop() {
    if (!systemInitialized) {
        delay(1000);
        return;
    }
    
    unsigned long currentMillis = millis();
    
    // ENVOI PÉRIODIQUE TOUTES LES 10 MINUTES
    if (currentMillis - lastSendTime >= SEND_INTERVAL) {
        sendSensorDataToBackend();
        lastSendTime = currentMillis;
    }
    
    // VÉRIFICATION WIFI PÉRIODIQUE
    static unsigned long lastWifiCheck = 0;
    if (currentMillis - lastWifiCheck >= 30000) { // Toutes les 30 secondes
        if (WiFi.status() != WL_CONNECTED) {
            Serial.println("[WARNING] Reconnexion WiFi...");
            WiFi.reconnect();
            delay(1000);
            
            if (WiFi.status() == WL_CONNECTED) {
                Serial.println("✓ WiFi reconnecté");
            }
        }
        lastWifiCheck = currentMillis;
    }
    
    // PETITE PAUSE POUR ÉVITER SURCHARGE
    delay(100);
}