#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "elapsedMillis.hpp"

namespace motor {
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
        pinMode(MOTOR_PIN_ON, OUTPUT);
        pinMode(MOTOR_PIN_CW, OUTPUT);
        pinMode(MOTOR_PIN_CCW, OUTPUT);
        digitalWrite(MOTOR_PIN_ON, LOW);
        digitalWrite(MOTOR_PIN_CW, LOW);
        digitalWrite(MOTOR_PIN_CCW, LOW);
    }

    void startSpin() {
        if (state != State::off) {
            return;
        }

        digitalWrite(MOTOR_PIN_ON, HIGH); // Enable motor driver
        digitalWrite(MOTOR_PIN_CW, HIGH);
        Serial.println("Motor spin CW");
        state = State::cw;
        spinTimeElapsed = 0; // Reset timer
    }

    void handle() {
        if (state == State::off) {
            return;
        }

        if (state == State::cw && spinTimeElapsed >= motorCWSpinInterval) {
            // change direction
            digitalWrite(MOTOR_PIN_CCW, HIGH);
            digitalWrite(MOTOR_PIN_CW, LOW);
            Serial.println("Motor spin CCW");
            state = State::ccw;
            spinTimeElapsed = 0; // Reset timer
            delay(motorCWSpinInterval); // Make synchronous to prevent power-consuming WiFi transmit
        }
        else if (state == State::ccw && spinTimeElapsed >= motorCCWSpinInterval) {
            // brake
            digitalWrite(MOTOR_PIN_CW, HIGH);
            Serial.println("Motor brake");
            state = State::brake;
            spinTimeElapsed = 0; // Reset timer
            delay(motorCCWSpinInterval); // Make synchronous to prevent power-consuming WiFi transmit
        }
        else if (state == State::brake && spinTimeElapsed >= motorBrakeInterval) {
            // stop
            digitalWrite(MOTOR_PIN_CW, LOW);
            digitalWrite(MOTOR_PIN_CCW, LOW);
            digitalWrite(MOTOR_PIN_ON, LOW); // Disable motor driver
            Serial.println("Motor stop");
            state = State::off;
            delay(motorBrakeInterval); // Make synchronous to prevent power-consuming WiFi transmit
        }
    }
}
