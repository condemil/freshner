#pragma once

#include <Arduino.h>

#include "config.hpp"
#include "elapsedMillis.hpp"
#include "motor.hpp"

int buttonPressed = false;
int previousButtonRead = LOW;
elapsedMillis buttonPressTimeElapsed;
const unsigned int debounceDelay = 100;

// int pressedState = HIGH;  // pulldown button
// int releasedState = LOW; // pulldown button

int pressedState = LOW;  // pullup button
int releasedState = HIGH; // pullup button


enum class ButtonType {
    pullup,
    pulldown,
};

void buttonSetup(){
    pinMode(BUTTON_PIN, INPUT);
}

void buttonPressedEvent() {
    Serial.println("Button pressed");
    startSpin();
}

void buttonHandle() {
    int buttonRead = digitalRead(BUTTON_PIN);

    if (buttonRead == pressedState && !buttonPressed) {
        if (previousButtonRead == releasedState) {
            // Button press started, reset timer
            Serial.println("Button press started");
            buttonPressTimeElapsed = 0;
        } else if (buttonPressTimeElapsed >= debounceDelay) {
            // Button pressed long enough witout bouncing,
            // threat is as actual press
            buttonPressedEvent();
            buttonPressed = true;
        }
    } else if (buttonRead == releasedState && buttonPressed) {
        // Button is released
        buttonPressed = false;
        Serial.println("Button press over");
    }

    previousButtonRead = buttonRead;
}
