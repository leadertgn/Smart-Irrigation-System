#include "PumpController.h"

PumpController::PumpController(uint8_t p1, uint8_t p2, bool lowTrigger)
    : _pin1(p1), _pin2(p2), _isLow(lowTrigger), _lastCmd(0) {}

void PumpController::begin() {
    pinMode(_pin1, OUTPUT);
    pinMode(_pin2, OUTPUT);
    safetyStop();
}

void PumpController::update(bool p1, bool p2) {
    // Écrire l'état en tenant compte du trigger bas/haut
    digitalWrite(_pin1, _isLow ? (p1 ? LOW : HIGH) : (p1 ? HIGH : LOW));
    digitalWrite(_pin2, _isLow ? (p2 ? LOW : HIGH) : (p2 ? HIGH : LOW));
    _lastCmd = millis(); // Reset timer à chaque commande
}

void PumpController::safetyStop() {
    digitalWrite(_pin1, _isLow ? HIGH : LOW);
    digitalWrite(_pin2, _isLow ? HIGH : LOW);
    Serial.println("[SAFETY] Pompes OFF");
}

bool PumpController::isTimeoutActive(unsigned long limit) {
    return (millis() - _lastCmd > limit);
}