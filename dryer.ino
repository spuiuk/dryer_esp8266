#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>

#include "globals.h"

void setup() {
    Serial.begin(9600);
    delay(100);
    Serial.println("Starting!");

    WiFiManager wm;
    if (!wm.autoConnect()) {
        dbg_print("Timed out waiting for Wifi. Putting to sleep");
        ESP.reset();
        delay(1000);
    }

    dbg_print("Connected to Wifi");
}

void loop() {
    delay(1000);
}