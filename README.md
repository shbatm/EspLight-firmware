# Fork of EspLight-firmware from Duality4Y
This fork is to make some minor updates and tweaks to Duality4Y's original ESPLight project to better fit my needs.

##Changes include:
* Tweaked PCB to add mounting holes and move the VCC to 3.3VDC on-board power supply to an off-board Buck Converter to save component costs.  WRT the buck converter: since I only plan to build a few of these, it's cheaper for me to buy some $2.50 DC to DC buck converter from [here](http://www.gearbest.com/development-boards/pp_51010.html) than to buy the eqivalent in parts+shipping from Digikey/Mouser.  The new board uses a 3-pin header (VCC/3.3V/GND) to connect to the buck board with jumper wires.
* Update software to use NeoPixelBus V2: NeoPixelBus seems to have updated its method statements in the new version and the old EspLight-firmware won't compile
* Fixed speed control: in old method speed control is based only on the "Red"/"Var0" parameter. New method it is an independent parameter (still need to update the app)
* Added ability to save state for power up using the `savesettings=1` parameter in the API. To reset to black, send a black command with `&savesettings=1` appended to the end.  Instead of just defaulting to off and requiring someone to connect with the app to turn the lights on, this will allow for a more standalone installation where the app is only required to change the mode.

##To-do:
* Update the APP to send speed parameters.
* Add "chaser" mode (aka Knightrider/KITT)
* Upgrade WS2812 functions to the NeoPixelBus animations


##Known Issues:
* Using NeoEsp8266Dma800KbpsMethod only supports the RDX0/GPIO3 pin, NeoEsp8266BitBang800KbpsMethod supports any available pin between 0 and 15 but uses only the CPU to send data to the NeoPixels. Due to WiFi interrupts it is not stable when used with WiFi features of the Esp8266.  Plan to rev up the board to use shared RDX & Data pin, work around for now is to break the trace from GPIO14 and jump from the RX header to the Clock header.

# Original Readme:

## EspLight-firmware
firmware for on a EspLight Board.

## first time use.
the first time you use the EspLight there will probably be no firmware it 

if no firmware follow 'loading precompiled firmware'

first time use with firmware:
* press the AP button to turn the EspLight into a access point.
* connect with the EspLight via the wifi. the first time it'll have a ssid like 
EspLight-xx where xx are numbers.
* surf to the configuration page. (type 192.168.4.1 and hit enter)
* in the configuration page, you'll have two options. Wifi and Led.
* fill in your wifi settings on the Wifi page and hit save.
* then in the LED page set the kind of leds you have and how many. in the case of 
analog leds you don't have to enter a number.

## loading precompiled firmware.
to load the precompiled firmware you'll have to download and install esptool:

https://github.com/themadinventor/esptool

next you'll have to connect your esplight to a usbserial converter.
* how to hook one up that came with the EspLight: https://www.tkkrlab.nl/wiki/EspLight#Program_the_firmware_into_the_EspLight

you can find the binary in the bin folder.

next you run :

```
$ esptool.py write_flash -ff 80m -fm qio -fs 32m-c1 0x00000 firmware.bin
```

after uploading you can follow the instructions for first time use.

## requirements for building:
this library: https://github.com/Makuna/NeoPixelBus

and the latest sources from: https://github.com/esp8266/arduino

Note: Duality4Y mentioned this source is compiled with SublimeText3 and the stino plugin and does not compile in ide because of arduino ide code mungeling.
shbatm's comment: stino seems to be depreciated. Now building with Sublime Text 3 and the [Deviot](https://github.com/gepd/Deviot) plugin (based on [PlatformIO](http://platformio.org/))
