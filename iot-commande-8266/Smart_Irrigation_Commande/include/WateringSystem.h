#ifndef WATERINGSYSTEM_H
#define WATERINGSYSTEM_H

#include <Arduino.h>
#include "config.h"

class WateringSystem {
private:
    bool z1_active;
    bool z2_active;
    unsigned long startTime;

public:
    WateringSystem() : z1_active(false), z2_active(false), startTime(0) {}

    void begin() {
        pinMode(PIN_PUMP, OUTPUT);
        pinMode(PIN_EV_Z1, OUTPUT);
        pinMode(PIN_EV_Z2, OUTPUT);
        stopAll();
    }

    void updateZones(bool v1, bool v2) {
        z1_active = v1;
        z2_active = v2;

        // Action sur les Électrovannes
        digitalWrite(PIN_EV_Z1, z1_active ? RELAY_ON : RELAY_OFF);
        digitalWrite(PIN_EV_Z2, z2_active ? RELAY_ON : RELAY_OFF);

        // La pompe tourne si au moins une vanne est ouverte
        if (z1_active || z2_active) {
            if (startTime == 0) startTime = millis(); // Déclenche le chrono
            digitalWrite(PIN_PUMP, RELAY_ON);
        } else {
            stopAll();
        }
    }

    void stopAll() {
        digitalWrite(PIN_PUMP, RELAY_OFF);
        digitalWrite(PIN_EV_Z1, RELAY_OFF);
        digitalWrite(PIN_EV_Z2, RELAY_OFF);
        z1_active = false;
        z2_active = false;
        startTime = 0;
    }

    unsigned long getElapsed() {
        if (startTime == 0) return 0;
        return millis() - startTime;
    }
};

#endif