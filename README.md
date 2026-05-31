# Door-alert
Motion detector device for detecting suspicious movement at the door and send alert through discord webhook

## Item list 
- ESP32 Wroom32
- TM1637 7-Segment Display
- Ultrasonic Sensors
- WiFi Connection
  
## How to use
1. Connect TM1637 to ESP32 <br>
- ```GND -> GND```
- ```VCC -> 3v3```
- ```CLK -> D19```
- ```DIO -> D18```

2. Connect Ultrasonic Sensor to ESP32 <br>
- ```VCC -> 3v3```
- ```TRIG -> D14```
- ```ECHO -> D12```
- ```GND -> GND```

3. Edit WiFi and Discord Configuration
- Edit ssid

