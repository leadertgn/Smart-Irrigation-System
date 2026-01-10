#include "SoilSensor.h"

SoilSensor::SoilSensor(uint8_t sensorPin) {
  pin = sensorPin;
  pinMode(pin, INPUT);
  Serial.println("[SoilSensor] Initialized");
}

int SoilSensor::readRaw() {
  int value = analogRead(pin);
  Serial.print("[SoilSensor] Valeur brute: ");
  Serial.println(value);
  return value;
}