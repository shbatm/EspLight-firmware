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

RgbColor WheelColor(float wheelValue) {
   // divide the wheelValue by 360.0f to get a value between 0.0 and 1.0 needed for HslColor
   return HslColor(wheelValue / 360.0f, 1.0f, 0.5f); // this will autoconvert back to RgbColor
}

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
    SetRandomSeed();
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

// Flickering (Fire/Candle) Animation
void flickerWS2812(int speed, int brightness, float startingColor)
{
    ccurrent = millis();
    if((ccurrent - cprevious) >= cinterval)
    {
        cprevious = ccurrent;
        cinterval = random(100, speed + 101);
        RgbColor baseColor(HtmlColor(0xf7ca42));
        if (startingColor == -1.0f) 
        {
            baseColor = WheelColor((float)random(360));
        }
        else if (startingColor == -3.0f) {
            baseColor = HtmlColor(0xffffff);
        }
        else if (startingColor >= 0.0f)
        {
            baseColor = WheelColor(startingColor);
        }

        brightness = (brightness < 0) ? 0 : brightness;
        brightness = (brightness > 100) ? 100 : brightness;
        float brightnessFactor = (float)(((float)brightness)/100);
        baseColor.Darken(255 - (255 * brightnessFactor));


        int i;
        for(i = 0; i < ws2812_striplen; i++)
        {
            int flicker = random(0,150);
            RgbColor pixelColor = baseColor;
            pixelColor.Darken(255 - (255 * flicker));
            strip->SetPixelColor(i, pixelColor);
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

AnimEaseFunction moveEase = NeoEase::QuarticInOut;

// CYLON EYE FEATURE:
RgbColor CylonEyeColor(HtmlColor(0x7f0000));  // can be changed by passing varTwo as a Wheel value between 0 and 360.
uint16_t lastPixel = 0; // track the eye position
uint16_t nextPixel;
uint16_t offsetPixels = 0;
int8_t moveDir = 1; // track the direction of movement
uint16_t eyerange = ws2812_striplen;

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
    if (moveDir > 0)
    {
        nextPixel = (progress * eyerange) + offsetPixels;
    }
    else
    {
        nextPixel = ((1.0f - progress) * eyerange) + offsetPixels;
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

void cylonWS2812(int speed, int brightness, float CylonEyeWheelValue)
{
    setWS2812Strip(0, 0, 0);
    setupAnimations(2);

    if (CylonEyeWheelValue == -1.0f) 
    {
        SetRandomSeed();
        CylonEyeColor = WheelColor((float)random(360));
    }
    else if (CylonEyeWheelValue == -3.0f) {
        CylonEyeColor = HtmlColor(0xffffff);
    }
    else if (CylonEyeWheelValue >= 0.0f)
    {
        CylonEyeColor = WheelColor(CylonEyeWheelValue);
    }

    brightness = (brightness < 0) ? 0 : brightness;
    brightness = (brightness > 100) ? 100 : brightness;
    float brightnessFactor = (float)(((float)brightness)/100);
    CylonEyeColor.Darken(255 - (255 * brightnessFactor));

    // Handle % range of strip to use; 0 = Full Strip
    if (stripcontrol.varOne > 100) {
        stripcontrol.varOne = 0;
    } else if (stripcontrol.varOne < 0) {
        stripcontrol.varOne = 0;
    }
    if (stripcontrol.varOne == 0) {
        eyerange = ws2812_striplen;
    } 
    else {
        eyerange = round((float)ws2812_striplen * ((float)stripcontrol.varOne/100.0f));
        
        // Set center of eye based on percentage of strip (varTwo)
        if (stripcontrol.varTwo == -1) {
            stripcontrol.varTwo = 50;
        } else if (stripcontrol.varTwo < -1 ) {
            stripcontrol.varTwo = 0;
        } else if (stripcontrol.varTwo > 100) {
            stripcontrol.varTwo = 100;
        }
        float midPoint = (float)ws2812_striplen * ((float)stripcontrol.varTwo/100.0f);
        float leftEnd = midPoint - ((float)eyerange/2.0f);
        if (leftEnd <= 1.0f) 
        { 
            offsetPixels = 0; 
        } else { 
            offsetPixels = round(leftEnd) - 1; 
        }

        // DEBUG INFO
        char debug1[150];
        sprintf(debug1, "\neyerange: %d\nws2812_striplen: %d\nstripcontrol.varTwo: %d\nmidPoint: %d\nleftEnd"
            ": %d\nlastPixel: %d\n", eyerange, ws2812_striplen, stripcontrol.varTwo, (int)(midPoint*100), (int)(leftEnd*100), offsetPixels);
        sendUdpDebugInfo(debug1);
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
float MaxLightness = 0.5f; // max lightness at the head of the tail (0.5f is full bright)
int RotateSetDirection = 1;
int RotateCurrDirection = 1;
int RotateCounter = 1;

void LoopAnimUpdate(const AnimationParam& param)
{
    // wait for this animation to complete,
    // we are using it as a timer of sorts
    if (param.state == AnimationState_Completed)
    {
        // done, time to restart this position tracking animation/timer
        animations->RestartAnimation(param.index);

        if (RotateSetDirection >= 0 && RotateSetDirection < 3)
        {
            // rotate the complete strip one pixel to the right on every update
            strip->RotateRight(1);
        }
        else if (RotateSetDirection < 0)
        {
            // rotate the complete strip one pixel to the left on every update
            strip->RotateLeft(1);
        }
        else if (RotateSetDirection >= 3) 
        {
            if (RotateCounter > 0 && RotateCounter < RotateSetDirection)
            {
                if (RotateCurrDirection == 1) 
                {
                    strip->RotateRight(1);
                    RotateCounter++;
                }
                else
                {
                    strip->RotateLeft(1);
                    RotateCounter--;
                }
            }
            else
            {
                RotateCurrDirection = RotateCurrDirection * -1;
                RotateCounter += RotateCurrDirection;
            }
        }

    }
}

void DrawTailPixels(int TailLength, float TailColor, int StartPixel)
{
    // using Hsl as it makes it easy to pick from similiar saturated colors
    float hue;
    if (TailColor < 0.0f && TailColor > -3.0f)
    {
        hue = random(360) / 360.0f;
    }
    else if (TailColor == -3.0f)
    {
        hue = TailColor;  // White
    }
    else
    {
        hue = TailColor / 360.0f;
    } 
    
    // Serial.printf("Making a tail of %d, %d long, starting at %d.\n", (int)(hue * 360.0f), TailLength, StartPixel);

    // Handle Colors
    RgbColor color;
    for (uint16_t index = 0; (index + StartPixel) < strip->PixelCount() && index <= TailLength; index++)
    {
        float lightness = index * MaxLightness / TailLength;
        if (hue >= 0.0f) 
        {
            color = HslColor(hue, 1.0f, lightness); // Regular Colors
        }
        else
        {
            color = HslColor(0.0f, 0.0f, lightness); // White
        }
        int pixelToUpdate = (index + StartPixel) % strip->PixelCount();
        strip->SetPixelColor(pixelToUpdate, color);
        //strip->SetPixelColor(index, colorGamma.Correct(color));
    }
}

void tailLoopWS2812(int speed, int brightness, uint16_t TailLength, int FillAndDir, float tailColors[5])
{
    setWS2812Strip(0, 0, 0);
    setupAnimations(1);

    brightness = (brightness < 0) ? 0 : brightness;
    brightness = (brightness > 100) ? 100 : brightness;
    MaxLightness = (float)(((float)brightness)/100.0f * 0.5f);  // 0.5f is full brightness in HSL scale

    // TODO: Check tails go to zero

    // Error checking for valid tail length
    if (TailLength >= strip->PixelCount()) 
    {
        TailLength = strip->PixelCount() - 1;
    }
    else if (TailLength <= 2) 
    {
        TailLength = 2; // Minimum length is 2, 1 pixel on, 1 pixel off.
    }

    //Serial.printf("Using tail length of %d.\n", TailLength);

    SetRandomSeed();

    // Find the number of valid colors we were given in tailColors
    int numColors = 0;
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
    if (!numColors) { numColors = 1; } // Error checking, make sure we have at least 1 color

    if (FillAndDir > 1 || FillAndDir < -1) 
    {
        int numTails = strip->PixelCount() / (numColors * TailLength);
        numTails = (numTails == 0) ? 1 : numTails;

        // Serial.printf("Creating %d tail(s) for %d color(s) on a strip %d long.\n", numTails, numColors, strip->PixelCount());

        for (uint8_t n = 0; n < numTails; n++)
        {
            for (uint8_t col = 0; col < numColors; col++)
            {
                DrawTailPixels(TailLength, tailColors[col], (n*TailLength*numColors) + (col*TailLength));
            }
        }
    }
    else
    {
        for (uint8_t col = 0; col < numColors; col++)
        {
            DrawTailPixels(TailLength, tailColors[col], col * TailLength);
        }
    }

    RotateSetDirection = FillAndDir;

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