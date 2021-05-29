#include <Arduino.h>

#include "button.hpp"
#include "logger.hpp"
#include "motor.hpp"
#include "mqtt.hpp"
#include "ota.hpp"
#include "pir.hpp"
#include "wifi.hpp"

void setup() {
    pinMode(1, OUTPUT); // GPIO 1 is used as TX
    logger::setup();
    logger::debugln(F("\nmain: started"));

    wifi::setup();
    ota::setup();
    mqtt::setup();
    pir::setup();
    button::setup();
    motor::setup();

    logger::debugln(F("main: setup is over"));
}

void loop() {
    ota::handle();
    mqtt::handle();
    pir::handle();
    button::handle();
    motor::handle();
}
