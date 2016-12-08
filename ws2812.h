#ifndef WS2812_H
#define WS2812_H
#include <NeoPixelBus.h>
#include <NeoPixelAnimator.h>
#include "stripcontrol.h"
#include "fadeColor.h"
#include "rainbow.h"

void setupWS2812(uint16_t, uint8_t);
void setWS2812Strip(int, int, int);
void fadeWS2812(int, int);
void rainbowWS2812(int, int);
void cylonWS2812(int, int);
void tailLoopWS2812(int, uint16_t, uint8_t, float[5]);
void updateWS2812();

#endif