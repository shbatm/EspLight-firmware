#include "ws2812.h"

// holds the length of the ledstrip in pixels or chips.
static int ws2812_striplen;
// create a pointer of NeoPixelBus object type.
NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> *strip = NULL;

// Variable to tell the update loop if we're using any NeoPixelBus animations
int UsingAnimations = 0;

// timing vairiables.
static unsigned long ccurrent = 0;
static unsigned long cprevious = 0;
static int cinterval = 10;

// variable that holds colors
static uint8_t *colors;

/* setup the neopixel object, and clear the ledstrip. */
void setupWS2812(uint16_t length, uint8_t pin)
{
    ws2812_striplen = length;
    colors = colorinc();
    if(strip == NULL)
    {
        strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(length, pin);
    }
    else
    {
        delete strip;
        strip = new NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod>(length, pin);
    }
    strip->Begin();
    setWS2812Strip(0, 0, 0);
    strip->Show();
}

/* set the ledstrip to a certain (r, g, b) value. */
void setWS2812Strip(int r, int g, int b)
{
    for(int i = 0; i < ws2812_striplen;i++)
    {
        RgbColor color = RgbColor(r, g, b);
        strip->SetPixelColor(i, color);
    }
    UsingAnimations = 0;
}

/* fade the whole ledstrip from one color to another. */
void fadeWS2812(int speed, int brightness)
{
    ccurrent = millis();
    if((ccurrent - cprevious) >= cinterval)
    {
        cprevious = ccurrent;
        colorinc();
    }

    cinterval = speed+1;
    float brightnessFactor = (float)(((float)brightness)/100);
    int r = colors[RED] * brightnessFactor;
    int g = colors[GREEN] * brightnessFactor;
    int b = colors[BLUE] * brightnessFactor;

    setWS2812Strip(r, g, b);
    UsingAnimations = 0;
}

/* 
    fade all the pixels individually from one color to the next.
    creating a rainbow like effect.
*/
void rainbowWS2812(int speed, int brightness)
{
    cinterval = speed + 1;
    ccurrent = millis();
    if((ccurrent - cprevious) >= cinterval)
    {
        cprevious = ccurrent;
        static int range = 0xff*3;
        uint8_t buffer[3][ws2812_striplen];
        int i, s;
        colors = colorinc();
        for(i = 0; i < ws2812_striplen; i++)
        {
            for(s = 0; s < range/ws2812_striplen; s++)
                colors = colorinc();
            float brightnessFactor = (float)(((float)brightness) / 100);
            int r = colors[RED] * brightnessFactor;
            int g = colors[GREEN] * brightnessFactor;
            int b = colors[BLUE] * brightnessFactor;
            RgbColor rgbcolor = RgbColor(r, g, b);
            strip->SetPixelColor(i, rgbcolor);
        }
        UsingAnimations = 0;
    }
}

// ---- ADVANCED NEOPIXELBUS ANIMATIONS SECTION ----
// Common variables and functions for all animations:
NeoPixelAnimator *animations = NULL;

NeoGamma<NeoGammaTableMethod> colorGamma; // for any fade animations, best to correct gamma

void setupAnimations(int numAnimations)
{
    if(animations == NULL)
    {
        animations = new NeoPixelAnimator(numAnimations);
    }
    else
    {
        delete animations;
        animations = new NeoPixelAnimator(numAnimations);
    }
}

RgbColor WheelColor(uint16_t wheelValue) {
   // divide the wheelValue by 360.0f to get a value between 0.0 and 1.0 needed for HslColor
   return HslColor(wheelValue / 360.0f, 1.0f, 0.5f); // this will autoconvert back to RgbColor
}

void SetRandomSeed()
{
    uint32_t seed;

    // random works best with a seed that can use 31 bits
    // analogRead on a unconnected pin tends toward less than four bits
    seed = analogRead(0);
    delay(1);

    for (int shifts = 3; shifts < 31; shifts += 3)
    {
        seed ^= analogRead(0) << shifts;
        delay(1);
    }

    // Serial.println(seed);
    randomSeed(seed);
}

AnimEaseFunction moveEase = NeoEase::QuarticInOut;

// CYLON EYE FEATURE:
RgbColor CylonEyeColor(HtmlColor(0x7f0000));  // can be changed by passing VarTwo as a Wheel value between 0 and 360.
uint16_t lastPixel = 0; // track the eye position
int8_t moveDir = 1; // track the direction of movement

void FadeAll(uint8_t darkenBy)
{
    RgbColor color;
    for (uint16_t indexPixel = 0; indexPixel < strip->PixelCount(); indexPixel++)
    {
        color = strip->GetPixelColor(indexPixel);
        color.Darken(darkenBy);
        strip->SetPixelColor(indexPixel, color);
    }
}

void FadeAnimUpdate(const AnimationParam& param)
{
    if (param.state == AnimationState_Completed)
    {
        FadeAll(10);
        animations->RestartAnimation(param.index);
    }
}

void MoveAnimUpdate(const AnimationParam& param)
{
    // apply the movement animation curve
    float progress = moveEase(param.progress);

    // use the curved progress to calculate the pixel to effect
    uint16_t nextPixel;
    if (moveDir > 0)
    {
        nextPixel = progress * ws2812_striplen;
    }
    else
    {
        nextPixel = (1.0f - progress) * ws2812_striplen;
    }

    // if progress moves fast enough, we may move more than
    // one pixel, so we update all between the calculated and
    // the last
    if (lastPixel != nextPixel)
    {
        for (uint16_t i = lastPixel + moveDir; i != nextPixel; i += moveDir)
        {
            strip->SetPixelColor(i, CylonEyeColor);
        }
    }
    strip->SetPixelColor(nextPixel, CylonEyeColor);

    lastPixel = nextPixel;

    if (param.state == AnimationState_Completed)
    {
        // reverse direction of movement
        moveDir *= -1;

        // done, time to restart this position tracking animation/timer
        animations->RestartAnimation(param.index);
    }
}

void cylonWS2812(int speed, int CylonEyeWheelValue)
{
    Serial.println("Setting up Cylon Eye");

    setupAnimations(2);

    if (CylonEyeWheelValue) 
    {
        CylonEyeColor = WheelColor(CylonEyeWheelValue);
    }

    // fade all pixels providing a tail that is longer the faster
    // the pixel moves.
    animations->StartAnimation(0, 5, FadeAnimUpdate);

    // speed variable is the time (in ms) it takes to move from one 
    // end to the other; for large strips, this should be a large value 
    // (several seconds).
    animations->StartAnimation(1, speed, MoveAnimUpdate);

    // Tell the loop function (updateWS2812) we're using animations now:
    UsingAnimations = 1;
}

// Rotating Loop with Tail
const float MaxLightness = 0.4f; // max lightness at the head of the tail (0.5f is full bright)

void LoopAnimUpdate(const AnimationParam& param)
{
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations->RestartAnimation(param.index);

        // rotate the complete strip one pixel to the right on every update
        strip->RotateRight(1);
    }
}

void DrawTailPixels(int TailLength, float TailColor, int StartPixel)
{
    // using Hsl as it makes it easy to pick from similiar saturated colors
    float hue;
    if (TailColor < 0)
    {
        hue = random(360) / 360.0f;
    }
    else
    {
        hue = TailColor;
    } 
    
    for (uint16_t index = (0 + StartPixel); index < strip->PixelCount() && index <= (StartPixel + TailLength); index++)
    {
        float lightness = index * MaxLightness / TailLength;
        RgbColor color = HslColor(hue, 1.0f, lightness);

        int pixelToUpdate = (index + StartPixel) % strip->PixelCount();
        strip->SetPixelColor(index, colorGamma.Correct(color));
    }
}

void tailLoopWS2812(int speed, uint16_t TailLength, uint8_t fill, float tailColors[5])
{
    Serial.println("Setting up Tail Loop");
    setupAnimations(1);
    //TODO: Handle brightness
    //TODO: Check tails go to zero


    // Error checking for valid tail length
    if (TailLength >= strip->PixelCount()) 
    {
        TailLength = strip->PixelCount() - 1;
    }
    else if (TailLength == 0) 
    {
        TailLength = 1;
    }

    SetRandomSeed();

    if (fill) 
    {
        int numColors;
        for (uint8_t z = 0; z < 5; z++)
        {
            if (tailColors[z] != -2.0f) 
            {
                numColors++;
            }
            else
            {
                break;
            }
        }

        int numTails = strip->PixelCount() / (numColors * TailLength) + 1;

        for (uint8_t n = 0; n < numTails; n++)
        {
            for (uint8_t col = 0; col < 5; col++)
            {
                DrawTailPixels(TailLength, tailColors[col], (n*TailLength*numColors) + (col*TailLength));
            }
        }
    }
    else
    {
        for (uint8_t col = 0; col < 5; col++)
        {
            DrawTailPixels(TailLength, tailColors[col], col * TailLength);
        }
    }

    // we use the index 0 animation to time how often we rotate all the pixels
    animations->StartAnimation(0, speed, LoopAnimUpdate); 

    // Tell the loop function (updateWS2812) we're using animations now:
    UsingAnimations = 1;
}


/* update the while ledstrip. */
void updateWS2812()
{
    if (UsingAnimations)
    {
        animations->UpdateAnimations();
    }

    strip->Show();
}