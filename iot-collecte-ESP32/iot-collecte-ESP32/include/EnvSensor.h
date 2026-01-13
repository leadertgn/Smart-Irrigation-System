// EnvSensor.h - DHT22 for ESP32
#ifndef ENVSENSOR_H
#define ENVSENSOR_H

#include <DHT.h>

class EnvSensor {
private:
    DHT dht;
    float lastTemp, lastHum;
    bool isReady;
    
public:
    // Constructor
    EnvSensor(uint8_t pin = 4, uint8_t type = DHT22);
    
    // Initialize
    void begin();
    
    // Read temperature (°C)
    float readTemperature();
    
    // Read humidity (%)
    float readHumidity();
    
    // Get last values
    float getTemperature() const;
    float getHumidity() const;
    
    // Check if ready
    bool ready() const;
};

#endif