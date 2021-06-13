#pragma once

#include <PubSubClient.h>
#include <WiFiClient.h>

#include "config.hpp"
#include "elapsedMillis.hpp"
#include "logger.hpp"
#include "motor.hpp"
#include "wifi.hpp"

namespace mqtt {
const uint16_t RECONNECT_DELAY = 5000;
const uint8_t COMMAND_RESET = 0;
const uint8_t COMMAND_RESTART = 1;
const uint8_t COMMAND_ON = 2;
const uint8_t WILL_QOS = 1;
const bool WILL_RETAIN = true;
static const char *WILL_PAYLOAD_AVAILABLE = "online";
static const char *WILL_PAYLOAD_NOT_AVAILABLE = "offline";
static const char *MESSAGE_ON = "ON";
static const char *MESSAGE_OFF = "OFF";

std::function<void(uint8_t)> _onCommand;

WiFiClient _esp_client;
PubSubClient _client(_esp_client);
elapsedMillis _reconnect_time_elapsed;
bool connected = false;

void publish(const char *topic, const char *message) {
    _client.publish(topic, message, true);
}

void _callback(char *topic, byte *payload, unsigned int length) {
    if (length >= MQTT_MAX_PACKET_SIZE) {
        logger::errorln(F("mqtt: payload is too long"));
        return;
    }

    if (strncmp((char *)payload, "RESET", length) == 0) {
        _onCommand(COMMAND_RESET);
    } else if (strncmp((char *)payload, "RESTART", length) == 0) {
        _onCommand(COMMAND_RESTART);
    } else if (strncmp((char *)payload, "ON", length) == 0) {
        _onCommand(COMMAND_ON);
    }

    logger::debugf("mqtt: message arrived [%s]: %s\n", topic, payload);
}

void _reconnect() {
    logger::debugln(F("mqtt: attempting to connect"));

    connected = _client.connect(config::HOSTNAME, config::conf.mqtt_login, config::conf.mqtt_pass,
        config::MQTT_TOPIC_AVAILABILITY, WILL_QOS, WILL_RETAIN, WILL_PAYLOAD_NOT_AVAILABLE);

    if (connected) {
        logger::debugln(F("mqtt: connected"));
        _client.subscribe(config::MQTT_TOPIC_COMMAND);
        logger::debugf("mqtt: subscribed to %s\n", config::MQTT_TOPIC_COMMAND);
        _client.publish(config::MQTT_TOPIC_AVAILABILITY, WILL_PAYLOAD_AVAILABLE);
    } else {
        logger::debugf(
            "mqtt: connect failed, rc=%d try again in %u seconds\n", _client.state(), RECONNECT_DELAY / 1000);
    }
}

void setup(std::function<void(uint8_t)> onCommand) {
    _onCommand = onCommand;

    _client.setServer(config::conf.mqtt_host, config::conf.mqtt_port);
    _client.setCallback(_callback);
}

void handle() {
    if (_client.connected()) {
        _client.loop();
        return;
    }

    if (_reconnect_time_elapsed >= RECONNECT_DELAY) {
        _reconnect_time_elapsed = 0; // reset timer
        _reconnect();
    }
}
} // namespace mqtt
