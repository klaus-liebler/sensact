EESchema Schematic File Version 2
LIBS:multiupbox-rescue
LIBS:power
LIBS:device
LIBS:transistors
LIBS:conn
LIBS:linear
LIBS:regul
LIBS:74xx
LIBS:cmos4000
LIBS:adc-dac
LIBS:memory
LIBS:xilinx
LIBS:microcontrollers
LIBS:dsp
LIBS:microchip
LIBS:analog_switches
LIBS:motorola
LIBS:texas
LIBS:intel
LIBS:audio
LIBS:interface
LIBS:digital-audio
LIBS:philips
LIBS:display
LIBS:cypress
LIBS:siliconi
LIBS:opto
LIBS:atmel
LIBS:contrib
LIBS:valves
LIBS:stm32
LIBS:w_microcontrollers
LIBS:multiupbox-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 4
Title "Multi-HutschieneSTM32F1"
Date "So 22 Mär 2015"
Rev "0.2"
Comp "Klaus Liebler"
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L ULN2803 U3
U 1 1 54E561D0
P 2050 1600
F 0 "U3" H 2200 2150 60  0000 C CNN
F 1 "ULN2803" H 2300 2050 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-18_7.5x11.6mm_Pitch1.27mm" H 2050 1600 60  0001 C CNN
F 3 "" H 2050 1600 60  0000 C CNN
	1    2050 1600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR073
U 1 1 54E563E0
P 2050 2250
F 0 "#PWR073" H 2050 2000 60  0001 C CNN
F 1 "GND" H 2050 2100 60  0000 C CNN
F 2 "" H 2050 2250 60  0000 C CNN
F 3 "" H 2050 2250 60  0000 C CNN
	1    2050 2250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR074
U 1 1 54E563F2
P 7400 3600
F 0 "#PWR074" H 7400 3350 60  0001 C CNN
F 1 "GND" H 7400 3450 60  0000 C CNN
F 2 "" H 7400 3600 60  0000 C CNN
F 3 "" H 7400 3600 60  0000 C CNN
	1    7400 3600
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P6
U 1 1 54E56CD2
P 5600 6300
F 0 "P6" H 5600 6450 50  0000 C CNN
F 1 "CONN_01X02" V 5700 6300 50  0000 C CNN
F 2 "kl_footprints:conn_2P" H 5600 6300 60  0001 C CNN
F 3 "" H 5600 6300 60  0000 C CNN
	1    5600 6300
	0    1    1    0   
$EndComp
$Comp
L CONN_01X02 P7
U 1 1 54E57445
P 4100 1350
F 0 "P7" H 4100 1500 50  0000 C CNN
F 1 "CONN_01X02" V 4200 1350 50  0000 C CNN
F 2 "kl_footprints:conn_2P" H 4100 1350 60  0001 C CNN
F 3 "" H 4100 1350 60  0000 C CNN
	1    4100 1350
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P8
U 1 1 54E574B4
P 4100 2650
F 0 "P8" H 4100 2800 50  0000 C CNN
F 1 "CONN_01X02" V 4200 2650 50  0000 C CNN
F 2 "kl_footprints:conn_2P" H 4100 2650 60  0001 C CNN
F 3 "" H 4100 2650 60  0000 C CNN
	1    4100 2650
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P9
U 1 1 54E574ED
P 4100 3950
F 0 "P9" H 4100 4100 50  0000 C CNN
F 1 "CONN_01X02" V 4200 3950 50  0000 C CNN
F 2 "kl_footprints:conn_2P" H 4100 3950 60  0001 C CNN
F 3 "" H 4100 3950 60  0000 C CNN
	1    4100 3950
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P11
U 1 1 54E57A48
P 10000 2700
F 0 "P11" H 10000 2850 50  0000 C CNN
F 1 "CONN_01X02" V 10100 2700 50  0000 C CNN
F 2 "kl_footprints:conn_2P" H 10000 2700 60  0001 C CNN
F 3 "" H 10000 2700 60  0000 C CNN
	1    10000 2700
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P12
U 1 1 54E57AEB
P 10000 4000
F 0 "P12" H 10000 4150 50  0000 C CNN
F 1 "CONN_01X02" V 10100 4000 50  0000 C CNN
F 2 "kl_footprints:conn_2P" H 10000 4000 60  0001 C CNN
F 3 "" H 10000 4000 60  0000 C CNN
	1    10000 4000
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P13
U 1 1 54E57B28
P 10000 5300
F 0 "P13" H 10000 5450 50  0000 C CNN
F 1 "CONN_01X02" V 10100 5300 50  0000 C CNN
F 2 "kl_footprints:conn_2P" H 10000 5300 60  0001 C CNN
F 3 "" H 10000 5300 60  0000 C CNN
	1    10000 5300
	1    0    0    -1  
$EndComp
Text HLabel 1250 2850 0    60   BiDi ~ 0
PC[0..15]
Text HLabel 1250 2550 0    60   BiDi ~ 0
PA[0..15]
Text HLabel 1250 2700 0    60   BiDi ~ 0
PB[0..15]
Text HLabel 1550 1750 0    60   Input ~ 0
PD2
NoConn ~ 4600 -1700
Text Label 6750 2650 0    60   ~ 0
PB9
Text Label 6750 2750 0    60   ~ 0
PB8
Text Label 6750 2550 0    60   ~ 0
PC13
Text Label 6750 2450 0    60   ~ 0
PC14
Text Label 6750 2350 0    60   ~ 0
PC15
Text Label 6750 3050 0    60   ~ 0
PB5
Text Label 6750 2950 0    60   ~ 0
PB6
Text Label 6750 2850 0    60   ~ 0
PB7
Text Label 1350 1250 0    60   ~ 0
PC9
Text Label 1350 1350 0    60   ~ 0
PA15
Text Label 1350 1450 0    60   ~ 0
PC10
Text Label 1350 1550 0    60   ~ 0
PC11
Text Label 1350 1650 0    60   ~ 0
PC12
Text Label 1350 1850 0    60   ~ 0
PB3
Text Label 1350 1950 0    60   ~ 0
PB4
$Comp
L ULN2803 U4
U 1 1 54E56223
P 7400 2700
F 0 "U4" H 7550 3250 60  0000 C CNN
F 1 "ULN2803" H 7650 3150 60  0000 C CNN
F 2 "Housings_SOIC:SOIC-18_7.5x11.6mm_Pitch1.27mm" H 7400 2700 60  0001 C CNN
F 3 "" H 7400 2700 60  0000 C CNN
	1    7400 2700
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY1
U 1 1 5509343E
P 3300 1150
F 0 "RLY1" H 3300 1400 60  0000 C CNN
F 1 "RELAY_34.51" H 3300 915 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 3300 1150 60  0001 C CNN
F 3 "" H 3300 1150 60  0000 C CNN
	1    3300 1150
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY2
U 1 1 55093795
P 3300 1800
F 0 "RLY2" H 3300 2050 60  0000 C CNN
F 1 "RELAY_34.51" H 3300 1565 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 3300 1800 60  0001 C CNN
F 3 "" H 3300 1800 60  0000 C CNN
	1    3300 1800
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY3
U 1 1 550937EE
P 3300 2450
F 0 "RLY3" H 3300 2700 60  0000 C CNN
F 1 "RELAY_34.51" H 3300 2215 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 3300 2450 60  0001 C CNN
F 3 "" H 3300 2450 60  0000 C CNN
	1    3300 2450
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY4
U 1 1 5509380D
P 3300 3100
F 0 "RLY4" H 3300 3350 60  0000 C CNN
F 1 "RELAY_34.51" H 3300 2865 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 3300 3100 60  0001 C CNN
F 3 "" H 3300 3100 60  0000 C CNN
	1    3300 3100
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY5
U 1 1 5509382A
P 3300 3750
F 0 "RLY5" H 3300 4000 60  0000 C CNN
F 1 "RELAY_34.51" H 3300 3515 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 3300 3750 60  0001 C CNN
F 3 "" H 3300 3750 60  0000 C CNN
	1    3300 3750
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY6
U 1 1 55093843
P 3300 4400
F 0 "RLY6" H 3300 4650 60  0000 C CNN
F 1 "RELAY_34.51" H 3300 4165 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 3300 4400 60  0001 C CNN
F 3 "" H 3300 4400 60  0000 C CNN
	1    3300 4400
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR075
U 1 1 54E5645F
P 2050 800
F 0 "#PWR075" H 2050 650 60  0001 C CNN
F 1 "+24V" H 2050 940 60  0000 C CNN
F 2 "" H 2050 800 60  0000 C CNN
F 3 "" H 2050 800 60  0000 C CNN
	1    2050 800 
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY7
U 1 1 5509511A
P 3300 5050
F 0 "RLY7" H 3300 5300 60  0000 C CNN
F 1 "RELAY_34.51" H 3300 4815 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 3300 5050 60  0001 C CNN
F 3 "" H 3300 5050 60  0000 C CNN
	1    3300 5050
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY8
U 1 1 55095642
P 3300 5700
F 0 "RLY8" H 3300 5950 60  0000 C CNN
F 1 "RELAY_34.51" H 3300 5465 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 3300 5700 60  0001 C CNN
F 3 "" H 3300 5700 60  0000 C CNN
	1    3300 5700
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X02 P2
U 1 1 55095A25
P 4100 5250
F 0 "P2" H 4100 5400 50  0000 C CNN
F 1 "CONN_01X02" V 4200 5250 50  0000 C CNN
F 2 "kl_footprints:conn_2P" H 4100 5250 60  0001 C CNN
F 3 "" H 4100 5250 60  0000 C CNN
	1    4100 5250
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY9
U 1 1 55097564
P 9450 2500
F 0 "RLY9" H 9450 2750 60  0000 C CNN
F 1 "RELAY_34.51" H 9450 2265 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 9450 2500 60  0001 C CNN
F 3 "" H 9450 2500 60  0000 C CNN
	1    9450 2500
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY10
U 1 1 550975B5
P 9450 3150
F 0 "RLY10" H 9450 3400 60  0000 C CNN
F 1 "RELAY_34.51" H 9450 2915 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 9450 3150 60  0001 C CNN
F 3 "" H 9450 3150 60  0000 C CNN
	1    9450 3150
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY11
U 1 1 550975DA
P 9450 3800
F 0 "RLY11" H 9450 4050 60  0000 C CNN
F 1 "RELAY_34.51" H 9450 3565 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 9450 3800 60  0001 C CNN
F 3 "" H 9450 3800 60  0000 C CNN
	1    9450 3800
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY12
U 1 1 550975FD
P 9450 4450
F 0 "RLY12" H 9450 4700 60  0000 C CNN
F 1 "RELAY_34.51" H 9450 4215 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 9450 4450 60  0001 C CNN
F 3 "" H 9450 4450 60  0000 C CNN
	1    9450 4450
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY13
U 1 1 550976F5
P 9450 5100
F 0 "RLY13" H 9450 5350 60  0000 C CNN
F 1 "RELAY_34.51" H 9450 4865 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 9450 5100 60  0001 C CNN
F 3 "" H 9450 5100 60  0000 C CNN
	1    9450 5100
	1    0    0    -1  
$EndComp
$Comp
L RELAY_34.51 RLY14
U 1 1 550977A2
P 9450 5750
F 0 "RLY14" H 9450 6000 60  0000 C CNN
F 1 "RELAY_34.51" H 9450 5515 60  0000 C CNN
F 2 "w_relay:relay_34.51" H 9450 5750 60  0001 C CNN
F 3 "" H 9450 5750 60  0000 C CNN
	1    9450 5750
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X04 P10
U 1 1 5509E05E
P 8050 4250
F 0 "P10" H 8050 4500 50  0000 C CNN
F 1 "CONN_02X04" H 8050 4000 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_2x04" H 8050 3050 60  0001 C CNN
F 3 "" H 8050 3050 60  0000 C CNN
	1    8050 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	2050 2100 2050 2250
Wire Wire Line
	7400 800  7400 2200
Wire Wire Line
	7400 3200 7400 3600
Wire Wire Line
	2450 1850 2500 1850
Wire Wire Line
	2500 1850 2500 4900
Wire Wire Line
	2550 1750 2450 1750
Wire Wire Line
	2450 1650 3100 1650
Wire Wire Line
	2800 1550 2450 1550
Wire Wire Line
	3100 2300 2700 2300
Wire Wire Line
	2700 2300 2700 1450
Wire Wire Line
	2700 1450 2450 1450
Wire Wire Line
	8800 2550 7800 2550
Wire Wire Line
	8850 2450 7800 2450
Wire Wire Line
	6750 2650 7000 2650
Wire Wire Line
	6750 2750 7000 2750
Wire Wire Line
	6750 2350 7000 2350
Wire Wire Line
	6750 2450 7000 2450
Wire Wire Line
	6750 2550 7000 2550
Wire Wire Line
	6750 2850 7000 2850
Wire Wire Line
	6750 2950 7000 2950
Wire Wire Line
	6750 3050 7000 3050
Wire Wire Line
	1550 1750 1650 1750
Wire Wire Line
	1650 1250 1350 1250
Wire Wire Line
	1650 1350 1350 1350
Wire Wire Line
	1650 1550 1350 1550
Wire Wire Line
	1650 1650 1350 1650
Wire Wire Line
	1650 1850 1350 1850
Wire Wire Line
	1650 1950 1350 1950
Wire Wire Line
	1350 1450 1650 1450
Wire Wire Line
	2550 1000 3100 1000
Wire Wire Line
	2450 1250 2900 1250
Connection ~ 7400 3500
Wire Wire Line
	2500 4900 3100 4900
Wire Wire Line
	3500 1000 3600 1000
Wire Wire Line
	3600 800  3600 5550
Wire Wire Line
	3600 1650 3500 1650
Wire Wire Line
	3600 2300 3500 2300
Wire Wire Line
	3600 2950 3500 2950
Connection ~ 3600 2300
Wire Wire Line
	3600 3600 3500 3600
Connection ~ 3600 2950
Wire Wire Line
	2450 1950 2450 5550
Wire Wire Line
	2450 5550 3100 5550
Wire Wire Line
	3600 4250 3500 4250
Connection ~ 3600 3600
Wire Wire Line
	3600 4900 3500 4900
Connection ~ 3600 4250
Wire Wire Line
	3600 5550 3500 5550
Connection ~ 3600 4900
Wire Wire Line
	3500 1300 3900 1300
Wire Wire Line
	3500 1950 3900 1950
Wire Wire Line
	3900 1950 3900 1400
Wire Wire Line
	3500 2600 3900 2600
Wire Wire Line
	3500 3250 3900 3250
Wire Wire Line
	3900 3250 3900 2700
Wire Wire Line
	3500 3900 3900 3900
Wire Wire Line
	3500 4550 3900 4550
Wire Wire Line
	3900 4550 3900 4000
Wire Wire Line
	3500 5200 3900 5200
Wire Wire Line
	3500 5850 3900 5850
Wire Wire Line
	3900 5850 3900 5300
Wire Wire Line
	3100 1250 3050 1250
Wire Wire Line
	3050 1250 3050 6100
Wire Wire Line
	3050 1900 3100 1900
Wire Wire Line
	3050 2550 3100 2550
Connection ~ 3050 1900
Wire Wire Line
	3050 3200 3100 3200
Connection ~ 3050 2550
Wire Wire Line
	3050 3850 3100 3850
Connection ~ 3050 3200
Wire Wire Line
	3050 4500 3100 4500
Connection ~ 3050 3850
Wire Wire Line
	3050 5150 3100 5150
Connection ~ 3050 4500
Wire Wire Line
	3050 5800 3100 5800
Connection ~ 3050 5150
Connection ~ 3600 1650
Wire Wire Line
	2050 800  9700 800 
Connection ~ 3600 1000
Wire Wire Line
	2050 800  2050 1100
Connection ~ 3600 800 
Wire Wire Line
	9700 800  9700 5600
Wire Wire Line
	9700 2350 9650 2350
Connection ~ 7400 800 
Wire Wire Line
	9700 3000 9650 3000
Connection ~ 9700 2350
Wire Wire Line
	9700 3650 9650 3650
Connection ~ 9700 3000
Wire Wire Line
	9700 4300 9650 4300
Connection ~ 9700 3650
Wire Wire Line
	9700 4950 9650 4950
Connection ~ 9700 4300
Wire Wire Line
	9700 5600 9650 5600
Connection ~ 9700 4950
Wire Wire Line
	9650 2650 9800 2650
Wire Wire Line
	9650 3300 9800 3300
Wire Wire Line
	9800 3300 9800 2750
Wire Wire Line
	9650 3950 9800 3950
Wire Wire Line
	9650 4600 9800 4600
Wire Wire Line
	9800 4600 9800 4050
Wire Wire Line
	9650 5250 9800 5250
Wire Wire Line
	9650 5900 9800 5900
Wire Wire Line
	9800 5900 9800 5350
Wire Wire Line
	9250 2600 9200 2600
Wire Wire Line
	9200 2600 9200 5850
Wire Wire Line
	9200 3250 9250 3250
Wire Wire Line
	9200 3900 9250 3900
Connection ~ 9200 3250
Wire Wire Line
	9200 4550 9250 4550
Connection ~ 9200 3900
Wire Wire Line
	9200 5200 9250 5200
Connection ~ 9200 4550
Wire Wire Line
	5650 5850 9250 5850
Connection ~ 9200 5200
Wire Wire Line
	5650 5850 5650 6100
Connection ~ 9200 5850
Wire Wire Line
	3050 6100 5550 6100
Connection ~ 3050 5800
Wire Wire Line
	3100 2950 2900 2950
Wire Wire Line
	2900 2950 2900 1250
Wire Wire Line
	3100 3600 2850 3600
Wire Wire Line
	2850 3600 2850 1350
Wire Wire Line
	2850 1350 2450 1350
Wire Wire Line
	2550 1750 2550 1000
Wire Wire Line
	2800 1550 2800 4250
Connection ~ 2800 4250
Wire Wire Line
	2800 4250 3100 4250
Wire Wire Line
	7800 3050 8950 3050
Wire Wire Line
	8950 3050 8950 2350
Wire Wire Line
	8950 2350 9250 2350
Wire Wire Line
	9050 3000 9250 3000
Wire Wire Line
	7800 2350 7800 2200
Wire Wire Line
	8750 5600 9250 5600
Wire Wire Line
	8850 4300 9250 4300
Wire Wire Line
	8800 2550 8800 4950
Wire Wire Line
	8800 4950 9250 4950
Wire Wire Line
	8850 2450 8850 4300
Wire Wire Line
	7800 2200 8900 2200
Wire Wire Line
	8900 2200 8900 3650
Wire Wire Line
	8900 3650 9250 3650
Wire Wire Line
	7300 4400 7800 4400
Wire Wire Line
	8300 4400 8700 4400
Wire Wire Line
	8300 4300 8700 4300
Wire Wire Line
	8300 4200 8700 4200
Wire Wire Line
	7800 4300 7400 4300
Wire Wire Line
	7800 2850 8750 2850
Wire Wire Line
	7800 2950 9050 2950
Wire Wire Line
	7400 3500 8300 3500
Wire Wire Line
	8300 3500 8300 4100
Wire Wire Line
	7800 4100 5900 4100
Wire Wire Line
	5900 4100 5900 800 
Connection ~ 5900 800 
$Comp
L +3V3 #PWR076
U 1 1 550D9AA9
P 7300 4400
F 0 "#PWR076" H 7300 4250 60  0001 C CNN
F 1 "+3V3" H 7300 4540 60  0000 C CNN
F 2 "" H 7300 4400 60  0000 C CNN
F 3 "" H 7300 4400 60  0000 C CNN
	1    7300 4400
	0    -1   -1   0   
$EndComp
Text Label 8400 4400 0    60   ~ 0
PB9
Text Label 8400 4300 0    60   ~ 0
PB8
Text Label 7450 4300 0    60   ~ 0
PB7
Text Label 8400 4200 0    60   ~ 0
PB6
Wire Wire Line
	9050 2950 9050 3000
Wire Wire Line
	8750 2850 8750 5600
$Comp
L CONN_01X02 P23
U 1 1 550DA84D
P 8050 2700
F 0 "P23" H 8050 2850 50  0000 C CNN
F 1 "CONN_01X02" V 8150 2700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 8050 2700 60  0001 C CNN
F 3 "" H 8050 2700 60  0000 C CNN
	1    8050 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 2650 7800 2650
Wire Wire Line
	7850 2750 7800 2750
Wire Wire Line
	7800 4200 7400 4200
Text Label 7450 4200 0    60   ~ 0
PB5
$EndSCHEMATC