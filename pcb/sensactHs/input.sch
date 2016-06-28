EESchema Schematic File Version 2
LIBS:sensactHs-rescue
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
LIBS:ESD_Protection
LIBS:sensact
LIBS:sensactHs-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 13
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
L CONN_01X05 INP4
U 1 1 56379561
P 3900 5700
F 0 "INP4" H 3900 6000 50  0000 C CNN
F 1 "ROTENC" V 4000 5700 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x05" H 3900 5700 60  0001 C CNN
F 3 "" H 3900 5700 60  0000 C CNN
	1    3900 5700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR3
U 1 1 56379B83
P 3700 6000
F 0 "#PWR3" H 3700 5750 50  0001 C CNN
F 1 "GND" H 3700 5850 50  0000 C CNN
F 2 "" H 3700 6000 60  0000 C CNN
F 3 "" H 3700 6000 60  0000 C CNN
	1    3700 6000
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR2
U 1 1 56379CAA
P 3500 6000
F 0 "#PWR2" H 3500 5850 50  0001 C CNN
F 1 "+3V3" H 3500 6140 50  0000 C CNN
F 2 "" H 3500 6000 60  0000 C CNN
F 3 "" H 3500 6000 60  0000 C CNN
	1    3500 6000
	-1   0    0    1   
$EndComp
Text Label 3300 5700 0    60   ~ 0
PC13
Text Label 3300 5600 0    60   ~ 0
PB7
Text Label 3300 5500 0    60   ~ 0
PB6
$Comp
L CONN_01X34 INP1
U 1 1 5638A145
P 800 2900
F 0 "INP1" H 800 4650 50  0000 C CNN
F 1 "PUSH_BUTTON_INPUT" V 900 2900 50  0000 C CNN
F 2 "w_conn_strip:vasch_strip_17x2" H 800 2900 60  0001 C CNN
F 3 "" H 800 2900 60  0000 C CNN
	1    800  2900
	-1   0    0    -1  
$EndComp
$Sheet
S 1450 1200 1050 400 
U 56389175
F0 "ESDProtection1" 60
F1 "ESDProtection.sch" 60
F2 "I1" I L 1450 1550 60 
F3 "I2" I L 1450 1250 60 
F4 "I3" I L 1450 1350 60 
F5 "I4" I L 1450 1450 60 
F6 "O1" O R 2500 1550 60 
F7 "O2" O R 2500 1350 60 
F8 "O3" O R 2500 1250 60 
F9 "O4" O R 2500 1450 60 
$EndSheet
$Sheet
S 2550 1600 1050 400 
U 5638C262
F0 "ESDProtection2" 60
F1 "ESDProtection.sch" 60
F2 "I1" I L 2550 1650 60 
F3 "I2" I L 2550 1950 60 
F4 "I3" I L 2550 1850 60 
F5 "I4" I L 2550 1750 60 
F6 "O1" O R 3600 1750 60 
F7 "O2" O R 3600 1950 60 
F8 "O3" O R 3600 1850 60 
F9 "O4" O R 3600 1650 60 
$EndSheet
$Sheet
S 1450 2000 1050 400 
U 5638D830
F0 "ESDProtection3" 60
F1 "ESDProtection.sch" 60
F2 "I1" I L 1450 2350 60 
F3 "I2" I L 1450 2050 60 
F4 "I3" I L 1450 2150 60 
F5 "I4" I L 1450 2250 60 
F6 "O1" O R 2500 2250 60 
F7 "O2" O R 2500 2050 60 
F8 "O3" O R 2500 2150 60 
F9 "O4" O R 2500 2350 60 
$EndSheet
$Sheet
S 2550 2400 1050 400 
U 5638DF1B
F0 "ESDProtection4" 60
F1 "ESDProtection.sch" 60
F2 "I1" I L 2550 2450 60 
F3 "I2" I L 2550 2750 60 
F4 "I3" I L 2550 2650 60 
F5 "I4" I L 2550 2550 60 
F6 "O1" O R 3600 2450 60 
F7 "O2" O R 3600 2650 60 
F8 "O3" O R 3600 2750 60 
F9 "O4" O R 3600 2550 60 
$EndSheet
$Sheet
S 1450 2800 1050 400 
U 5638E82E
F0 "ESDProtection5" 60
F1 "ESDProtection.sch" 60
F2 "I1" I L 1450 3150 60 
F3 "I2" I L 1450 2850 60 
F4 "I3" I L 1450 2950 60 
F5 "I4" I L 1450 3050 60 
F6 "O1" O R 2500 3150 60 
F7 "O2" O R 2500 2950 60 
F8 "O3" O R 2500 2850 60 
F9 "O4" O R 2500 3050 60 
$EndSheet
$Sheet
S 2550 3200 1050 400 
U 5638F2A3
F0 "ESDProtection6" 60
F1 "ESDProtection.sch" 60
F2 "I1" I L 2550 3250 60 
F3 "I2" I L 2550 3550 60 
F4 "I3" I L 2550 3450 60 
F5 "I4" I L 2550 3350 60 
F6 "O1" O R 3600 3350 60 
F7 "O2" O R 3600 3550 60 
F8 "O3" O R 3600 3450 60 
F9 "O4" O R 3600 3250 60 
$EndSheet
$Sheet
S 1450 3600 1050 400 
U 5638FD0A
F0 "ESDProtection7" 60
F1 "ESDProtection.sch" 60
F2 "I1" I L 1450 3950 60 
F3 "I2" I L 1450 3650 60 
F4 "I3" I L 1450 3750 60 
F5 "I4" I L 1450 3850 60 
F6 "O1" O R 2500 3850 60 
F7 "O2" O R 2500 3650 60 
F8 "O3" O R 2500 3750 60 
F9 "O4" O R 2500 3950 60 
$EndSheet
$Sheet
S 2550 4000 1050 400 
U 56390DD9
F0 "ESDProtection8" 60
F1 "ESDProtection.sch" 60
F2 "I1" I L 2550 4050 60 
F3 "I2" I L 2550 4250 60 
F4 "I3" I L 2550 4350 60 
F5 "I4" I L 2550 4150 60 
F6 "O1" O R 3600 4050 60 
F7 "O2" O R 3600 4250 60 
F8 "O3" O R 3600 4350 60 
F9 "O4" O R 3600 4150 60 
$EndSheet
$Comp
L PCA9555 U18
U 1 1 56393F18
P 4650 3800
F 0 "U18" H 4400 4750 60  0000 C CNN
F 1 "PCA9555" H 4900 4750 60  0000 C CNN
F 2 "kl_footprints:TSSOP-24_4.4x7.8mm_Pitch0.65mm_handsolder" H 4650 3800 60  0001 C CNN
F 3 "" H 4650 3800 60  0000 C CNN
	1    4650 3800
	-1   0    0    1   
$EndComp
$Comp
L PCA9555 U19
U 1 1 56395398
P 4650 1900
F 0 "U19" H 4400 2850 60  0000 C CNN
F 1 "PCA9555" H 4900 2850 60  0000 C CNN
F 2 "kl_footprints:TSSOP-24_4.4x7.8mm_Pitch0.65mm_handsolder" H 4650 1900 60  0001 C CNN
F 3 "" H 4650 1900 60  0000 C CNN
	1    4650 1900
	-1   0    0    1   
$EndComp
Text HLabel 1350 4900 0    60   BiDi ~ 0
PA[0..15]
Text HLabel 1350 5050 0    60   BiDi ~ 0
PB[0..15]
Text HLabel 1350 5200 0    60   BiDi ~ 0
PC[0..15]
Text Label 1500 4900 0    60   ~ 0
PA[0..15]
Text Label 1500 5050 0    60   ~ 0
PB[0..15]
Text Label 1500 5200 0    60   ~ 0
PC[0..15]
Wire Wire Line
	3700 6000 3700 5900
Wire Wire Line
	3700 5800 3500 5800
Wire Wire Line
	3500 5800 3500 6000
Wire Wire Line
	3700 5700 3300 5700
Wire Wire Line
	3700 5600 3300 5600
Wire Wire Line
	3700 5500 3300 5500
Wire Bus Line
	1350 4900 1950 4900
Wire Bus Line
	1350 5050 1950 5050
Wire Bus Line
	1350 5200 1950 5200
Wire Wire Line
	1000 1350 1250 1350
Wire Wire Line
	1250 1350 1250 1250
Wire Wire Line
	1250 1250 1450 1250
Wire Wire Line
	1000 1550 1300 1550
Wire Wire Line
	1300 1550 1300 1350
Wire Wire Line
	1300 1350 1450 1350
Wire Wire Line
	1000 1750 1350 1750
Wire Wire Line
	1350 1750 1350 1450
Wire Wire Line
	1350 1450 1450 1450
Wire Wire Line
	1000 1950 1400 1950
Wire Wire Line
	1400 1950 1400 1550
Wire Wire Line
	1400 1550 1450 1550
Wire Wire Line
	1000 1250 1200 1250
Wire Wire Line
	1200 1250 1200 1650
Wire Wire Line
	1000 1450 1150 1450
Wire Wire Line
	1150 1450 1150 1700
Wire Wire Line
	1000 1650 1000 1800
Wire Wire Line
	1000 2750 1400 2750
Wire Wire Line
	1400 2750 1400 2350
Wire Wire Line
	1400 2350 1450 2350
Wire Wire Line
	1000 2550 1350 2550
Wire Wire Line
	1350 2550 1350 2250
Wire Wire Line
	1350 2250 1450 2250
Wire Wire Line
	1000 2350 1300 2350
Wire Wire Line
	1300 2350 1300 2150
Wire Wire Line
	1300 2150 1450 2150
Wire Wire Line
	1000 2150 1250 2150
Wire Wire Line
	1250 2150 1250 2050
Wire Wire Line
	1250 2050 1450 2050
Wire Wire Line
	1000 2050 1200 2050
Wire Wire Line
	1200 2050 1200 2450
Wire Wire Line
	1200 2450 2550 2450
Wire Wire Line
	1000 2250 1150 2250
Wire Wire Line
	1150 2250 1150 2500
Wire Wire Line
	1150 2500 2500 2500
Wire Wire Line
	2500 2500 2500 2550
Wire Wire Line
	2500 2550 2550 2550
Wire Wire Line
	1000 2450 1100 2450
Wire Wire Line
	1100 2450 1100 2600
Wire Wire Line
	1100 2600 2500 2600
Wire Wire Line
	2500 2600 2500 2650
Wire Wire Line
	2500 2650 2550 2650
Wire Wire Line
	1000 2650 2450 2650
Wire Wire Line
	2450 2650 2450 2750
Wire Wire Line
	2450 2750 2550 2750
Wire Wire Line
	1000 3550 1400 3550
Wire Wire Line
	1400 3550 1400 3150
Wire Wire Line
	1400 3150 1450 3150
Wire Wire Line
	1000 3350 1350 3350
Wire Wire Line
	1350 3350 1350 3050
Wire Wire Line
	1350 3050 1450 3050
Wire Wire Line
	1000 3150 1300 3150
Wire Wire Line
	1300 3150 1300 2950
Wire Wire Line
	1300 2950 1450 2950
Wire Wire Line
	1000 2950 1250 2950
Wire Wire Line
	1250 2950 1250 2850
Wire Wire Line
	1250 2850 1450 2850
Wire Wire Line
	1000 2850 1200 2850
Wire Wire Line
	1200 2850 1200 3300
Wire Wire Line
	1200 3300 2500 3300
Wire Wire Line
	2500 3300 2500 3250
Wire Wire Line
	2500 3250 2550 3250
Wire Wire Line
	1000 3050 1150 3050
Wire Wire Line
	1150 3050 1150 3400
Wire Wire Line
	1150 3400 2500 3400
Wire Wire Line
	2500 3400 2500 3350
Wire Wire Line
	2500 3350 2550 3350
Wire Wire Line
	1000 3250 1100 3250
Wire Wire Line
	1100 3250 1100 3450
Wire Wire Line
	1100 3450 2550 3450
Wire Wire Line
	1000 3450 1000 3500
Wire Wire Line
	1000 3500 2500 3500
Wire Wire Line
	2500 3500 2500 3550
Wire Wire Line
	2500 3550 2550 3550
Wire Wire Line
	1000 4350 1400 4350
Wire Wire Line
	1400 4350 1400 3950
Wire Wire Line
	1400 3950 1450 3950
Wire Wire Line
	1000 4150 1350 4150
Wire Wire Line
	1350 4150 1350 3850
Wire Wire Line
	1350 3850 1450 3850
Wire Wire Line
	1000 3950 1300 3950
Wire Wire Line
	1300 3950 1300 3750
Wire Wire Line
	1300 3750 1450 3750
Wire Wire Line
	1000 3750 1250 3750
Wire Wire Line
	1250 3750 1250 3650
Wire Wire Line
	1250 3650 1450 3650
Wire Wire Line
	1000 3650 1200 3650
Wire Wire Line
	1200 3650 1200 4050
Wire Wire Line
	1200 4050 2550 4050
Wire Wire Line
	1000 3850 1150 3850
Wire Wire Line
	1150 3850 1150 4100
Wire Wire Line
	1150 4100 2500 4100
Wire Wire Line
	2500 4100 2500 4150
Wire Wire Line
	2500 4150 2550 4150
Wire Wire Line
	1000 4050 1100 4050
Wire Wire Line
	1100 4050 1100 4300
Wire Wire Line
	1000 4250 2550 4250
Wire Wire Line
	1100 4300 2500 4300
Wire Wire Line
	2500 4300 2500 4350
Wire Wire Line
	2500 4350 2550 4350
Wire Wire Line
	1200 1650 2550 1650
Wire Wire Line
	1150 1700 2500 1700
Wire Wire Line
	2500 1700 2500 1750
Wire Wire Line
	2500 1750 2550 1750
Wire Wire Line
	1000 1800 2500 1800
Wire Wire Line
	2500 1800 2500 1850
Wire Wire Line
	2500 1850 2550 1850
Wire Wire Line
	1000 1850 2450 1850
Wire Wire Line
	2450 1850 2450 1950
Wire Wire Line
	2450 1950 2550 1950
Wire Wire Line
	2500 1250 2650 1250
Wire Wire Line
	2650 1250 2650 1100
Wire Wire Line
	2650 1100 4100 1100
Wire Wire Line
	3600 1650 3700 1650
Wire Wire Line
	3700 1650 3700 1200
Wire Wire Line
	3700 1200 4100 1200
Wire Wire Line
	3750 1750 3600 1750
Wire Wire Line
	3750 1300 3750 1750
Wire Wire Line
	3750 1300 4100 1300
Wire Wire Line
	4100 1400 2550 1400
Wire Wire Line
	2550 1400 2550 1350
Wire Wire Line
	2550 1350 2500 1350
Wire Wire Line
	2500 1450 4100 1450
Wire Wire Line
	4100 1450 4100 1500
Wire Wire Line
	3600 1850 3800 1850
Wire Wire Line
	3800 1850 3800 1600
Wire Wire Line
	3800 1600 4100 1600
Wire Wire Line
	3600 1950 3850 1950
Wire Wire Line
	3850 1950 3850 1700
Wire Wire Line
	3850 1700 4100 1700
Wire Wire Line
	2500 1550 4100 1550
Wire Wire Line
	4100 1550 4100 1800
Wire Wire Line
	2500 2050 4100 2050
Wire Wire Line
	4100 2050 4100 2000
Wire Wire Line
	3600 2450 3650 2450
Wire Wire Line
	3650 2450 3650 2100
Wire Wire Line
	3650 2100 4100 2100
Wire Wire Line
	3600 2550 3700 2550
Wire Wire Line
	3700 2550 3700 2200
Wire Wire Line
	3700 2200 4100 2200
Wire Wire Line
	2500 2150 4100 2150
Wire Wire Line
	4100 2150 4100 2300
Wire Wire Line
	2500 2250 4050 2250
Wire Wire Line
	4050 2250 4050 2400
Wire Wire Line
	4050 2400 4100 2400
Wire Wire Line
	3600 2650 3750 2650
Wire Wire Line
	3750 2650 3750 2500
Wire Wire Line
	3750 2500 4100 2500
Wire Wire Line
	3600 2750 3800 2750
Wire Wire Line
	3800 2750 3800 2600
Wire Wire Line
	3800 2600 4100 2600
Wire Wire Line
	2500 2350 4000 2350
Wire Wire Line
	4000 2350 4000 2700
Wire Wire Line
	4000 2700 4100 2700
Wire Wire Line
	2500 2850 4100 2850
Wire Wire Line
	4100 2850 4100 3000
Wire Wire Line
	3600 3250 3650 3250
Wire Wire Line
	3650 3250 3650 3100
Wire Wire Line
	3650 3100 4100 3100
Wire Wire Line
	3700 3200 4100 3200
Wire Wire Line
	3600 3350 3700 3350
Wire Wire Line
	3700 3350 3700 3200
Wire Wire Line
	2500 2950 4050 2950
Wire Wire Line
	4050 2950 4050 3300
Wire Wire Line
	4050 3300 4100 3300
Wire Wire Line
	2500 3050 4000 3050
Wire Wire Line
	4000 3050 4000 3400
Wire Wire Line
	4000 3400 4100 3400
Wire Wire Line
	3600 3450 4100 3450
Wire Wire Line
	4100 3450 4100 3500
Wire Wire Line
	3600 3550 4100 3550
Wire Wire Line
	4100 3550 4100 3600
Wire Wire Line
	2500 3150 3950 3150
Wire Wire Line
	3950 3150 3950 3700
Wire Wire Line
	3950 3700 4100 3700
Wire Wire Line
	2500 3650 3900 3650
Wire Wire Line
	3900 3650 3900 3900
Wire Wire Line
	3900 3900 4100 3900
Wire Wire Line
	3600 4050 3650 4050
Wire Wire Line
	3650 4050 3650 4000
Wire Wire Line
	3650 4000 4100 4000
Wire Wire Line
	3600 4150 3650 4150
Wire Wire Line
	3650 4150 3650 4100
Wire Wire Line
	3650 4100 4100 4100
Wire Wire Line
	2500 3750 3850 3750
Wire Wire Line
	3850 3750 3850 4200
Wire Wire Line
	3850 4200 4100 4200
Wire Wire Line
	2500 3850 3800 3850
Wire Wire Line
	3800 3850 3800 4300
Wire Wire Line
	3800 4300 4100 4300
Wire Wire Line
	3600 4250 3750 4250
Wire Wire Line
	3750 4250 3750 4400
Wire Wire Line
	3750 4400 4100 4400
Wire Wire Line
	3600 4350 3700 4350
Wire Wire Line
	3700 4350 3700 4500
Wire Wire Line
	3700 4500 4100 4500
Wire Wire Line
	2500 3950 4000 3950
Wire Wire Line
	4000 3950 4000 4600
Wire Wire Line
	4000 4600 4100 4600
Text GLabel 5200 4500 2    60   BiDi ~ 0
SDA1
Text GLabel 5200 4600 2    60   BiDi ~ 0
SCL1
Text GLabel 5200 2600 2    60   BiDi ~ 0
SDA1
Text GLabel 5200 2700 2    60   BiDi ~ 0
SCL1
Wire Wire Line
	5200 2700 5100 2700
Wire Wire Line
	5200 2600 5100 2600
Wire Wire Line
	5200 4500 5100 4500
Wire Wire Line
	5200 4600 5100 4600
$Comp
L +3V3 #PWR5
U 1 1 563C51BD
P 5100 1350
F 0 "#PWR5" H 5100 1200 50  0001 C CNN
F 1 "+3V3" H 5100 1490 50  0000 C CNN
F 2 "" H 5100 1350 60  0000 C CNN
F 3 "" H 5100 1350 60  0000 C CNN
	1    5100 1350
	-1   0    0    1   
$EndComp
$Comp
L +3V3 #PWR7
U 1 1 563C54E9
P 5100 3250
F 0 "#PWR7" H 5100 3100 50  0001 C CNN
F 1 "+3V3" H 5100 3390 50  0000 C CNN
F 2 "" H 5100 3250 60  0000 C CNN
F 3 "" H 5100 3250 60  0000 C CNN
	1    5100 3250
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR4
U 1 1 563C6475
P 5100 1150
F 0 "#PWR4" H 5100 900 50  0001 C CNN
F 1 "GND" H 5100 1000 50  0000 C CNN
F 2 "" H 5100 1150 60  0000 C CNN
F 3 "" H 5100 1150 60  0000 C CNN
	1    5100 1150
	-1   0    0    1   
$EndComp
$Comp
L GND #PWR6
U 1 1 563C6873
P 5100 3050
F 0 "#PWR6" H 5100 2800 50  0001 C CNN
F 1 "GND" H 5100 2900 50  0000 C CNN
F 2 "" H 5100 3050 60  0000 C CNN
F 3 "" H 5100 3050 60  0000 C CNN
	1    5100 3050
	-1   0    0    1   
$EndComp
Wire Wire Line
	5100 1200 5100 1150
Wire Wire Line
	5100 1300 5100 1350
Wire Wire Line
	5100 3250 5100 3200
Wire Wire Line
	5100 3100 5100 3050
Wire Wire Line
	5100 2500 5500 2500
Text Label 5200 2500 2    60   ~ 0
PC1
Wire Wire Line
	5100 4400 5500 4400
Text Label 5250 4400 2    60   ~ 0
PC0
$Comp
L GND #PWR8
U 1 1 563D6207
P 5150 3900
F 0 "#PWR8" H 5150 3650 50  0001 C CNN
F 1 "GND" H 5150 3750 50  0000 C CNN
F 2 "" H 5150 3900 60  0000 C CNN
F 3 "" H 5150 3900 60  0000 C CNN
	1    5150 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	5150 3900 5100 3900
Wire Wire Line
	5100 3800 5100 3700
Wire Wire Line
	5100 3700 5200 3700
Wire Wire Line
	5200 3700 5200 3200
Wire Wire Line
	5200 3200 5100 3200
Wire Wire Line
	5100 1800 5100 2000
Connection ~ 5100 1900
Wire Wire Line
	5100 1800 5250 1800
Wire Wire Line
	5250 1800 5250 1300
Wire Wire Line
	5250 1300 5100 1300
$Comp
L CONN_01X14 INP2
U 1 1 563DE443
P 8050 2650
F 0 "INP2" H 8050 3400 50  0000 C CNN
F 1 "CONN_01X14" V 8150 2650 50  0000 C CNN
F 2 "w_conn_strip:vasch_strip_7x2" H 8050 2650 60  0001 C CNN
F 3 "" H 8050 2650 60  0000 C CNN
	1    8050 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 2000 7000 2000
Wire Wire Line
	7850 2100 7000 2100
Wire Wire Line
	7850 2200 7000 2200
Wire Wire Line
	7850 2300 7000 2300
Wire Wire Line
	7850 2400 7000 2400
Wire Wire Line
	7850 2500 7000 2500
Wire Wire Line
	7850 2600 7000 2600
Wire Wire Line
	7850 2700 7000 2700
Wire Wire Line
	7850 2800 7000 2800
Wire Wire Line
	7850 2900 7000 2900
Wire Wire Line
	7850 3000 7000 3000
Wire Wire Line
	7850 3100 7000 3100
Wire Wire Line
	7850 3200 7000 3200
Text Label 7000 2000 0    60   ~ 0
PC2
Text Label 7000 2100 0    60   ~ 0
PC3
Text Label 7000 2200 0    60   ~ 0
PA0
Text Label 7000 2300 0    60   ~ 0
PA1
Text Label 7000 2400 0    60   ~ 0
PA2
Text Label 7000 2500 0    60   ~ 0
PA3
Text Label 7000 2600 0    60   ~ 0
PA4
Text Label 7000 2700 0    60   ~ 0
PA5
Text Label 7000 2800 0    60   ~ 0
PA6
Text Label 7000 2900 0    60   ~ 0
PA7
Text Label 7000 3000 0    60   ~ 0
PC4
Text Label 7000 3100 0    60   ~ 0
PB1
Text Label 7000 3200 0    60   ~ 0
PB0
$Comp
L GND #PWR9
U 1 1 563E0FE5
P 7800 3300
F 0 "#PWR9" H 7800 3050 50  0001 C CNN
F 1 "GND" H 7800 3150 50  0000 C CNN
F 2 "" H 7800 3300 60  0000 C CNN
F 3 "" H 7800 3300 60  0000 C CNN
	1    7800 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7850 3300 7800 3300
$Comp
L GND #PWR1
U 1 1 563E97EB
P 1050 4600
F 0 "#PWR1" H 1050 4350 50  0001 C CNN
F 1 "GND" H 1050 4450 50  0000 C CNN
F 2 "" H 1050 4600 60  0000 C CNN
F 3 "" H 1050 4600 60  0000 C CNN
	1    1050 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 4450 1050 4450
Wire Wire Line
	1050 4450 1050 4600
Wire Wire Line
	1000 4550 1050 4550
Connection ~ 1050 4550
$EndSCHEMATC