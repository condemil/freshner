#pragma once

#include <ESP8266WiFi.h>
#include <PubSubClient.h>

#include "config.hpp"
#include "elapsedMillis.hpp"
#include "logger.hpp"
#include "motor.hpp"
#include "wifi.hpp"

namespace mqtt {
    PubSubClient client(wifi::espClient);
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

    void motorCallback(bool isActive) {
        logger::debugf("mqtt: received motor callback: %d\n", isActive);

        if (isActive) {
            publish(MQTT_TOPIC_STATE, "ON", true);
        } else {
            publish(MQTT_TOPIC_STATE, "OFF", true);
        }
    }

    void reconnect() {
        // Loop until we're reconnected
        logger::debugln(F("mqtt: attempting connection..."));
        String clientId = "ESP8266Client-";
        clientId += String(ESP.getChipId(), HEX);

        if (client.connect(clientId.c_str(), MQTT_LOGIN, MQTT_PASSWORD)) {
            logger::debugln(F("mqtt: connected"));
            client.subscribe(MQTT_TOPIC_COMMAND);
        } else {
            logger::debugf("mqtt: connect failed, rc=%d try again in %u seconds\n", client.state(), reconnectDelay / 1000);
        }
    }

    void setup() {
        client.setServer(MQTT_HOST, MQTT_PORT);
        client.setCallback(callback);
        motor::setCallback(motorCallback);
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
}
