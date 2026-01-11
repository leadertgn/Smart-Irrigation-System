#include "PumpController.h"

PumpController::PumpController(uint8_t pin1, uint8_t pin2) 
    : pinPump1(pin1), pinPump2(pin2), 
      pump1State(false), pump2State(false),
      lastCommandTime(0) {
}

void PumpController::begin() {
    pinMode(pinPump1, OUTPUT);
    pinMode(pinPump2, OUTPUT);
    digitalWrite(pinPump1, HIGH);
    digitalWrite(pinPump2, HIGH);
    lastCommandTime = millis();
}

void PumpController::activatePump(uint8_t pumpNumber) {
    if (pumpNumber == 1) {
        digitalWrite(pinPump1, LOW);
        pump1State = true;
        Serial.println("[PUMP] Pompe 1 ACTIVEE");
    } else if (pumpNumber == 2) {
        digitalWrite(pinPump2, LOW);
        pump2State = true;
        Serial.println("[PUMP] Pompe 2 ACTIVEE");
    }
    lastCommandTime = millis();
}

void PumpController::deactivatePump(uint8_t pumpNumber) {
    if (pumpNumber == 1) {
        digitalWrite(pinPump1, HIGH);
        pump1State = false;
        Serial.println("[PUMP] Pompe 1 DESACTIVEE");
    } else if (pumpNumber == 2) {
        digitalWrite(pinPump2, HIGH);
        pump2State = false;
        Serial.println("[PUMP] Pompe 2 DESACTIVEE");
    }
    lastCommandTime = millis();
}

void PumpController::safetyStop() {
    digitalWrite(pinPump1, HIGH);
    digitalWrite(pinPump2, HIGH);
    pump1State = false;
    pump2State = false;
    Serial.println("[SAFETY] ARRET D'URGENCE - Toutes les pompes coupees");
}

void PumpController::updateState(bool p1, bool p2) {
    if (p1 != pump1State) {
        if (p1) activatePump(1);
        else deactivatePump(1);
    }
    if (p2 != pump2State) {
        if (p2) activatePump(2);
        else deactivatePump(2);
    }
}

bool PumpController::checkSafetyTimeout(unsigned long timeoutMs) {
    unsigned long currentTime = millis();
    unsigned long elapsed = (currentTime >= lastCommandTime) 
                          ? (currentTime - lastCommandTime) 
                          : (ULONG_MAX - lastCommandTime + currentTime);
    
    if (elapsed > timeoutMs) {
        safetyStop();
        return true;
    }
    return false;
}