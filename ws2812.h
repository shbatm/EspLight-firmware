#ifndef WS2812_H
#define WS2812_H
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include "stripcontrol.h"
#include "fadeColor.h"
#include "rainbow.h"

extern void sendUdpDebugInfo(String);

void setupWS2812(uint16_t, uint8_t);
void setWS2812Strip(int, int, int);
void fadeWS2812(int, int);
void rainbowWS2812(int, int);
void flickerWS2812(int, int, float);
void cylonWS2812(int, int, float);
void tailLoopWS2812(int, int, uint16_t, int, float[5]);
void updateWS2812();

#endif