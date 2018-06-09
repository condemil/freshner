#pragma once

#include <ESP8266WiFi.h>

#include "config.h"

void wifiSetup() {
    WiFi.mode(WIFI_STA);
    WiFi.begin(WIFI_SSID, WIFI_PASSWORD);
    while (WiFi.waitForConnectResult() != WL_CONNECTED) {
        Serial.println("Connection Failed! Rebooting...");
        delay(5000);
        ESP.restart();
    }
    Serial.printf("IP address: %s\n", WiFi.localIP().toString().c_str());
}
