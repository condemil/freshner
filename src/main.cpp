#include "button.hpp"
#include "motor.hpp"
#include "ota.hpp"
#include "pir.hpp"
#include "wifi.hpp"

void setup() {
    pinMode(1, OUTPUT); // GPIO 1 is used as TX
    Serial.begin(115200);
    Serial.println("\nBooting");

    wifi::setup();
    ota::setup();
    pir::setup();
    button::setup();
    motor::setup();

    Serial.println("Ready");
}

void loop() {
    ota::handle();
    pir::handle();
    button::handle();
    motor::handle();
}
