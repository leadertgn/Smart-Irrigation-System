#include "SoilSensor.h"
#include "config.h"

SoilSensor::SoilSensor(uint8_t pinA, uint8_t pinB)
    : pin1(pinA), pin2(pinB), lastValue1(0), lastValue2(0), isReady(false) {
}

void SoilSensor::begin() {
    // ESP32 analog pins don't need pinMode
    isReady = true;
    
    #if DEBUG_MODE
    Serial.print("[SoilSensor] Ready on pins ");
    Serial.print(pin1);
    Serial.print(" and ");
    Serial.println(pin2);
    #endif
}

void SoilSensor::readSensors() {
    if (!isReady) begin();
    
    lastValue1 = analogRead(pin1);
    lastValue2 = analogRead(pin2);
    
    #if DEBUG_MODE
    Serial.print("[SoilSensor] Values: ");
    Serial.print(lastValue1);
    Serial.print(" | ");
    Serial.println(lastValue2);
    #endif
}

int SoilSensor::getValue1() {
    if (lastValue1 == 0 && isReady) readSensors();
    return lastValue1;
}

int SoilSensor::getValue2() {
    if (lastValue2 == 0 && isReady) readSensors();
    return lastValue2;
}

int SoilSensor::getPercentage1() {
    // ESP32: 0=wet, 4095=dry → invert for 0%=dry, 100%=wet
    int val = getValue1();
    return map(val, 0, 4095, 100, 0);
}

int SoilSensor::getPercentage2() {
    int val = getValue2();
    return map(val, 0, 4095, 100, 0);
}

bool SoilSensor::ready() const {
    return isReady;
}