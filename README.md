AUTHOR:  David OBERLEITNER, ic22b011@technikum-wien.at

DATE:    June 15, 2024

SUMMARY: This project is part of the semester project SHWM97, which is a weather station. 
	It consists of a base station, which is a server collecting data from the various stations
	and displays the information. 
	This module employs an altitude click, which collects pressure/altitude and temperature data. 
	The data is sent to the base station via WIFI/TCP. 
	The third module contains a humidity sensor and also a temperature sensor. 


## OS

The system uses FreeRTOS. 
Tasks: 
+ Altitude monitoring task: Starts and calibrates the altitude click. Gets one
altitude measurement at start-up. Periodically checks, 
if the module is active. Unless so, it will re-initialize. 

+ Data task: Polls the sensors for measurements (pressure and temperature). 
Converts the data and sends it to the sender task via message queue. 
Sends "-3333" as measured value in case of error. 

+ Wifi monitoring task: Initializes the WIFI module and connects to TCP server. 
Checks for connection periodically and restarts initialization if not connected. 

+ Sender task: Waits for data in the message queue. Builds TCP message string
and sends it to base station. 


## Circuitry

The WIFI click sits on port 1 and uses uart for communication. 
The altitude click uses I2C and sits on port 2. 

---

## Pins

The following pins are used for this project:

WIFI: 
+ PB0 WIFI_CS
+ PA9 USART1_TX
+ PA10 USART1_RX

ALTITUDE: 
+ PB6 I2C1_SCL
+ PB7 I2C1_SDA

USART to CONSOLE:
+ PA2 USART2_TX
+ PA15(JTDI) USART2_RX

---

## Connection/Interface

The WIFI module will try to connect to the AP with the credentials: 
+ SSID: "SHWM97"
+ Password: "1234567890"

... and to the server: 
+ Type: "TCP"
+ Remote host: "192.168.9.1"
+ Remote port: "5000"

The TCP message string to pass on the data: 
+ Device ID: "1"
+ Message format: "<device ID>,<physical quantity>,<value in float>;"
++ Altitude: "1,ALT,xxx.xx;"
++ Pressure: "1,P,xxx.xx;"
++ Temperature: "1,TP,xx.x;" (Note: "TP" stands for "temperature of pressure module"
				so the base station can distinguish the temp. data 
				from other modules' temp. data.) 


## Usage

Once plugged in, the controller should initialize the clicks and calibrate the
sensors. 
Once the TCP connection is up, it sends one altitude measurement to the base station
and then starts to take pressure and temperature measurements periodically 
(default: 5 seconds). This data is sent to the base station via TCP message. 
Both clicks are monitored and should restart initialization in case of inactivity. 

The base station will receive messages periodically. In case of sensor error, 
it will receive a message with "-3333" as value. 
The device ID is included in the message to give information on the sending device 
so that the base station can react to the sender going offline. 


For debugging, testing and troubleshooting, the module can be connected to a console 
interface and receive information about everything that happens. 
For getting debugging messages, uncomment "#define DEBUGGING" in main.h. 
In order to test only the altitude click's functions, uncomment "#define NO_WIFI"
in main.h. 

---