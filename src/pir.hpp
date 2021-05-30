#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "elapsedMillis.hpp"
#include "logger.hpp"
#include "motor.hpp"

namespace pir {
    bool pirActivated = false;
    unsigned int val = 0;

    elapsedMillis measureTimeElapsed;
    elapsedMillis motionTimeElapsed;
    const unsigned int measureInterval = 5 * 60 * 1000; // 5 minutes
    const unsigned int actionInterval = 50 * 1000; // 50 seconds
    unsigned int actionTime = 0;
    bool isMeasure = false;

    void firePirEvent();
    void motionStarted();
    void motionEnded();
    void startMeasure();
    void endMeasure();
    void logStats();


    void setup() {
        pinMode(config::IO_PIR, INPUT);
        pinMode(config::IO_LED, OUTPUT);
    }

    void handle() {
        val = digitalRead(config::IO_PIR);
        if (val == HIGH) {
            if (!pirActivated) {
                pirActivated = true;
                motionStarted();
            }
        } else {
            if (pirActivated) {
                pirActivated = false;
                motionEnded();
            }
        }

        if (isMeasure && measureTimeElapsed >= measureInterval) {
            endMeasure();
        }
    }

    void motionStarted() {
        digitalWrite(config::IO_LED, HIGH);
        motionTimeElapsed = 0; // Reset timer
        if (!isMeasure) {
            startMeasure();
        }
    }

    void motionEnded() {
        digitalWrite(config::IO_LED, LOW);
        actionTime += motionTimeElapsed;
        logger::debugf("pir: motion time: %lus, total action time: %us, measure time elapsed: %lus\n",
            motionTimeElapsed / 1000, actionTime / 1000, measureTimeElapsed / 1000);
    }

    void startMeasure() {
        measureTimeElapsed = 0; // Reset timer
        actionTime = 0;
        isMeasure = true;
    }

    void endMeasure() {
        if (!pirActivated) {
            isMeasure = false;
        }

        if (actionTime >= actionInterval) {
            firePirEvent();
        }

        logger::debugf("pir: measured interval: %us\n", actionTime / 1000);

        actionTime = 0;
        measureTimeElapsed = 0; // Reset timer
        motionTimeElapsed = 0; // Reset timer
    }

    void firePirEvent() {
        motor::startSpin();
    }
}
