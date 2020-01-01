#include <FS.h>
#include <ESP8266WiFi.h>
#include <ESP8266WebServer.h>
#include <DNSServer.h>
#include <WiFiManager.h>
#include <ArduinoJson.h>
#include <string.h>
#include <WiFiClient.h>

#include "globals.h"

bool shouldSaveConfig = false;

const char *pbHost = "api.pushingbox.com";
const int  pbPort = 80;

/* add default values for your parameters here */
/* char token[34] = "YourSecurityToken"; */
char pushingBoxDeviceId[20] = "";

bool pbSendNotification(char *str)
{
    WiFiClient client;

    dbg_print("Send Notification: Attempting connect");
    if(!client.connect(pbHost, pbPort)) {
        Serial.println("Could not connect to Pushing Box server");
        return false;
    }

    if (strlen(pushingBoxDeviceId) == 0) {
        Serial.println("Pushing box device Id not set");
        return false;
    }

    if (client.connected()) {
        dbg_print("Connected");
    } else {
        dbg_print("Not connected");
    }

    String postStr = "devid=" + String(pushingBoxDeviceId) + "&";
    postStr += "action=" + String(str);

    client.print("POST /pushingbox HTTP/1.1\n");
    client.print("Host: api.pushingbox.com\n");
    client.print("Connection: close\n");
    client.print("Content-Type: application/x-www-form-urlencoded\n");
    client.print("Content-Length: ");
    client.print(postStr.length());
    client.print("\n\n");
    client.print(postStr);
    client.flush();
    client.stop();

    dbg_print("Sent message");
}

bool readConfigFile(void)
{
    dbg_print("Called readConfigFile")

    File configFile = SPIFFS.open("/config.json", "r");
    if (!configFile) {
        Serial.println("Failed to read config");
        return false;
    }

    size_t size = configFile.size();
    std::unique_ptr<char[]> buf(new char[size]);
    configFile.readBytes(buf.get(), size);

    StaticJsonDocument<512> doc;
    auto error = deserializeJson(doc, buf.get());
    if (error) {
        Serial.println("Could not parse config file");
        configFile.close();
        return false;
    }

    /* Copy your parameter value from json */
    /* strcpy(token, doc["token"]); */
    strcpy(pushingBoxDeviceId, doc["pushingBoxDeviceId"]);

    configFile.close();
    return true;
}

bool saveConfigFile(void)
{
    StaticJsonDocument<512> doc;

    dbg_print("Called saveConfigFile")

    /* Copy over parameters to json object */
    /* doc[token] = token; */
    doc["pushingBoxDeviceId"] = pushingBoxDeviceId;
    Serial.println("Pushing box id" + String(pushingBoxDeviceId));

    File configFile = SPIFFS.open("/config.json", "w");
    if(!configFile) {
        Serial.println("Failed to open configfile for writing");
        return false;
    }

    serializeJson(doc, configFile);
    configFile.close();

    return true;
}

void saveConfigCallback()
{
    dbg_print("Save Config Callback");
    shouldSaveConfig = true;
}

void setup() {
    WiFiManager wm;

    Serial.begin(115200);
    delay(1000);
    Serial.println("Starting!");

    Serial.println("Waiting 3 seconds");
    Serial.println("Press and hold flash button(GPIO 0) to reset settings");
    delay(3000);
    pinMode(0, INPUT);
    if (digitalRead(0) == LOW) {
        Serial.println("Resetting Settings");
        SPIFFS.format();
        wm.resetSettings();
    }

    dbg_print("Attempting to mount fs");
    if (!SPIFFS.begin()) {
        Serial.println("Failed to mount fs.");
        return;
    }

    if (!readConfigFile()) {
        Serial.println("Could not read config file");
    }

    /* Add custom parameters here */
    /* WiFiManagerParameter custom_token("token", "Token", token, 40); */
    WiFiManagerParameter custom_pushingBoxDeviceId("pushingBoxDeviceId",
                                                    "Pushing Box DeviceId",
                                                    pushingBoxDeviceId, 20);

    /* Add customer parameter objects to WiFiManager */
    /* wm.addParameter(&custom_token); */
    wm.addParameter(&custom_pushingBoxDeviceId);

    wm.setSaveConfigCallback(saveConfigCallback);
    if (!wm.autoConnect()) {
        dbg_print("Timed out waiting for Wifi. Putting to sleep");
        ESP.reset();
        delay(1000);
        return;
    }

    if (shouldSaveConfig) {
        /* Copy over updated parameters */
        /* strcpy(token, custom_token.getValue()); */

        strcpy(pushingBoxDeviceId, custom_pushingBoxDeviceId.getValue());

        if (!saveConfigFile()) {
            Serial.println("Could not save config file");
            return;
        }
    }

    dbg_print("Connected to Wifi");

    pbSendNotification("Device Initialised");
}

void loop() {
    delay(10000);
}