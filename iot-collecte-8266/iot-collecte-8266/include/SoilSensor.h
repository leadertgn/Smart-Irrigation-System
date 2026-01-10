#ifndef SOILSENSOR_H
#define SOILSENSOR_H

#include <Arduino.h>

class SoilSensor {
  private:
    uint8_t pin;
    
  public:
    SoilSensor(uint8_t sensorPin);
    int readRaw();
};

#endif