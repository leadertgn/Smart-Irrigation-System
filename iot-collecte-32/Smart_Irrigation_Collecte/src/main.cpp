#include <Arduino.h>
#include <WiFi.h>
#include <HTTPClient.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <DHT.h>
#include "config.h"

// --- Variables pour les paramètres personnalisés ---
char custom_backend_url[128];
char custom_dht_type[3];
char custom_iot_token[64];

// --- Objets ---
DHT* dht;
WiFiManager wm;
bool shouldSaveConfig = false;

// Variables pour les moyennes
float sum_temp = 0, sum_hum = 0;
long sum_soil1 = 0, sum_soil2 = 0;
int measure_count = 0;
unsigned long last_measure_time = 0;

void setup() {
    Serial.begin(115200);
    delay(1000);
    Serial.println("\n--- Unité de Collecte ESP32 ---");

    // 1. Préparation des champs personnalisés WiFiManager avec les valeurs de config.h
    WiFiManagerParameter custom_url_param("url", "URL Backend", DEFAULT_BACKEND_URL, 128);
    WiFiManagerParameter custom_dht_param("dht", "Type DHT (11/22)", DEFAULT_DHT_TYPE, 3);
    WiFiManagerParameter custom_token_param("token", "IoT Secret Token", DEFAULT_IOT_TOKEN, 64);
    
    wm.addParameter(&custom_url_param);
    wm.addParameter(&custom_dht_param);
    wm.addParameter(&custom_token_param);

    // 2. Connexion au WiFi (ou lancement du portail Smart_Collecte_AP)
    if (!wm.autoConnect("Smart_Collecte_AP")) {
        Serial.println("❌ Échec de connexion, redémarrage...");
        delay(3000);
        ESP.restart();
    }

    // 3. Récupération des valeurs (soit par défaut, soit saisies sur le portail)
    strcpy(custom_backend_url, custom_url_param.getValue());
    strcpy(custom_dht_type, custom_dht_param.getValue());
    strcpy(custom_iot_token, custom_token_param.getValue());

    // 4. Initialisation dynamique du capteur DHT
    int type = (atoi(custom_dht_type) == 22) ? DHT22 : DHT11;
    dht = new DHT(PIN_DHT, type);
    dht->begin();

    Serial.println("🌐 WiFi Connecté !");
    Serial.printf("🔗 URL: %s\n🔑 Token: %s\n🌡️ DHT: %d\n", custom_backend_url, custom_iot_token, type);
}

void sendDataToBackend(float t, float h, int s1, int s2) {
    if (WiFi.status() == WL_CONNECTED) {
        HTTPClient http;
        http.begin(custom_backend_url);
        
        // Ajout des Headers de sécurité
        http.addHeader("Content-Type", "application/json");
        http.addHeader("x-device-token", custom_iot_token);

        // Construction du JSON conforme à ton backend
        JsonDocument doc;
        doc["air_temp"] = serialized(String(t, 1));
        doc["air_hum"] = serialized(String(h, 1));
        
        JsonArray zones = doc["zones"].to<JsonArray>();
        
        JsonObject z1 = zones.add<JsonObject>();
        z1["id"] = "z1";
        z1["raw"] = s1;

        JsonObject z2 = zones.add<JsonObject>();
        z2["id"] = "z2";
        z2["raw"] = s2;

        String requestBody;
        serializeJson(doc, requestBody);

        Serial.print("📤 Envoi : ");
        serializeJson(doc, Serial);
        Serial.println();

        int httpResponseCode = http.POST(requestBody);

        if (httpResponseCode >= 200 && httpResponseCode < 300) {
            Serial.printf("✅ Succès (%d)\n", httpResponseCode);
        } else {
          Serial.printf("❌ Erreur %d : %s\n", httpResponseCode, HTTPClient::errorToString(httpResponseCode).c_str());        }
        http.end();
    }
}

void loop() {
    unsigned long currentMillis = millis();

    // Prise de mesure tous les MEASURE_INTERVAL_MS (5s)
    if (currentMillis - last_measure_time >= MEASURE_INTERVAL_MS) {
        last_measure_time = currentMillis;

        float t = dht->readTemperature();
        float h = dht->readHumidity();
        int s1 = analogRead(PIN_SOIL_Z1);
        int s2 = analogRead(PIN_SOIL_Z2);

        if (!isnan(t) && !isnan(h)) {
            sum_temp += t;
            sum_hum += h;
            sum_soil1 += s1;
            sum_soil2 += s2;
            measure_count++;

            Serial.printf("📊 [%d/%d] T:%.1f°C | H:%.1f%% | S1:%d | S2:%d\n", 
                          measure_count, NB_MEASURES_FOR_AVG, t, h, s1, s2);
        }

        // Envoi après NB_MEASURES_FOR_AVG (4 mesures = 20s)
        if (measure_count >= NB_MEASURES_FOR_AVG) {
            sendDataToBackend(sum_temp/measure_count, sum_hum/measure_count, 
                             sum_soil1/measure_count, sum_soil2/measure_count);
            
            // Réinitialisation
            sum_temp = 0; sum_hum = 0; sum_soil1 = 0; sum_soil2 = 0;
            measure_count = 0;
        }
    }
}