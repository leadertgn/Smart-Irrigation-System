#include "EnvSensor.h"
#include "config.h"

EnvSensor::EnvSensor(uint8_t pin, uint8_t type) 
    : dht(pin, type), lastTemp(NAN), lastHum(NAN), isReady(false) {
}

void EnvSensor::begin() {
    dht.begin();
    isReady = true;
    
    #if DEBUG_MODE
    Serial.println("[EnvSensor] DHT22 ready");
    #endif
}

float EnvSensor::readTemperature() {
    if (!isReady) begin();
    
    lastTemp = dht.readTemperature();
    
    if (isnan(lastTemp)) {
        #if DEBUG_MODE
        Serial.println("[EnvSensor] Temperature error");
        #endif
        return NAN;
    }
    
    #if DEBUG_MODE
    Serial.print("[EnvSensor] Temperature: ");
    Serial.print(lastTemp);
    Serial.println(" °C");
    #endif
    
    return lastTemp;
}

float EnvSensor::readHumidity() {
    if (!isReady) begin();
    
    lastHum = dht.readHumidity();
    
    if (isnan(lastHum)) {
        #if DEBUG_MODE
        Serial.println("[EnvSensor] Humidity error");
        #endif
        return NAN;
    }
    
    #if DEBUG_MODE
    Serial.print("[EnvSensor] Humidity: ");
    Serial.print(lastHum);
    Serial.println(" %");
    #endif
    
    return lastHum;
}

float EnvSensor::getTemperature() const {
    return lastTemp;
}

float EnvSensor::getHumidity() const {
    return lastHum;
}

bool EnvSensor::ready() const {
    return isReady;
}