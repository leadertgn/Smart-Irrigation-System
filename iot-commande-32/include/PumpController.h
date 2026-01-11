#ifndef PUMPCONTROLLER_H
#define PUMPCONTROLLER_H

#include <Arduino.h>

class PumpController {
private:
    uint8_t pinPump1;
    uint8_t pinPump2;
    bool pump1State;
    bool pump2State;
    unsigned long lastCommandTime;
    
public:
    PumpController(uint8_t pin1, uint8_t pin2);
    void begin();
    void activatePump(uint8_t pumpNumber);
    void deactivatePump(uint8_t pumpNumber);
    void safetyStop();
    void updateState(bool p1, bool p2);
    bool checkSafetyTimeout(unsigned long timeoutMs = 1200000);
    bool getPump1State() const { return pump1State; }
    bool getPump2State() const { return pump2State; }
    unsigned long getLastCommandTime() const { return lastCommandTime; }
};

#endif