#Questions for Duality4Y

__Background:__ Working on a project based on Duality4Y's ESPLight project for some lighting around my house. This document includes some of the issue's I've found.  I'm building this list to at some point show Duality4Y what I'm doing and ask for any information I'm missing.


1. `<otaupload.h>` - What library is this from? Cannot find it. Had to comment out lines referencing otaupload.h, setupOta, and handleSketchUpdate
	* Found in Duality4Y's personal GitHub repo. Now included locally in this repo. 
		
2. NeoPixelBus V2 is failing during build: `"error: invalid use of template-name 'NeoPixelBus' without an argument list"`. 
	* Looking at the documentation of the NeoPixelBus site, you need to declare the method to use. For example: `NeoPixelBus<NeoGrbFeature, Neo800KbpsMethod> strip(16, 2);`
	* For the Esp8266 platforms, just omit the pin argument. This is due to hardware support that doesn't allow for selecting a pin...The NeoEsp8266Dma800KbpsMethod is the underlying method that gets used if you use Neo800KbpsMethod on Esp8266 platforms. There should be no need to use it directly.

	* The NeoEsp8266Dma800KbpsMethod only supports the RDX0/GPIO3 pin. The Pin argument is omitted. See other esp8266 methods below if you don't have this pin available.

	* NOTE: Due to the varied board layouts for Esp8266, even though the pin maybe exposed, it may not be usable due to some custom feature on the board you use. If you find it not working, you should review the hardware schematic of your board and confirm the pin is not used for other purposes that interferes with it being used for NeoPixelBus." The other method that allows the selection of a pin 

	* NeoEsp8266BitBang800KbpsMethod supports any available pin between 0 and 15.
	This method uses only the CPU to send data to the NeoPixels. But due to WiFi interrupts it is not stable when used with WiFi features of the Esp8266.

3. Need to confim if there are any special build commands. Away from my workshop so I don't have any spare boards to test with right now.

4. Why is speed control passed as the RED parameter? i.e. Speed is the Var0 parameter which is the same as the red value as opposed to its own value.
