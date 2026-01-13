// SoilSensor.h - ESP32 with 2 sensors
#ifndef SOILSENSOR_H
#define SOILSENSOR_H

#include <Arduino.h>

class SoilSensor {
private:
    uint8_t pin1, pin2;
    int lastValue1, lastValue2;
    bool isReady;
    
public:
    // Constructor
    SoilSensor(uint8_t pinA = 32, uint8_t pinB = 33);
    
    // Initialize
    void begin();
    
    // Read both sensors
    void readSensors();
    
    // Get values (0-4095 for ESP32)
    int getValue1();
    int getValue2();
    
    // Get percentage (0-100%)
    int getPercentage1();
    int getPercentage2();
    
    // Check if ready
    bool ready() const;
};

#endif