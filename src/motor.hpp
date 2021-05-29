#pragma once

#include <Arduino.h>
#include <functional>

#include "config.hpp"
#include "elapsedMillis.hpp"
#include "logger.hpp"

namespace motor {
    std::function<void(bool)> callback;

    enum class State {
        off,
        cw,
        ccw,
        brake,
    };

    State state = State::off;
    elapsedMillis spinTimeElapsed;
    const unsigned int motorCWSpinInterval = 1000;
    const unsigned int motorCCWSpinInterval = 400;
    const unsigned int motorBrakeInterval = 200;

    void setup() {
        pinMode(config::IO_MOTOR_ON, OUTPUT);
        pinMode(config::IO_MOTOR_CW, OUTPUT);
        pinMode(config::IO_MOTOR_CCW, OUTPUT);
        digitalWrite(config::IO_MOTOR_ON, LOW);
        digitalWrite(config::IO_MOTOR_CW, LOW);
        digitalWrite(config::IO_MOTOR_CCW, LOW);
    }

    void startSpin() {
        if (state != State::off) {
            return;
        }

        if (callback) {
            callback(true);
        }

        digitalWrite(config::IO_MOTOR_ON, HIGH); // Enable motor driver
        digitalWrite(config::IO_MOTOR_CW, HIGH);
        logger::debugln(F("motor: spin CW"));
        state = State::cw;
        spinTimeElapsed = 0; // Reset timer
    }

    void handle() {
        if (state == State::off) {
            return;
        }

        if (state == State::cw && spinTimeElapsed >= motorCWSpinInterval) {
            // change direction
            digitalWrite(config::IO_MOTOR_CCW, HIGH);
            digitalWrite(config::IO_MOTOR_CW, LOW);
            logger::debugln(F("motor: spin CCW"));
            state = State::ccw;
            spinTimeElapsed = 0; // Reset timer
            delay(motorCWSpinInterval); // Make synchronous to prevent power-consuming WiFi transmit
        }
        else if (state == State::ccw && spinTimeElapsed >= motorCCWSpinInterval) {
            // brake
            digitalWrite(config::IO_MOTOR_CW, HIGH);
            logger::debugln(F("motor: brake"));
            state = State::brake;
            spinTimeElapsed = 0; // Reset timer
            delay(motorCCWSpinInterval); // Make synchronous to prevent power-consuming WiFi transmit
        }
        else if (state == State::brake && spinTimeElapsed >= motorBrakeInterval) {
            // stop
            digitalWrite(config::IO_MOTOR_CW, LOW);
            digitalWrite(config::IO_MOTOR_CCW, LOW);
            digitalWrite(config::IO_MOTOR_ON, LOW); // Disable motor driver
            logger::debugln(F("motor: stop"));
            state = State::off;
            delay(motorBrakeInterval); // Make synchronous to prevent power-consuming WiFi transmit

            if (callback) {
                callback(false);
            }
        }
    }

    void setCallback(std::function<void(bool)> motorCallback) {
        callback = motorCallback;
    }
}
