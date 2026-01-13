#ifndef VALVECONTROLLER_H
#define VALVECONTROLLER_H

#include <Arduino.h>

class ValveController {
private:
    uint8_t pin;
    bool activeLow;
    bool status;
    unsigned long startTime;

public:
    ValveController(uint8_t p, bool al) : pin(p), activeLow(al), status(false), startTime(0) {}

    void begin() {
        pinMode(pin, OUTPUT);
        stop(); 
    }

    void start() {
        digitalWrite(pin, activeLow ? LOW : HIGH);
        if (!status) startTime = millis();
        status = true;
    }

    void stop() {
        digitalWrite(pin, activeLow ? HIGH : LOW);
        status = false;
        startTime = 0;
    }

    bool isOn() const { return status; }
    unsigned long getElapsed() const { return status ? millis() - startTime : 0; }
};

#endif