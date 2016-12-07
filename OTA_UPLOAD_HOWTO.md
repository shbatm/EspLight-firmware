# OTA Upload Details -

Build in Sublime/Deviot using Alt+D,Alt+B

Run the following in command prompt:
python.exe C:\Users\<user>\.platformio\packages\framework-arduinoespressif8266\tools\espota.py -i ESP_IP_ADDRESS -p 8266 -f "<path_to_build_directory>\.pioenvs\esp12e\firmware.bin"


For example:
python.exe C:\users\Tim\.platformio\packages\framework-arduinoespressif8266\tools\espota.py -i 192.168.1.34 -p 8266 -f "C:\Users\Tim\Documents\projects\EspLight-firmware\bin\EspLight-firmware\.pioenvs\esp12e\firmware.bin"