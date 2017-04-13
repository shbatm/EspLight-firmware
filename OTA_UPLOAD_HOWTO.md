# OTA Upload Details

## Building from Source
The code will not compile using the Arduino IDE.  To compile the code, use Sublime Text Editor with Deviot and PlatformIO plugins. See bottom of [README](README.md#building) for details.

## Upload to Device
If you build in Sublime/Deviot (using Alt+D,Alt+B); run the following in a command prompt:
`python.exe C:\Users\<user>\.platformio\packages\framework-arduinoespressif8266\tools\espota.py -i ESP_IP_ADDRESS -p 8266 -f "<path_to_build_directory>\.pioenvs\esp12e\firmware.bin"`

If you want to upload the binary from this repo to an existing ESPLightX, run the following in a command prompt from the `bin` folder of the repo:
`python.exe espota.py -i ESP_IP_ADDRESS -p 8266 -f "firmware.bin"`
*Note: the OTA upload will not work to upgrade the original ESPLight firmware to the ESPLightX firmware*