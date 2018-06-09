#pragma once

#include <ArduinoOTA.h>

#include "config.hpp"

namespace ota {
    void setup() {
        // ArduinoOTA.setPort(8266); // Port defaults to 8266
        // ArduinoOTA.setHostname("myesp8266"); // Hostname defaults to esp8266-[ChipID]

        Serial.printf("Hostname: esp8266-%06x.local\n", ESP.getChipId());
        ArduinoOTA.setPassword(OTA_PASSWORD);

        ArduinoOTA.onStart([]() {
            String type;
            if (ArduinoOTA.getCommand() == U_FLASH)
            type = "sketch";
            else // U_SPIFFS
            type = "filesystem";

            // NOTE: if updating SPIFFS this would be the place to unmount SPIFFS using SPIFFS.end()
            Serial.println("Start updating " + type);
        });

        ArduinoOTA.onEnd([]() {
            Serial.println("\nEnd");
        });

        ArduinoOTA.onProgress([](unsigned int progress, unsigned int total) {
            Serial.printf("Progress: %u%%\r", (progress / (total / 100)));
        });

        ArduinoOTA.onError([](ota_error_t error) {
            Serial.printf("Error[%u]: ", error);
            if (error == OTA_AUTH_ERROR) Serial.println("Auth Failed");
            else if (error == OTA_BEGIN_ERROR) Serial.println("Begin Failed");
            else if (error == OTA_CONNECT_ERROR) Serial.println("Connect Failed");
            else if (error == OTA_RECEIVE_ERROR) Serial.println("Receive Failed");
            else if (error == OTA_END_ERROR) Serial.println("End Failed");
        });

        ArduinoOTA.begin();
    }

    void handle() {
        ArduinoOTA.handle();
    }
}
