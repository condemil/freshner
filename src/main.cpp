#include <Arduino.h>

#include "button.hpp"
#include "config.hpp"
#include "logger.hpp"
#include "motor.hpp"
#include "mqtt.hpp"
#include "ota.hpp"
#include "pir.hpp"
#include "wifi.hpp"

Button button;

void onButtonClick();
void onMotor(bool);

void setup() {
    pinMode(1, OUTPUT); // GPIO 1 is used as TX
    logger::setup();
    logger::debugln(F("\nmain: started"));

    config::setup();
    wifi::setup();

    if (strlen(config::conf.syslog_host) != 0) {
        logger::setupSyslog(config::conf.syslog_host, config::conf.syslog_port, config::HOSTNAME, config::NAME);
    }

    ota::setup();
    mqtt::setup();
    pir::setup();
    button.setup(ButtonType::pullup, config::IO_BUTTON, onButtonClick);
    motor::setup(onMotor);

    logger::debugln(F("main: setup is over"));
}

void loop() {
    wifi::handle();
    ota::handle();
    mqtt::handle();
    pir::handle();
    button.handle();
    motor::handle();
}

void onButtonClick() {
    motor::startSpin();
}

void onMotor(bool enabled) {
    if (enabled) {
        mqtt::publish(config::MQTT_TOPIC_STATE, "ON", true);
    }

    if (!enabled) {
        mqtt::publish(config::MQTT_TOPIC_STATE, "OFF", true);
    }
}
