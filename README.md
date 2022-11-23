# Automatic soap dispenser with wifi interface using Arduino MKR1000

The Automatic Soap Dispenser is controlled by an Arduino MKR1000. The Arduino is triggered by a HC-SR04 Ultrasonic sensor that is activated every 5 seconds. When the sensor detects a hand under the soap pump, a SG90 stepper motor is activated and pushes the pump 10 times to dispense soap (10 doses). When it's done, another SG90 motor is activated and used as a countdown to 20 seconds. The Arduino is able to connect to a network (Wi-fi Web Server) and print information like the live reading of the ultrasonic sensor, the last time someone used the soap dispenser, if the soap dispenser is used right now and in case the soap dispenser is used right now how much time is left to reach 20 seconds. The design is interchangeable with most common soap dispensers found in homes and it was made with MDF. 

Front | Back
:----:|:----:
![](https://i.imgur.com/PibYvJb.jpg) | ![](https://i.imgur.com/KThGmUR.jpg)

| Components   | Tools       | Software   | 
| ----------- | ----------- | ----------- |
| Arduino MKR1000 x1        | Hand Saw | Arduino IDE
| SG90 Micro Servo Motor x2 | Sandpaper
| HC-SR04 Ultrasonic Sensor x1 | Wood Glue   
| Jumper Wires (M/M, F/F, M/F) |
| Screws      |
| Fishing Line 0.50mm  | 
| Soap Dispenser      |
| MDF Board Sheets  |
| Cardboard Box x1   |   
| SG90 Micro Servo Motor x2  |  

## Circuit

![](https://i.imgur.com/tdU1LSW.png)

## License

[GNU GPLv3](https://choosealicense.com/licenses/gpl-3.0/)