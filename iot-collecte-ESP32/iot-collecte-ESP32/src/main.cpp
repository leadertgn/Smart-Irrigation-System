// ===========================================
// ESP32 IoT COLLECT - MAIN CODE
// 2 Soil Sensors + DHT22 + WiFiManager
// ===========================================

#include <Arduino.h>
#include <WiFi.h>
#include <WiFiManager.h>
#include <HTTPClient.h>
#include <WiFiClientSecure.h>
#include <ArduinoJson.h>
#include "SoilSensor.h"
#include "EnvSensor.h"
#include "config.h"

// ============== GLOBAL OBJECTS ==============
SoilSensor soilSensor;
EnvSensor envSensor;
WiFiManager wifiManager;

// ============== GLOBAL VARIABLES ==============
unsigned long lastSendTime = 0;
bool systemReady = false;
String deviceToken = DEVICE_TOKEN;

// ============== LED FUNCTIONS ==============
void setupLED() {
    pinMode(LED_PIN, OUTPUT);
    digitalWrite(LED_PIN, LOW); // ESP32: LOW = OFF
}

void blinkLED(int times, int delayMs = 200) {
    for (int i = 0; i < times; i++) {
        digitalWrite(LED_PIN, HIGH);
        delay(delayMs);
        digitalWrite(LED_PIN, LOW);
        delay(delayMs);
    }
}

// ============== WIFI SETUP ==============
void setupWiFi() {
    Serial.println("\n[WiFi] Setting up...");
    
    // WiFiManager configuration
    wifiManager.setConfigPortalTimeout(180); // 3 minutes
    wifiManager.setConnectTimeout(30);       // 30 seconds
    
    // Try to connect to saved WiFi
    if (!wifiManager.autoConnect("ESP32-Irrigation", "password123")) {
        Serial.println("[WiFi] Failed to connect");
        blinkLED(5, 500);
        ESP.restart();
    }
    
    Serial.println("[WiFi] Connected!");
    Serial.print("SSID: ");
    Serial.println(WiFi.SSID());
    Serial.print("IP: ");
    Serial.println(WiFi.localIP());
}

// ============== JSON CREATION ==============
String createJSON(int soil1, int soil2, float temp, float hum) {
    StaticJsonDocument<300> doc;
    
    doc["node_id"] = "ESP32_ZONE_DOUBLE";
    doc["soil_raw_1"] = soil1;
    doc["soil_raw_2"] = soil2;
    doc["soil_percent_1"] = soilSensor.getPercentage1();
    doc["soil_percent_2"] = soilSensor.getPercentage2();
    doc["air_temp"] = round(temp * 10) / 10.0; // 1 decimal
    doc["air_hum"] = round(hum * 10) / 10.0;   // 1 decimal
    
    String output;
    serializeJson(doc, output);
    return output;
}

// ============== SEND DATA TO BACKEND ==============
bool sendToBackend() {
    Serial.println("\n=== SENDING DATA ===");
    
    // 1. Read sensors
    soilSensor.readSensors();
    int soil1 = soilSensor.getValue1();
    int soil2 = soilSensor.getValue2();
    float temp = envSensor.readTemperature();
    float hum = envSensor.readHumidity();
    
    if (isnan(temp) || isnan(hum)) {
        Serial.println("ERROR: Invalid DHT data");
        blinkLED(3, 500);
        return false;
    }
    
    // 2. Create JSON
    String jsonData = createJSON(soil1, soil2, temp, hum);
    Serial.print("JSON: ");
    Serial.println(jsonData);
    
    // 3. Prepare HTTPS request
    WiFiClientSecure client;
    client.setInsecure(); // Skip certificate verification for now
    
    HTTPClient http;
    http.begin(client, BACKEND_URL);
    http.addHeader("Content-Type", "application/json");
    http.addHeader("X-Device-Token", deviceToken);
    
    // 4. Send POST request
    int httpCode = http.POST(jsonData);
    
    // 5. Handle response
    if (httpCode > 0) {
        Serial.printf("HTTP Code: %d\n", httpCode);
        
        if (httpCode == 200 || httpCode == 201) {
            String response = http.getString();
            Serial.print("Response: ");
            Serial.println(response);
            Serial.println("SUCCESS! Data sent.");
            blinkLED(2, 100);
            return true;
        } else {
            Serial.printf("Error: %s\n", http.errorToString(httpCode).c_str());
            blinkLED(5, 100);
        }
    } else {
        Serial.printf("Failed: %s\n", http.errorToString(httpCode).c_str());
        blinkLED(5, 100);
    }
    
    http.end();
    return false;
}

// ============== SETUP ==============
void setup() {
    // 1. Serial communication
    Serial.begin(115200);
    delay(1000);
    
    Serial.println("\n\n==================================");
    Serial.println("   ESP32 IoT COLLECT SYSTEM");
    Serial.println("==================================");
    
    // 2. LED setup
    setupLED();
    blinkLED(3, 150);
    
    // 3. WiFi setup
    setupWiFi();
    
    // 4. Initialize sensors
    Serial.println("\n[System] Initializing sensors...");
    soilSensor.begin();
    envSensor.begin();
    
    // 5. Test sensors
    Serial.println("[System] Testing sensors...");
    soilSensor.readSensors();
    envSensor.readTemperature();
    envSensor.readHumidity();
    
    // 6. System ready
    systemReady = true;
    Serial.println("\n✅ SYSTEM READY");
    Serial.println("First send in 10 minutes...");
}

// ============== LOOP ==============
void loop() {
    if (!systemReady) {
        delay(1000);
        return;
    }
    
    // Send data every 10 minutes
    if (millis() - lastSendTime >= SEND_INTERVAL) {
        if (WiFi.status() == WL_CONNECTED) {
            sendToBackend();
        } else {
            Serial.println("[WiFi] Reconnecting...");
            WiFi.reconnect();
        }
        lastSendTime = millis();
    }
    
    // Check WiFi every 30 seconds
    static unsigned long lastCheck = 0;
    if (millis() - lastCheck >= 30000) {
        if (WiFi.status() != WL_CONNECTED) {
            WiFi.reconnect();
        }
        lastCheck = millis();
    }
    
    // Small delay
    delay(100);
}