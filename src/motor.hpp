#pragma once

#include <Arduino.h>
#include <functional>

#include "config.hpp"
#include "elapsedMillis.hpp"
#include "logger.hpp"

namespace motor {
std::function<void(bool)> callback;

enum class State {
    start,
    cw,
    ccw,
    brake,
    off
};

State state = State::off;
elapsedMillis spinTimeElapsed;
const unsigned int motorCWSpinInterval = 1000;
const unsigned int motorCCWSpinInterval = 400;
const unsigned int motorBrakeInterval = 200;

void setup(std::function<void(bool)> motorCallback) {
    callback = motorCallback;

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

    state = State::start;
    callback(true);
}

void setMotor(uint8_t enable, uint8_t cw, uint8_t ccw) {
    digitalWrite(config::IO_MOTOR_ON, enable);
    digitalWrite(config::IO_MOTOR_CW, cw);
    digitalWrite(config::IO_MOTOR_CCW, ccw);
}

void handle() {
    if (state == State::off) {
        return;
    }

    if (state == State::start) {
        state = State::cw;
        setMotor(HIGH, HIGH, LOW);
        spinTimeElapsed = 0;
        return;
    }

    if (state == State::cw && spinTimeElapsed >= motorCWSpinInterval) {
        state = State::ccw;
        setMotor(HIGH, LOW, HIGH);
        spinTimeElapsed = 0;
        return;
    }


    if (state == State::ccw && spinTimeElapsed >= motorCCWSpinInterval) {
        state = State::brake;
        setMotor(HIGH, HIGH, HIGH);
        spinTimeElapsed = 0;
        return;
    }

    if (state == State::brake && spinTimeElapsed >= motorBrakeInterval) {
        state = State::off;
        setMotor(LOW, LOW, LOW);
        callback(false);
    }
}
} // namespace mqtt
