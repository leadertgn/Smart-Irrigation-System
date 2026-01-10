// SoilSensor.cpp
// Implémentation de la classe SoilSensor

#include "SoilSensor.h"
#include "config.h"

SoilSensor::SoilSensor(uint8_t sensorPin)
    : pin(sensorPin), lastReading(0), initialized(false) {
}

void SoilSensor::begin() {
    if (pin != A0 && pin != 17) { // A0 = 17 sur ESP8266
        Serial.println("[SoilSensor] ERREUR: Pin doit être A0 (analogique)");
        return;
    }
    
    // Pas besoin de pinMode pour A0 (entrée analogique)
    initialized = true;
    
    #ifdef SERIAL_DEBUG
    Serial.print("[SoilSensor] Initialisé sur pin A0 (");
    Serial.print(pin);
    Serial.println(")");
    #endif
}

int SoilSensor::readRaw() {
    if (!initialized) {
        begin();
    }
    
    lastReading = analogRead(pin);
    
    #ifdef SERIAL_DEBUG
    Serial.print("[SoilSensor] Lecture brute: ");
    Serial.println(lastReading);
    #endif
    
    return lastReading;
}

int SoilSensor::getLastReading() const {
    return lastReading;
}

bool SoilSensor::isInitialized() const {
    return initialized;
}