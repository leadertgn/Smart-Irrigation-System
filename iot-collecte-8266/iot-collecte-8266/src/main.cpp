#include <Arduino.h>
#include <WiFiManager.h>
#include <ESP8266HTTPClient.h>
#include <WiFiClientSecure.h>
#include "SoilSensor.h"
#include "EnvSensor.h"

// Configuration
#define SOIL_PIN A0
#define DHT_PIN D4
#define DHT_TYPE DHT22

SoilSensor soilSensor(SOIL_PIN);
EnvSensor envSensor(DHT_PIN, DHT_TYPE);

WiFiManager wm;
char serverUrl[100] = "http://localhost:3000/api/data/collect";

unsigned long lastSendTime = 0;
const long interval = 600000; // 10 minutes

void setup() {
  Serial.begin(115200);
  delay(1000);
  
  pinMode(LED_BUILTIN, OUTPUT);
  digitalWrite(LED_BUILTIN, HIGH);
  
  Serial.println("\n=== Irrigation System - ESP8266 ===");
  
  // Configuration WiFiManager
  WiFiManagerParameter custom_url("server", "Server URL", serverUrl, 100);
  wm.addParameter(&custom_url);
  
  if (!wm.autoConnect("Irrigation-AP")) {
    Serial.println("Failed to connect");
    ESP.restart();
  }
  
  strcpy(serverUrl, custom_url.getValue());
  Serial.print("Server URL: ");
  Serial.println(serverUrl);
  
  Serial.println("Setup complete!");
}

void loop() {
  if (millis() - lastSendTime >= interval) {
    // Lire les capteurs
    int soilValue = soilSensor.readRaw();
    float temp = envSensor.getTemperature();
    float hum = envSensor.getHumidity();
    
    // Créer JSON
    String json = "{";
    json += "\"node_id\":\"ESP8266_ZONE_1\",";
    json += "\"soil_raw\":" + String(soilValue) + ",";
    json += "\"air_temp\":" + String(temp) + ",";
    json += "\"air_hum\":" + String(hum);
    json += "}";
    
    Serial.print("Sending: ");
    Serial.println(json);
    
    // Envoyer HTTP
    WiFiClient client;
    HTTPClient http;
    
    http.begin(client, serverUrl);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Device-Token", "ESP8266_SECRET");
    
    int httpCode = http.POST(json);
    
    if (httpCode > 0) {
      Serial.printf("HTTP Code: %d\n", httpCode);
      digitalWrite(LED_BUILTIN, HIGH);
    } else {
      Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
      digitalWrite(LED_BUILTIN, LOW);
    }
    
    http.end();
    lastSendTime = millis();
  }
  
  delay(1000);
}