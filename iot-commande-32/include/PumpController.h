#ifndef PUMPCONTROLLER_H
#define PUMPCONTROLLER_H

#include <Arduino.h>

class PumpController {
private:
    uint8_t _pin1;
    uint8_t _pin2;
    bool _isLow;
    unsigned long _lastCmd;

public:
    PumpController(uint8_t p1, uint8_t p2, bool lowTrigger);
    void begin();
    void update(bool p1, bool p2);
    void safetyStop();
    bool isTimeoutActive(unsigned long limit = 1200000);
};

#endif