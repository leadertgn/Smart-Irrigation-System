// EnvSensor.h
// Classe pour le capteur DHT (température/humidité air)
// Conforme au cahier : "Classe EnvSensor"

#ifndef ENVSENSOR_H
#define ENVSENSOR_H

#include <Arduino.h>
#include <DHT.h>

class EnvSensor {
private:
    DHT dht;
    uint8_t pin;
    uint8_t type;
    float lastTemperature;
    float lastHumidity;
    bool initialized;

public:
    // Constructeur
    EnvSensor(uint8_t dhtPin = D4, uint8_t dhtType = DHT22);
    
    // Initialiser le capteur
    void begin();
    
    // Lire la température (Celsius)
    float readTemperature();
    
    // Lire l'humidité relative (%)
    float readHumidity();
    
    // Lire température et humidité
    bool read(float &temperature, float &humidity);
    
    // Getters
    float getLastTemperature() const;
    float getLastHumidity() const;
    bool isInitialized() const;
};

#endif