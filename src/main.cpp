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

void onMQTTCommand(uint8_t);
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
    mqtt::setup(onMQTTCommand);
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

void onMQTTCommand(uint8_t command) {
    switch (command) {
    case mqtt::COMMAND_RESET:
        config::truncate();
        ESP.restart();
        break;
    case mqtt::COMMAND_RESTART:
        ESP.restart();
        break;
    case mqtt::COMMAND_ON:
        motor::startSpin();
        break;
    }
}

void onMotor(bool enabled) {
    if (enabled) {
        mqtt::publish(config::MQTT_TOPIC_STATE, mqtt::MESSAGE_ON);
    }

    if (!enabled) {
        mqtt::publish(config::MQTT_TOPIC_STATE, mqtt::MESSAGE_OFF);
    }
}
