# arduino-m2560-enc28j60-mqtt-danzup-v1
enc28j60 mqtt  danzup arduino atmega2560 , BMP180 , tft lcd ili ILI9486 
version 1.0
Arduino with ethernet module and lcd  that connect to raspberry pi that run node-red with my home automation .
It fetch temperature that is setup in my raspberry pi thermostat  connecting on mqtt topic (my Nest look alike thermostat in node red ), 
fetch outside temperature also from mqtt topic of outside sensor based on esp8266 and DHT22 and
print on lcd along with arduino local BMP180 themperature , pressure 
and have 2 switch button that can set temperature in room  with +  and - .
