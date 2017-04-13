#ifndef EFFECTPARSE_H
#define EFFECTPARSE_H

/*
    this code handels the effect parsing and applying part.
    it also listens for a locate packets. and sends a response.
*/

#include <Arduino.h>
#include <WiFiUdp.h>
#include "stripcontrol.h"

extern String board_name;
extern int accessPin;
extern bool remoteModeSwitch;

extern void debugPrintStripControl();
extern void printWifiStatus();

void printPacketInfo(int);

void setupEffectParse(int);
String getAlphaNumString(String &);
String effectArg(const char *);
void applyEffectData();
void parseEffectPacket(String);
String readPacketContents(WiFiUDP);
void handleEffectUpdate();
void findResponse(WiFiUDP);
void sendUdpDebugInfo(String);

#endif