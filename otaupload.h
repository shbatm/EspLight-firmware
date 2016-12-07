#ifndef OTAUPLOAD_H
#define OTAUPLOAD_H

#include <Arduino.h>
#include <ESP8266WiFi.h>
#include <WiFiUdp.h>
#include <ArduinoOTA.h>

#define OTAPORT         48266

void setupOta();
void handleSketchUpdate();

#endif