#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>
#include <WiFiClient.h>

#include "config.hpp"
#include "elapsedMillis.hpp"
#include "logger.hpp"
#include "motor.hpp"
#include "wifi.hpp"

namespace mqtt {
const bool WILL_RETAIN = true;
const uint8_t WILL_QOS = 1;
static const char *WILL_PAYLOAD_AVAILABLE = "online";
static const char *WILL_PAYLOAD_NOT_AVAILABLE = "offline";

WiFiClient _esp_client;
PubSubClient client(_esp_client);
elapsedMillis reconnectTimeElapsed;
const unsigned int reconnectDelay = 5000;

void publish(const char *topic, const char *message, bool retained) {
    client.publish(topic, message, retained);
}

void callback(char *topic, uint8_t *payload, unsigned int length) {
    payload[length] = '\0'; // TODO: find a better way, this will fail if payload will be exactly MQTT_MAX_PACKET_SIZE

    String s = String((char*)payload);

    if (s == "ON") {
        motor::startSpin();
    }

    logger::debugf("mqtt: message arrived [%s]: %s\n", topic, s.c_str());
}

void reconnect() {
    // Loop until we're reconnected
    logger::debugln(F("mqtt: attempting connection..."));

    if (client.connect(config::HOSTNAME, config::conf.mqtt_login, config::conf.mqtt_pass,
        config::MQTT_TOPIC_AVAILABILITY, WILL_QOS, WILL_RETAIN, WILL_PAYLOAD_NOT_AVAILABLE)) {
        logger::debugln(F("mqtt: connected"));
        client.subscribe(config::MQTT_TOPIC_COMMAND);
        client.publish(config::MQTT_TOPIC_AVAILABILITY, WILL_PAYLOAD_AVAILABLE);
    } else {
        logger::debugf("mqtt: connect failed, rc=%d try again in %u seconds\n", client.state(), reconnectDelay / 1000);
    }
}

void setup() {
    client.setServer(config::conf.mqtt_host, config::conf.mqtt_port);
    client.setCallback(callback);
}

void handle() {
    if (!client.connected()) {
        if (reconnectTimeElapsed >= reconnectDelay) {
            reconnectTimeElapsed = 0; // Reset timer
            reconnect();
        }
    } else {
        client.loop();
    }
}
} // namespace mqtt
