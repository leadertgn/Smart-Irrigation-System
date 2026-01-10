#include "EnvSensor.h"

EnvSensor::EnvSensor(uint8_t pin, uint8_t type) : dht(pin, type) {
  dht.begin();
  Serial.println("[EnvSensor] Initialized");
}

float EnvSensor::getTemperature() {
  float temp = dht.readTemperature();
  if (isnan(temp)) {
    Serial.println("[EnvSensor] Failed to read temperature!");
    return -999;
  }
  Serial.print("[EnvSensor] Temperature: ");
  Serial.println(temp);
  return temp;
}

float EnvSensor::getHumidity() {
  float hum = dht.readHumidity();
  if (isnan(hum)) {
    Serial.println("[EnvSensor] Failed to read humidity!");
    return -999;
  }
  Serial.print("[EnvSensor] Humidity: ");
  Serial.println(hum);
  return hum;
}