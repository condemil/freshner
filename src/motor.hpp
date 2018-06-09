#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "elapsedMillis.hpp"

enum class MotorState {
    off,
    cw,
    ccw,
    brake,
};

MotorState motorState = MotorState::off;
elapsedMillis spinTimeElapsed;
const unsigned int motorCWSpinInterval = 1000;
const unsigned int motorCCWSpinInterval = 400;
const unsigned int motorBrakeInterval = 200;

void motorSetup() {
    pinMode(MOTOR_PIN_ON, OUTPUT);
    pinMode(MOTOR_PIN_CW, OUTPUT);
    pinMode(MOTOR_PIN_CCW, OUTPUT);
    digitalWrite(MOTOR_PIN_ON, LOW);
    digitalWrite(MOTOR_PIN_CW, LOW);
    digitalWrite(MOTOR_PIN_CCW, LOW);
}

void startSpin() {
    if (motorState != MotorState::off) {
        return;
    }

    digitalWrite(MOTOR_PIN_ON, HIGH); // Enable motor driver
    digitalWrite(MOTOR_PIN_CW, HIGH);
    Serial.println("Motor spin CW");
    motorState = MotorState::cw;
    spinTimeElapsed = 0; // Reset timer
}

void motorHandle() {
    if (motorState == MotorState::off) {
        return;
    }

    if (motorState == MotorState::cw && spinTimeElapsed >= motorCWSpinInterval) {
        // change direction
        digitalWrite(MOTOR_PIN_CCW, HIGH);
        digitalWrite(MOTOR_PIN_CW, LOW);
        Serial.println("Motor spin CCW");
        motorState = MotorState::ccw;
        spinTimeElapsed = 0; // Reset timer
        delay(motorCWSpinInterval); // Make synchronous to prevent power-consuming WiFi transmit
    }
    else if (motorState == MotorState::ccw && spinTimeElapsed >= motorCCWSpinInterval) {
        // brake
        digitalWrite(MOTOR_PIN_CW, HIGH);
        Serial.println("Motor brake");
        motorState = MotorState::brake;
        spinTimeElapsed = 0; // Reset timer
        delay(motorCCWSpinInterval); // Make synchronous to prevent power-consuming WiFi transmit
    }
    else if (motorState == MotorState::brake && spinTimeElapsed >= motorBrakeInterval) {
        // stop
        digitalWrite(MOTOR_PIN_CW, LOW);
        digitalWrite(MOTOR_PIN_CCW, LOW);
        digitalWrite(MOTOR_PIN_ON, LOW); // Disable motor driver
        Serial.println("Motor stop");
        motorState = MotorState::off;
        delay(motorBrakeInterval); // Make synchronous to prevent power-consuming WiFi transmit
    }
}
