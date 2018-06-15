#include "button.hpp"
#include "motor.hpp"
#include "mqtt.hpp"
#include "ota.hpp"
#include "pir.hpp"
#include "wifi.hpp"

void setup() {
    pinMode(1, OUTPUT); // GPIO 1 is used as TX
    Serial.begin(115200);
    Serial.println("\nBooting");

    wifi::setup();
    ota::setup();
    mqtt::setup();
    pir::setup();
    button::setup();
    motor::setup();

    Serial.println("Ready");
}

void loop() {
    ota::handle();
    mqtt::handle();
    pir::handle();
    button::handle();
    motor::handle();
}
