#ifndef WATERINGSYSTEM_H
#define WATERINGSYSTEM_H

#include <Arduino.h>
#include "config.h"

class WateringSystem {
private:
    bool z1_active, z2_active;
    unsigned long z1_startTime, z2_startTime;
    const int MARGIN_SEC = 5; // Marge de sécurité de 5 secondes

public:
    WateringSystem() : z1_active(false), z2_active(false), z1_startTime(0), z2_startTime(0) {}

    void begin() {
        pinMode(PIN_PUMP, OUTPUT);
        pinMode(PIN_EV_Z1, OUTPUT);
        pinMode(PIN_EV_Z2, OUTPUT);
        stopAll();
    }

    // Retourne true si une coupure de sécurité a eu lieu
    bool updateZones(bool target1, bool target2, int dur1, int dur2) {
        unsigned long now = millis();
        bool safetyCutOccurred = false;

        // Gestion Zone 1
        if (target1 && !z1_active) z1_startTime = now; 
        z1_active = target1;
        if (z1_active && (now - z1_startTime > (dur1 + MARGIN_SEC) * 1000)) {
            z1_active = false;
            safetyCutOccurred = true;
        }

        // Gestion Zone 2
        if (target2 && !z2_active) z2_startTime = now;
        z2_active = target2;
        if (z2_active && (now - z2_startTime > (dur2 + MARGIN_SEC) * 1000)) {
            z2_active = false;
            safetyCutOccurred = true;
        }

        digitalWrite(PIN_EV_Z1, z1_active ? RELAY_ON : RELAY_OFF);
        digitalWrite(PIN_EV_Z2, z2_active ? RELAY_ON : RELAY_OFF);
        digitalWrite(PIN_PUMP, (z1_active || z2_active) ? RELAY_ON : RELAY_OFF);

        return safetyCutOccurred;
    }

    void stopAll() {
        digitalWrite(PIN_PUMP, RELAY_OFF);
        digitalWrite(PIN_EV_Z1, RELAY_OFF);
        digitalWrite(PIN_EV_Z2, RELAY_OFF);
        z1_active = z2_active = false;
        z1_startTime = z2_startTime = 0;
    }
};
#endif