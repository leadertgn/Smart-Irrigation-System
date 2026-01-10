// SoilSensor.h
// Classe pour le capteur d'humidité du sol (A0)
// Conforme au cahier : "Classe SoilSensor"

#ifndef SOILSENSOR_H
#define SOILSENSOR_H

#include <Arduino.h>

class SoilSensor {
private:
    uint8_t pin;
    int lastReading;
    bool initialized;

public:
    // Constructeur
    SoilSensor(uint8_t sensorPin = A0);
    
    // Initialiser le capteur
    void begin();
    
    // Lire la valeur brute (0-1023) - méthode principale
    int readRaw();
    
    // Obtenir la dernière lecture
    int getLastReading() const;
    
    // Vérifier si initialisé
    bool isInitialized() const;
};

#endif