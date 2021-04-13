EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "Twomes Temperature Sattelite"
Date "2021-04-12"
Rev "1"
Comp "Lectoraat Energietransitie"
Comment1 "Author: Sjors Smit"
Comment2 "Reviewed by: "
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L RF_Module:ESP32-WROOM-32D U1
U 1 1 606F0BAD
P 4150 2500
F 0 "U1" H 4350 3950 50  0000 C CNN
F 1 "ESP32-WROOM-32D" H 4650 3850 50  0000 C CNN
F 2 "RF_Module:ESP32-WROOM-32" H 4150 1000 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf" H 3850 2550 50  0001 C CNN
	1    4150 2500
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Temperature:Si7051-A20 U2
U 1 1 606F29D2
P 9250 1850
F 0 "U2" H 9300 2100 50  0000 L CNN
F 1 "Si7051-A20" H 9300 1600 50  0000 L CNN
F 2 "Package_DFN_QFN:DFN-6-1EP_3x3mm_P1mm_EP1.65x2.55mm" H 9250 1450 50  0001 C CNN
F 3 "https://www.silabs.com/documents/public/data-sheets/Si7050-1-3-4-5-A20.pdf" H 9050 2150 50  0001 C CNN
	1    9250 1850
	1    0    0    -1  
$EndComp
$Comp
L Sensor_Temperature:DS18B20Z U3
U 1 1 606F312D
P 9800 4000
F 0 "U3" H 9750 4250 50  0000 R CNN
F 1 "DS18B20Z" H 9750 3750 50  0000 R CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 8800 3750 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf" H 9650 4250 50  0001 C CNN
	1    9800 4000
	-1   0    0    -1  
$EndComp
$Comp
L Device:Battery_Cell BT1
U 1 1 606F4C1D
P 950 2150
F 0 "BT1" H 1068 2246 50  0000 L CNN
F 1 "LS14500" H 1068 2155 50  0000 L CNN
F 2 "Battery:BatteryHolder_Keystone_2460_1xAA" V 950 2210 50  0001 C CNN
F 3 "~" V 950 2210 50  0001 C CNN
	1    950  2150
	1    0    0    -1  
$EndComp
Wire Notes Line
	2630 370  2630 7850
Wire Notes Line
	7990 6540 7990 400 
Wire Notes Line
	7150 400  7160 400 
$Comp
L Jumper:SolderJumper_2_Open JP1
U 1 1 607471E8
P 1700 6600
F 0 "JP1" H 1700 6805 50  0000 C CNN
F 1 "SolderJumper" H 1760 6690 50  0000 C CNN
F 2 "" H 1700 6600 50  0001 C CNN
F 3 "~" H 1700 6600 50  0001 C CNN
	1    1700 6600
	1    0    0    -1  
$EndComp
Text Label 1450 6700 2    50   ~ 0
FTDI_CTS
Text Label 1450 6500 2    50   ~ 0
RX0
Text Label 1450 6400 2    50   ~ 0
TX0
Text Label 1450 6300 2    50   ~ 0
FTDI_DTR
$Comp
L power:GND #PWR01
U 1 1 6074DA43
P 1900 6800
F 0 "#PWR01" H 1900 6550 50  0001 C CNN
F 1 "GND" H 1905 6627 50  0000 C CNN
F 2 "" H 1900 6800 50  0001 C CNN
F 3 "" H 1900 6800 50  0001 C CNN
	1    1900 6800
	1    0    0    -1  
$EndComp
Text Notes 2600 5900 2    71   ~ 14
FTDI-Compatible programming header
Wire Notes Line
	2630 5720 390  5720
Wire Notes Line
	390  6220 390  6200
$Comp
L Device:Q_NMOS_GDS Q1
U 1 1 60750CE6
P 1550 2400
F 0 "Q1" V 1799 2400 50  0000 C CNN
F 1 "Q_NMOS_GDS" V 1890 2400 50  0000 C CNN
F 2 "" H 1750 2500 50  0001 C CNN
F 3 "~" H 1550 2400 50  0001 C CNN
	1    1550 2400
	0    -1   1    0   
$EndComp
Wire Wire Line
	950  1750 1550 1750
Connection ~ 1550 1750
Wire Wire Line
	1350 2500 950  2500
Wire Wire Line
	950  1750 950  1950
Wire Wire Line
	950  2250 950  2500
Wire Wire Line
	1750 2500 2100 2500
Wire Wire Line
	1550 1750 2100 1750
$Comp
L power:GND #PWR03
U 1 1 60755507
P 2100 2500
F 0 "#PWR03" H 2100 2250 50  0001 C CNN
F 1 "GND" H 2105 2327 50  0000 C CNN
F 2 "" H 2100 2500 50  0001 C CNN
F 3 "" H 2100 2500 50  0001 C CNN
	1    2100 2500
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR04
U 1 1 60756C1B
P 2150 6600
F 0 "#PWR04" H 2150 6450 50  0001 C CNN
F 1 "VCC" H 2167 6773 50  0000 C CNN
F 2 "" H 2150 6600 50  0001 C CNN
F 3 "" H 2150 6600 50  0001 C CNN
	1    2150 6600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1850 6600 2150 6600
Text Notes 2150 1150 2    100  ~ 20
Battery and Reverse\nVoltage protection
Text Notes 1550 1350 2    50   ~ 0
Vcc(max) = 3.6V\n
Wire Wire Line
	4150 1100 4150 1000
$Comp
L power:VCC #PWR07
U 1 1 607591E8
P 4150 950
F 0 "#PWR07" H 4150 800 50  0001 C CNN
F 1 "VCC" H 4167 1123 50  0000 C CNN
F 2 "" H 4150 950 50  0001 C CNN
F 3 "" H 4150 950 50  0001 C CNN
	1    4150 950 
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR08
U 1 1 607594C6
P 4150 4000
F 0 "#PWR08" H 4150 3750 50  0001 C CNN
F 1 "GND" H 4155 3827 50  0000 C CNN
F 2 "" H 4150 4000 50  0001 C CNN
F 3 "" H 4150 4000 50  0001 C CNN
	1    4150 4000
	1    0    0    -1  
$EndComp
Wire Wire Line
	4150 4000 4150 3900
$Comp
L Switch:SW_Push SW1
U 1 1 6075C844
P 3300 7050
F 0 "SW1" V 3350 7300 50  0000 R CNN
F 1 "SW_Push" V 3200 7450 50  0000 R CNN
F 2 "" H 3300 7250 50  0001 C CNN
F 3 "~" H 3300 7250 50  0001 C CNN
	1    3300 7050
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3300 7350 3300 7300
Wire Wire Line
	3300 6850 3300 6600
Text Label 3250 6600 2    50   ~ 0
Reset
Text Label 4000 6600 2    50   ~ 0
GPIO0
Wire Wire Line
	3300 6600 3250 6600
Wire Wire Line
	3300 6600 3300 6400
Connection ~ 3300 6600
Wire Wire Line
	4550 6600 4500 6600
Wire Wire Line
	4050 6600 4000 6600
Text Label 4500 6600 2    50   ~ 0
GPIO23
Wire Wire Line
	4550 6850 4550 6600
Wire Wire Line
	4050 6850 4050 6600
$Comp
L Switch:SW_Push SW3
U 1 1 6075D21B
P 4550 7050
F 0 "SW3" V 4596 7002 50  0000 R CNN
F 1 "SW_Push" V 4505 7002 50  0000 R CNN
F 2 "" H 4550 7250 50  0001 C CNN
F 3 "~" H 4550 7250 50  0001 C CNN
	1    4550 7050
	0    -1   -1   0   
$EndComp
$Comp
L Switch:SW_Push SW2
U 1 1 6075CD77
P 4050 7050
F 0 "SW2" V 4096 7002 50  0000 R CNN
F 1 "SW_Push" V 4005 7002 50  0000 R CNN
F 2 "" H 4050 7250 50  0001 C CNN
F 3 "~" H 4050 7250 50  0001 C CNN
	1    4050 7050
	0    -1   -1   0   
$EndComp
$Comp
L Device:R R2
U 1 1 607682F2
P 3300 6250
F 0 "R2" H 3370 6296 50  0000 L CNN
F 1 "10K" H 3370 6205 50  0000 L CNN
F 2 "" V 3230 6250 50  0001 C CNN
F 3 "~" H 3300 6250 50  0001 C CNN
	1    3300 6250
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR05
U 1 1 60768A04
P 3300 6050
F 0 "#PWR05" H 3300 5900 50  0001 C CNN
F 1 "VCC" H 3317 6223 50  0000 C CNN
F 2 "" H 3300 6050 50  0001 C CNN
F 3 "" H 3300 6050 50  0001 C CNN
	1    3300 6050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 6050 3300 6100
$Comp
L power:GND #PWR06
U 1 1 60769798
P 3300 7350
F 0 "#PWR06" H 3300 7100 50  0001 C CNN
F 1 "GND" H 3305 7177 50  0000 C CNN
F 2 "" H 3300 7350 50  0001 C CNN
F 3 "" H 3300 7350 50  0001 C CNN
	1    3300 7350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR09
U 1 1 60769D6E
P 4050 7250
F 0 "#PWR09" H 4050 7000 50  0001 C CNN
F 1 "GND" H 4055 7077 50  0000 C CNN
F 2 "" H 4050 7250 50  0001 C CNN
F 3 "" H 4050 7250 50  0001 C CNN
	1    4050 7250
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR010
U 1 1 6076A2F9
P 4550 7250
F 0 "#PWR010" H 4550 7000 50  0001 C CNN
F 1 "GND" H 4555 7077 50  0000 C CNN
F 2 "" H 4550 7250 50  0001 C CNN
F 3 "" H 4550 7250 50  0001 C CNN
	1    4550 7250
	1    0    0    -1  
$EndComp
Text Notes 3750 7650 0    50   ~ 0
User inputs\n(Use internal Pull-Up resistors)
Text Notes 3500 7700 2    50   ~ 0
Reset Switch
$Comp
L Device:LED D1
U 1 1 6076F143
P 5600 7100
F 0 "D1" V 5639 6982 50  0000 R CNN
F 1 "RED_LED" V 5548 6982 50  0000 R CNN
F 2 "" H 5600 7100 50  0001 C CNN
F 3 "~" H 5600 7100 50  0001 C CNN
	1    5600 7100
	0    -1   -1   0   
$EndComp
$Comp
L Device:LED D2
U 1 1 6076FCF8
P 6200 7100
F 0 "D2" V 6239 6982 50  0000 R CNN
F 1 "GREEN_LED" V 6148 6982 50  0000 R CNN
F 2 "" H 6200 7100 50  0001 C CNN
F 3 "~" H 6200 7100 50  0001 C CNN
	1    6200 7100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	6200 7250 6200 7350
Wire Wire Line
	5600 7350 5600 7250
Wire Wire Line
	5600 6950 5600 6850
Wire Wire Line
	6200 6850 6200 6950
$Comp
L Device:R R4
U 1 1 60771BB4
P 6200 6700
F 0 "R4" H 6270 6746 50  0000 L CNN
F 1 "1K" H 6270 6655 50  0000 L CNN
F 2 "" V 6130 6700 50  0001 C CNN
F 3 "~" H 6200 6700 50  0001 C CNN
	1    6200 6700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R3
U 1 1 60772220
P 5600 6700
F 0 "R3" H 5670 6746 50  0000 L CNN
F 1 "1K" H 5670 6655 50  0000 L CNN
F 2 "" V 5530 6700 50  0001 C CNN
F 3 "~" H 5600 6700 50  0001 C CNN
	1    5600 6700
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 6550 5600 6350
Wire Wire Line
	6200 6550 6200 6350
Text Label 6450 6350 2    50   ~ 0
LED_STATUS
Text Label 5750 6350 2    50   ~ 0
LED_ERROR
Wire Wire Line
	6200 6350 6450 6350
Wire Wire Line
	5600 6350 5750 6350
$Comp
L power:GND #PWR012
U 1 1 60778DFC
P 6200 7350
F 0 "#PWR012" H 6200 7100 50  0001 C CNN
F 1 "GND" H 6205 7177 50  0000 C CNN
F 2 "" H 6200 7350 50  0001 C CNN
F 3 "" H 6200 7350 50  0001 C CNN
	1    6200 7350
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR011
U 1 1 607790FE
P 5600 7350
F 0 "#PWR011" H 5600 7100 50  0001 C CNN
F 1 "GND" H 5605 7177 50  0000 C CNN
F 2 "" H 5600 7350 50  0001 C CNN
F 3 "" H 5600 7350 50  0001 C CNN
	1    5600 7350
	1    0    0    -1  
$EndComp
$Comp
L Device:C C1
U 1 1 60779C5A
P 3500 7050
F 0 "C1" H 3615 7096 50  0000 L CNN
F 1 "1uF" H 3615 7005 50  0000 L CNN
F 2 "" H 3538 6900 50  0001 C CNN
F 3 "~" H 3500 7050 50  0001 C CNN
	1    3500 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	3500 6900 3500 6600
Wire Wire Line
	3500 6600 3300 6600
Wire Wire Line
	3500 7200 3500 7300
Wire Wire Line
	3500 7300 3300 7300
Connection ~ 3300 7300
Wire Wire Line
	3300 7300 3300 7250
$Comp
L Connector_Generic:Conn_01x04 J2
U 1 1 6077CB05
P 7100 3950
F 0 "J2" H 7050 4250 50  0000 L CNN
F 1 "Molex PicoFlex 90814" H 7050 4150 50  0000 L CNN
F 2 "" H 7100 3950 50  0001 C CNN
F 3 "~" H 7100 3950 50  0001 C CNN
	1    7100 3950
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 6077D2D4
P 7100 5250
F 0 "J3" H 7050 5500 50  0000 L CNN
F 1 "Molex PicoFlex 90814" H 7050 5450 50  0000 L CNN
F 2 "" H 7100 5250 50  0001 C CNN
F 3 "~" H 7100 5250 50  0001 C CNN
	1    7100 5250
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J4
U 1 1 6077D59A
P 8400 4000
F 0 "J4" H 8318 4317 50  0000 C CNN
F 1 "Molex PicoFlex 90814" H 8318 4226 50  0000 C CNN
F 2 "" H 8400 4000 50  0001 C CNN
F 3 "~" H 8400 4000 50  0001 C CNN
	1    8400 4000
	-1   0    0    -1  
$EndComp
Wire Notes Line
	11300 3000 8000 3000
Text Notes 9700 900  2    79   ~ 16
On same PCB as ESP32
Text Notes 9950 750  2    79   ~ 16
Si7051 I2C Temperature Sensor
Text Notes 9900 3200 2    79   ~ 16
DS18B20 Temperature Sensors
Text Notes 9550 3350 2    79   ~ 16
*On seperate breakout
Wire Wire Line
	8600 4200 8900 4200
Wire Wire Line
	8900 4200 8900 4350
Wire Wire Line
	8900 4350 9800 4350
Wire Wire Line
	8600 4100 8900 4100
Wire Wire Line
	8900 4100 8900 4200
Connection ~ 8900 4200
Wire Wire Line
	8600 3900 9450 3900
Wire Wire Line
	9450 3700 9800 3700
Wire Wire Line
	8600 4000 9500 4000
Wire Wire Line
	9450 3700 9450 3900
Wire Wire Line
	9800 4350 9800 4300
$Comp
L Sensor_Temperature:DS18B20Z U4
U 1 1 6078B959
P 9800 5250
F 0 "U4" H 9750 5500 50  0000 R CNN
F 1 "DS18B20Z" H 9750 5000 50  0000 R CNN
F 2 "Package_SO:SOIC-8_3.9x4.9mm_P1.27mm" H 8800 5000 50  0001 C CNN
F 3 "http://datasheets.maximintegrated.com/en/ds/DS18B20.pdf" H 9650 5500 50  0001 C CNN
	1    9800 5250
	-1   0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J5
U 1 1 6078B95F
P 8400 5250
F 0 "J5" H 8318 5567 50  0000 C CNN
F 1 "Molex PicoFlex 90814" H 8318 5476 50  0000 C CNN
F 2 "" H 8400 5250 50  0001 C CNN
F 3 "~" H 8400 5250 50  0001 C CNN
	1    8400 5250
	-1   0    0    -1  
$EndComp
Wire Wire Line
	8600 5450 8900 5450
Wire Wire Line
	8900 5450 8900 5600
Wire Wire Line
	8900 5600 9800 5600
Wire Wire Line
	8600 5350 8900 5350
Wire Wire Line
	8900 5350 8900 5450
Connection ~ 8900 5450
Wire Wire Line
	8600 5150 9450 5150
Wire Wire Line
	9450 4950 9800 4950
Wire Wire Line
	8600 5250 9500 5250
Wire Wire Line
	9450 4950 9450 5150
Wire Wire Line
	9800 5600 9800 5550
Wire Wire Line
	6900 3850 6800 3850
Wire Wire Line
	6900 4050 6900 4150
Wire Wire Line
	6900 4150 6800 4150
Connection ~ 6900 4150
Wire Wire Line
	6900 5350 6900 5450
Connection ~ 6900 5450
Wire Wire Line
	6900 5150 6800 5150
$Comp
L power:VCC #PWR019
U 1 1 6079C0E5
P 6800 3650
F 0 "#PWR019" H 6800 3500 50  0001 C CNN
F 1 "VCC" H 6817 3823 50  0000 C CNN
F 2 "" H 6800 3650 50  0001 C CNN
F 3 "" H 6800 3650 50  0001 C CNN
	1    6800 3650
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR021
U 1 1 6079C490
P 6800 4950
F 0 "#PWR021" H 6800 4800 50  0001 C CNN
F 1 "VCC" H 6817 5123 50  0000 C CNN
F 2 "" H 6800 4950 50  0001 C CNN
F 3 "" H 6800 4950 50  0001 C CNN
	1    6800 4950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR020
U 1 1 6079C8D0
P 6800 4150
F 0 "#PWR020" H 6800 3900 50  0001 C CNN
F 1 "GND" H 6805 3977 50  0000 C CNN
F 2 "" H 6800 4150 50  0001 C CNN
F 3 "" H 6800 4150 50  0001 C CNN
	1    6800 4150
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR022
U 1 1 6079CF00
P 6800 5450
F 0 "#PWR022" H 6800 5200 50  0001 C CNN
F 1 "GND" H 6805 5277 50  0000 C CNN
F 2 "" H 6800 5450 50  0001 C CNN
F 3 "" H 6800 5450 50  0001 C CNN
	1    6800 5450
	1    0    0    -1  
$EndComp
Wire Wire Line
	6800 5450 6900 5450
Text Label 5850 3950 0    50   ~ 0
TempIn_ESP
Text Label 5850 5250 0    50   ~ 0
TempOut_ESP
Text Notes 5650 7700 0    50   ~ 0
Indicator LEDs
Wire Notes Line
	2650 5800 8000 5800
Text Notes 4100 6050 0    79   ~ 16
User I/O
Text Notes 600  2950 0    50   ~ 0
*Reverse voltage circuit still needs testing
Wire Notes Line
	400  3100 2600 3100
Text Notes 650  3350 0    79   ~ 16
SuperCapacitor
$Comp
L Device:CP C2
U 1 1 607B5CBE
P 1250 4400
F 0 "C2" H 1400 4500 50  0000 L CNN
F 1 "CP" H 1368 4355 50  0001 L CNN
F 2 "" H 1288 4250 50  0001 C CNN
F 3 "~" H 1250 4400 50  0001 C CNN
	1    1250 4400
	1    0    0    -1  
$EndComp
Wire Notes Line
	1050 4100 1050 4800
Wire Notes Line
	1050 4800 1850 4800
Wire Notes Line
	1850 4800 1850 4100
Wire Notes Line
	1050 4100 1850 4100
Text Notes 1400 4600 0    50   ~ 0
5V\n0.47F\nsuperCap
$Comp
L power:VCC #PWR014
U 1 1 607C1177
P 2100 1750
F 0 "#PWR014" H 2100 1600 50  0001 C CNN
F 1 "VCC" H 2117 1923 50  0000 C CNN
F 2 "" H 2100 1750 50  0001 C CNN
F 3 "" H 2100 1750 50  0001 C CNN
	1    2100 1750
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR013
U 1 1 607C31B6
P 1250 4900
F 0 "#PWR013" H 1250 4650 50  0001 C CNN
F 1 "GND" H 1255 4727 50  0000 C CNN
F 2 "" H 1250 4900 50  0001 C CNN
F 3 "" H 1250 4900 50  0001 C CNN
	1    1250 4900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1250 4550 1250 4900
Wire Wire Line
	1250 3900 1100 3900
Wire Wire Line
	1250 3900 1250 4250
Wire Wire Line
	1250 3900 1650 3900
Connection ~ 1250 3900
$Comp
L Device:R R1
U 1 1 607C9F50
P 950 3900
F 0 "R1" V 850 3900 50  0000 C CNN
F 1 "*?R" V 950 3900 50  0000 C CNN
F 2 "" V 880 3900 50  0001 C CNN
F 3 "~" H 950 3900 50  0001 C CNN
	1    950  3900
	0    1    1    0   
$EndComp
Wire Wire Line
	800  3900 650  3900
Wire Wire Line
	650  3900 650  3800
$Comp
L power:VCC #PWR02
U 1 1 607CBF07
P 650 3800
F 0 "#PWR02" H 650 3650 50  0001 C CNN
F 1 "VCC" H 667 3973 50  0000 C CNN
F 2 "" H 650 3800 50  0001 C CNN
F 3 "" H 650 3800 50  0001 C CNN
	1    650  3800
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Schottky D3
U 1 1 607CC479
P 1800 3900
F 0 "D3" H 1800 3684 50  0000 C CNN
F 1 "D_Schottky" H 1800 3775 50  0000 C CNN
F 2 "" H 1800 3900 50  0001 C CNN
F 3 "~" H 1800 3900 50  0001 C CNN
	1    1800 3900
	-1   0    0    1   
$EndComp
Wire Wire Line
	1950 3900 2150 3900
Text Label 2150 3900 0    50   ~ 0
V_CAP
Text Notes 700  5250 0    50   ~ 0
*SuperCapacitor Circuit still needs testing
$Comp
L power:VCC #PWR023
U 1 1 607D2521
P 9250 1450
F 0 "#PWR023" H 9250 1300 50  0001 C CNN
F 1 "VCC" H 9267 1623 50  0000 C CNN
F 2 "" H 9250 1450 50  0001 C CNN
F 3 "" H 9250 1450 50  0001 C CNN
	1    9250 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 1450 9250 1500
Wire Wire Line
	9250 2150 9250 2200
$Comp
L power:GND #PWR024
U 1 1 607D5B66
P 9250 2250
F 0 "#PWR024" H 9250 2000 50  0001 C CNN
F 1 "GND" H 9255 2077 50  0000 C CNN
F 2 "" H 9250 2250 50  0001 C CNN
F 3 "" H 9250 2250 50  0001 C CNN
	1    9250 2250
	1    0    0    -1  
$EndComp
Wire Wire Line
	8850 1750 8450 1750
Wire Wire Line
	8450 1850 8850 1850
Text Label 8450 1850 0    50   ~ 0
ESP_SDA
Text Label 8450 1750 0    50   ~ 0
ESP_SCL
Wire Wire Line
	9250 1500 9950 1500
Connection ~ 9250 1500
Wire Wire Line
	9250 1500 9250 1550
Wire Wire Line
	9250 2200 9950 2200
Connection ~ 9250 2200
Wire Wire Line
	9250 2200 9250 2250
$Comp
L Device:C C5
U 1 1 607E3D43
P 9950 1850
F 0 "C5" H 10065 1896 50  0000 L CNN
F 1 "100nF" H 10065 1805 50  0000 L CNN
F 2 "" H 9988 1700 50  0001 C CNN
F 3 "~" H 9950 1850 50  0001 C CNN
	1    9950 1850
	1    0    0    -1  
$EndComp
Wire Wire Line
	9950 2000 9950 2200
Wire Wire Line
	9950 1700 9950 1500
Wire Wire Line
	9800 3700 10350 3700
Connection ~ 9800 3700
Wire Wire Line
	9800 4350 10350 4350
Connection ~ 9800 4350
Wire Wire Line
	9800 4950 10350 4950
Connection ~ 9800 4950
Wire Wire Line
	9800 5600 10350 5600
Connection ~ 9800 5600
$Comp
L Device:C C7
U 1 1 607F4E08
P 10350 5300
F 0 "C7" H 10465 5346 50  0000 L CNN
F 1 "1uF" H 10465 5255 50  0000 L CNN
F 2 "" H 10388 5150 50  0001 C CNN
F 3 "~" H 10350 5300 50  0001 C CNN
	1    10350 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C9
U 1 1 607F5423
P 10750 5300
F 0 "C9" H 10865 5346 50  0000 L CNN
F 1 "100nF" H 10865 5255 50  0000 L CNN
F 2 "" H 10788 5150 50  0001 C CNN
F 3 "~" H 10750 5300 50  0001 C CNN
	1    10750 5300
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 607F57ED
P 10350 4050
F 0 "C6" H 10465 4096 50  0000 L CNN
F 1 "1uF" H 10465 4005 50  0000 L CNN
F 2 "" H 10388 3900 50  0001 C CNN
F 3 "~" H 10350 4050 50  0001 C CNN
	1    10350 4050
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 607F5BB8
P 10750 4050
F 0 "C8" H 10865 4096 50  0000 L CNN
F 1 "100nF" H 10865 4005 50  0000 L CNN
F 2 "" H 10788 3900 50  0001 C CNN
F 3 "~" H 10750 4050 50  0001 C CNN
	1    10750 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	10350 4200 10350 4350
Connection ~ 10350 4350
Wire Wire Line
	10350 3900 10350 3700
Connection ~ 10350 3700
Wire Wire Line
	10350 3700 10750 3700
Wire Wire Line
	10350 4350 10750 4350
Wire Wire Line
	10750 4200 10750 4350
Wire Wire Line
	10750 3700 10750 3900
Wire Wire Line
	10750 4950 10750 5150
Wire Wire Line
	10750 5450 10750 5600
Wire Wire Line
	10350 5600 10350 5450
Connection ~ 10350 5600
Wire Wire Line
	10350 5600 10750 5600
Wire Wire Line
	10350 5150 10350 4950
Connection ~ 10350 4950
Wire Wire Line
	10350 4950 10750 4950
Text Notes 9600 5850 0    50   ~ 0
*DS18B20 have extra capacitors due\nto the longer wires
Text Notes 8850 6900 0    50   ~ 0
All resistors are 0603 1/4W\nAll unpolarized capacitors are 0603 MLCC >5V\nAll Polarized capacitors are SMD aluminium capacitors >5V\nUnless otherwise specified
Wire Wire Line
	3550 1300 3250 1300
Text Label 3250 1300 0    50   ~ 0
Reset
Wire Wire Line
	5850 3950 6450 3950
$Comp
L Device:R R8
U 1 1 608352D8
P 6600 3850
F 0 "R8" V 6500 3850 50  0000 C CNN
F 1 "4k7" V 6600 3850 50  0000 C CNN
F 2 "" V 6530 3850 50  0001 C CNN
F 3 "~" H 6600 3850 50  0001 C CNN
	1    6600 3850
	0    1    1    0   
$EndComp
Wire Wire Line
	6800 3850 6750 3850
Connection ~ 6800 3850
Wire Wire Line
	6450 3850 6450 3950
Connection ~ 6450 3950
Wire Wire Line
	6450 3950 6900 3950
Wire Wire Line
	6800 3650 6800 3850
Wire Wire Line
	6800 4950 6800 5150
$Comp
L Device:R R9
U 1 1 60842E5B
P 6650 5150
F 0 "R9" V 6550 5150 50  0000 C CNN
F 1 "4k7" V 6650 5150 50  0000 C CNN
F 2 "" V 6580 5150 50  0001 C CNN
F 3 "~" H 6650 5150 50  0001 C CNN
	1    6650 5150
	0    1    1    0   
$EndComp
Connection ~ 6800 5150
Wire Wire Line
	6500 5150 6500 5250
Connection ~ 6500 5250
Wire Wire Line
	6500 5250 6900 5250
Wire Wire Line
	5850 5250 6500 5250
Text Label 5300 1300 2    50   ~ 0
GPIO0
Text Label 5300 1400 2    50   ~ 0
TX0
Text Label 5350 1600 2    50   ~ 0
RX0
Text Label 5350 2700 2    50   ~ 0
ESP_SDA
Text Label 5350 2800 2    50   ~ 0
ESP_SCL
Text Label 5350 2900 2    50   ~ 0
GPIO23
Text Label 5350 3000 2    50   ~ 0
TempIn_ESP
Text Label 5350 3100 2    50   ~ 0
TempOut_ESP
Wire Wire Line
	4750 3100 5350 3100
Wire Wire Line
	4750 3000 5350 3000
Wire Wire Line
	4750 2900 5350 2900
Wire Wire Line
	4750 1400 5300 1400
Wire Wire Line
	4750 1300 5300 1300
Text Notes 1300 7400 0    50   ~ 0
*TX and RX are ESP-Pins\npins are switched on \nFTDI/CP2102 breakout
Text Notes 4800 6450 2    50   ~ 0
*GPIO0 also used to enter\nprogramming mode on boot
$Comp
L power:VCC #PWR015
U 1 1 6087C4C1
P 5450 2300
F 0 "#PWR015" H 5450 2150 50  0001 C CNN
F 1 "VCC" H 5467 2473 50  0000 C CNN
F 2 "" H 5450 2300 50  0001 C CNN
F 3 "" H 5450 2300 50  0001 C CNN
	1    5450 2300
	1    0    0    -1  
$EndComp
$Comp
L power:VCC #PWR016
U 1 1 6087CBBC
P 5750 2300
F 0 "#PWR016" H 5750 2150 50  0001 C CNN
F 1 "VCC" H 5767 2473 50  0000 C CNN
F 2 "" H 5750 2300 50  0001 C CNN
F 3 "" H 5750 2300 50  0001 C CNN
	1    5750 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R6
U 1 1 6087D1C5
P 5450 2500
F 0 "R6" H 5380 2454 50  0000 R CNN
F 1 "10k" H 5380 2545 50  0000 R CNN
F 2 "" V 5380 2500 50  0001 C CNN
F 3 "~" H 5450 2500 50  0001 C CNN
	1    5450 2500
	-1   0    0    1   
$EndComp
$Comp
L Device:R R7
U 1 1 6087DA42
P 5750 2500
F 0 "R7" H 5680 2454 50  0000 R CNN
F 1 "10k" H 5680 2545 50  0000 R CNN
F 2 "" V 5680 2500 50  0001 C CNN
F 3 "~" H 5750 2500 50  0001 C CNN
	1    5750 2500
	-1   0    0    1   
$EndComp
Wire Wire Line
	5450 2700 5450 2650
Wire Wire Line
	4750 2700 5450 2700
Wire Wire Line
	5750 2800 5750 2650
Wire Wire Line
	4750 2800 5750 2800
Text Notes 6700 2400 2    50   ~ 0
I2C pull-up resistors
Wire Wire Line
	5750 2300 5750 2350
Wire Wire Line
	5450 2300 5450 2350
$Comp
L Device:R R5
U 1 1 60890ACB
P 5000 1600
F 0 "R5" V 5100 1600 50  0000 C CNN
F 1 "1k" V 5000 1600 50  0000 C CNN
F 2 "" V 4930 1600 50  0001 C CNN
F 3 "~" H 5000 1600 50  0001 C CNN
	1    5000 1600
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5350 1600 5150 1600
Wire Wire Line
	4850 1600 4750 1600
Text Notes 2600 5350 2    50   ~ 0
*SuperCap charge resistor still needs to be calculated
$Comp
L Device:C C3
U 1 1 6089964B
P 5600 1400
F 0 "C3" H 5715 1446 50  0000 L CNN
F 1 "100nF" H 5715 1355 50  0000 L CNN
F 2 "" H 5638 1250 50  0001 C CNN
F 3 "~" H 5600 1400 50  0001 C CNN
	1    5600 1400
	1    0    0    -1  
$EndComp
$Comp
L Device:CP C4
U 1 1 6089A1E9
P 6100 1400
F 0 "C4" H 6218 1446 50  0000 L CNN
F 1 "470uF" H 6218 1355 50  0000 L CNN
F 2 "" H 6138 1250 50  0001 C CNN
F 3 "~" H 6100 1400 50  0001 C CNN
	1    6100 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	5600 1250 5600 1200
Wire Wire Line
	5600 1200 5850 1200
Wire Wire Line
	6100 1200 6100 1250
Wire Wire Line
	6100 1550 6100 1600
Wire Wire Line
	5600 1600 5600 1550
$Comp
L power:GND #PWR018
U 1 1 608A314B
P 5850 1600
F 0 "#PWR018" H 5850 1350 50  0001 C CNN
F 1 "GND" H 5855 1427 50  0000 C CNN
F 2 "" H 5850 1600 50  0001 C CNN
F 3 "" H 5850 1600 50  0001 C CNN
	1    5850 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5850 1600 5600 1600
$Comp
L power:VCC #PWR017
U 1 1 608A373B
P 5850 1200
F 0 "#PWR017" H 5850 1050 50  0001 C CNN
F 1 "VCC" H 5867 1373 50  0000 C CNN
F 2 "" H 5850 1200 50  0001 C CNN
F 3 "" H 5850 1200 50  0001 C CNN
	1    5850 1200
	1    0    0    -1  
$EndComp
Connection ~ 5850 1200
Wire Wire Line
	5850 1600 6100 1600
Connection ~ 5850 1600
Wire Wire Line
	5850 1200 6100 1200
Text Notes 7200 1250 2    50   ~ 0
*Place decoupling capacitors as\nclose to ESP32 as possible
Wire Wire Line
	1550 1750 1550 2200
Connection ~ 4150 1000
Wire Wire Line
	4150 1000 4150 950 
Text Label 3750 1000 0    50   ~ 0
V_CAP
Wire Wire Line
	3750 1000 4150 1000
NoConn ~ 4750 3600
NoConn ~ 4750 3500
NoConn ~ 4750 3400
NoConn ~ 4750 3200
NoConn ~ 4750 3300
NoConn ~ 4750 2600
NoConn ~ 4750 2500
NoConn ~ 4750 2400
NoConn ~ 4750 2100
NoConn ~ 4750 2000
NoConn ~ 4750 1900
NoConn ~ 4750 1800
NoConn ~ 4750 1700
NoConn ~ 4750 1500
NoConn ~ 3550 1500
NoConn ~ 3550 1600
NoConn ~ 3550 3000
NoConn ~ 3550 2900
NoConn ~ 3550 2800
NoConn ~ 3550 2700
NoConn ~ 3550 2600
NoConn ~ 3550 2500
Text Label 5300 2300 2    50   ~ 0
LED_STATUS
Wire Wire Line
	4750 2300 5300 2300
Wire Wire Line
	4750 2200 5300 2200
Text Label 5300 2200 2    50   ~ 0
LED_ERROR
$Comp
L Connector:Conn_01x06_Female J1
U 1 1 6074864B
P 850 6500
F 0 "J1" H 750 6850 50  0000 C CNN
F 1 "Female header" H 730 6110 50  0000 C CNN
F 2 "" H 850 6500 50  0001 C CNN
F 3 "~" H 850 6500 50  0001 C CNN
	1    850  6500
	-1   0    0    -1  
$EndComp
Wire Wire Line
	1050 6300 1450 6300
Wire Wire Line
	1450 6400 1050 6400
Wire Wire Line
	1050 6500 1450 6500
Wire Wire Line
	1550 6600 1050 6600
Wire Wire Line
	1050 6700 1450 6700
Wire Wire Line
	1050 6800 1900 6800
NoConn ~ 1050 6700
NoConn ~ 1050 6300
$EndSCHEMATC
