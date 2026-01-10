// EnvSensor.cpp
// Implémentation de la classe EnvSensor

#include "EnvSensor.h"
#include "config.h"

EnvSensor::EnvSensor(uint8_t dhtPin, uint8_t dhtType)
    : dht(dhtPin, dhtType), pin(dhtPin), type(dhtType),
    lastTemperature(NAN), lastHumidity(NAN), initialized(false) {
}

void EnvSensor::begin() {
    dht.begin();
    initialized = true;
    
    #ifdef SERIAL_DEBUG
    Serial.print("[EnvSensor] Initialisé DHT sur pin ");
    Serial.print(pin);
    Serial.print(", type: ");
    Serial.println(type == DHT11 ? "DHT11" : "DHT22");
    #endif
}

float EnvSensor::readTemperature() {
    if (!initialized) {
        begin();
    }
    
    lastTemperature = dht.readTemperature();
    
    if (isnan(lastTemperature)) {
        #ifdef SERIAL_DEBUG
        Serial.println("[EnvSensor] ERREUR: Lecture température échouée");
        #endif
        return NAN;
    }
    
    #ifdef SERIAL_DEBUG
    Serial.print("[EnvSensor] Température: ");
    Serial.print(lastTemperature);
    Serial.println(" °C");
    #endif
    
    return lastTemperature;
}

float EnvSensor::readHumidity() {
    if (!initialized) {
        begin();
    }
    
    lastHumidity = dht.readHumidity();
    
    if (isnan(lastHumidity)) {
        #ifdef SERIAL_DEBUG
        Serial.println("[EnvSensor] ERREUR: Lecture humidité échouée");
        #endif
        return NAN;
    }
    
    #ifdef SERIAL_DEBUG
    Serial.print("[EnvSensor] Humidité: ");
    Serial.print(lastHumidity);
    Serial.println(" %");
    #endif
    
    return lastHumidity;
}

bool EnvSensor::read(float &temperature, float &humidity) {
    temperature = readTemperature();
    humidity = readHumidity();
    
    return !isnan(temperature) && !isnan(humidity);
}

float EnvSensor::getLastTemperature() const {
    return lastTemperature;
}

float EnvSensor::getLastHumidity() const {
    return lastHumidity;
}

bool EnvSensor::isInitialized() const {
    return initialized;
}