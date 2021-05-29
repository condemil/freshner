#pragma once

#include <ESP8266WiFi.h>

#include "config.hpp"
#include "logger.hpp"

namespace wifi {
    WiFiClient espClient;

    void setup() {
        WiFi.mode(WIFI_STA);
        WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
        while (WiFi.waitForConnectResult() != WL_CONNECTED) {
            logger::debugln(F("wifi: connection Failed! Rebooting..."));
            delay(5000);
            ESP.restart();
        }
    }
}
