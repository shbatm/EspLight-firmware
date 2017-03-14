
#API
This page details the communication protocol for sending commands to the ESPLink.

##Control
*TODO: Implement soft-reset and AP-mode button pushes for remote installations*

##Effects & Parameters
The table below lists the parameters that can be sent to the ESPLink using the App, ISY Network Module, or any other method of sending a UDP packet.

The format of the string should be: 
`?pin=...&effect=...[&brightness=...][&var0=...][&var(1 thru 7)=...][&savesetting=1]`

###Pin
`pin=`:	4-digit pin assigned at ESPLight setup

###Effect
|Effect Name  |Effect Number|Description              |Analog?|WS2801?|WS2812?|
|:-----------:|:-----------:|-------------------------|:-----:|:-----:|:-----:|
|             |`&effect=`   |                         |       |       |       |
|RGBCOLORS    |0|All LEDs single color|Y|Y|Y|
|FADING       |1|All LEDs fade together through rainbow|Y|Y|Y|
|DIGITALFADING|2|Each LED fades through all colors in rainbow|N|Y|Y|
|CYLONEYE     |3|Single color bounces back and forth through strip, with a tail behind it.|N|N(1)|Y|
|TAILLOOP     |4|Multiple colors chase through strip|N|N(1)|Y|
|FLICKER      |5|Candle/Fireplace Effect|N|N(1)|Y|

*(1) Possible, but not yet developed*

###Brightness
`brightness=`: Brightness Percentage (0-99%)--any integer value between 0 and 99.

###VarZero, VarOne, VarTwo
**For `RGBCOLORS` Effect (`effect=0`):** 

* Send `?pin=0000&brightness=99&var0=RED&var1=GREEN&var2=BLUE`
* Where RED, GREEN, BLUE are the RGB Color Wheel Values (0-255) for the color to use.

**For All Other Effects:**

* VarZero (`var0=`) controls the speed of the effect--any integer value between 0-32767.
* VarOne (`var1=`) and VarTwo (`var2=`) have various uses depending on the effect.
* See table below for more details

|Effect Name  |VarZero<br />`&var0=`|VarOne<br />`&var1=`|VarTwo<br />`&var2=`|
|:-----------:|:--------------:|:-------------:|:-------------:|
|FADING       |Speed (1)       |Not Used       |Not Used       |
|DIGITALFADING|Speed (1)       |Not Used       |Not Used       |
|CYLONEYE     |Speed (2)       |Length<br />0-Full Strip<br />X-# of pixels wide|Centered Around Pixel<br />-1 - Center of Strip<br />X-Center on Pixel #X<br />(ignored if VarOne = 0)|
|TAILLOOP     |Speed (3)       |Tail Length<br />(# of Pixels)|Direction & Fill:<br />1 - Right, No Fill<br />2 - Right, Fill Strip<br />-1 - Left, No Fill<br />-2 - Left, Fill Strip<br />>=3 - Fill, Alternate direction after X pixels|
|FLICKER      |Speed (1)       |Not Used       |Not Used       |

|Speed|Description|
|:---:|-----------|
|1    |Time in `ms` between changes; smaller=faster|
|2    |Time in `ms` it takes to cross the whole strip, smaller=faster|
|3    |Time in `ms` to move (1) pixel to the right, smaller=faster|

###VarWheel[0-4]

* HSL Color Wheel **HUE** Values
* When passing Hue values, see the mapping below:

|VarWheel[0]|VarWheel[1]|VarWheel[2]|VarWheel[3]|VarWheel[4]|
|-----------|-----------|-----------|-----------|-----------|
|`[var3=]`  |`[var4=]`  |`[var5=]`  |`[var6=]`  |`[var7=]`  |

* The VarWheel[0] variable is only used for the `CYLONEYE`,`TAILLOOP`, & `FLICKER` effects.
    * `var3=-2` is the default option and is used if the parameter is not passed
        - `CYLONEYE` default is RED
        - `TAILLOOP` default is RANDOM
        - `FLICKER` default is YELLOW/ORANGE (Flame-colored)
    * A HUE value can be passed as a `float` type: 0.0-360.0
    * For a random color each time the effect is activated: send `var3=-1`
    * Since this parameter only passes the **H**(ue) of HSL, **S**aturation & **L**uminence are fixed in the software. To use white, you must send: `var3=-3`.
* VarWheel[1 thru 4]--`var4=`,`var5=`,`var6=`,`var7=`--are only used to select additional colors for `TAILLOOP` effect.  Up to 4 additional colors may be used and sent using the same rules as VarWheel[0]. The only exception is the default (-2) will be black/off instead of random.
* The example below would send a "christmas light" effect of 5 colors chasing each other:<br />`?pin=1234&effect=4&var0=250&var1=2&var2=2&var3=0&var4=100&var5=240 &var6=-3&var7=25`

###Save Settings

* `savesetting=1`: In order to save a particular configuration to be used every time power is restored to the ESPLight, send `&savesetting=1` at the end of the parameter string.  This flag will tell the software to write the effect to the EEPROM and use it after power restore / reset button push.
* To reset the default back to off/black, send:<br />`?pin=1234&effect=0&brightness=0&var0=0&var1=0&var2=0&savesetting=1`

##Example Strings

* Fire Effect: `?pin=1234&effect=5&brightness=50&var0=750&var3=-2`
* Christmas Lights: `?pin=1234&effect=4&var0=250&var1=2&var2=2&var3=0&var4=100&var5=240 &var6=-3&var7=25`
* Black: `?pin=1234&effect=0&brightness=0&var0=0&var1=0&var2=0`

##Using the API

* To send a message in a Linux terminal, run:<br />`printf "?pin=0000&effect=2&brightness=99&var0=200&var1=0&var2=0" | nc -u -q1 esplight-ip 1337`
* To receive messages from the APP in a Linux terminal, run: `nc -u -l -p 1337`
* To send a message from the ISY Network Module: Add a new Network Resource
    + Type: UDP
    + Host: IP/Hostname of the ESPLight (recommend setting a DCHP Reservation or static IP)
    + Port: 1,337
    + Timeout: 500ms
    + Mode: Raw Text
    + Body: *your command string* (e.g. `?pin=1234&effect=2&brightness=99&var0=200`)

##Adding Effects

For Developers, to add a new effect:

1. Add the name in `stripcontrol.h`
2. Add an `else if` statement to `stripcontrol.cpp` to call the approriate functions
3. Add the appropriate functions to the appropriate strip files (`analog.cpp`, `WS2801.cpp`, and/or `WS2812.cpp`)
4. Add any new function calls to the appropriate header files
