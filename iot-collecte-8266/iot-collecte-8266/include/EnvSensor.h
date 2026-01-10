#ifndef ENVSENSOR_H
#define ENVSENSOR_H

#include <Arduino.h>
#include <DHT.h>

class EnvSensor {
  private:
    DHT dht;
    
  public:
    EnvSensor(uint8_t pin, uint8_t type);
    float getTemperature();
    float getHumidity();
};

#endif