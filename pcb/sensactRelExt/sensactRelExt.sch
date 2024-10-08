EESchema Schematic File Version 2
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
LIBS:sensact
LIBS:sensactRelExt-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Comp
L CONN_02X16 P2
U 1 1 57C5D536
P 10150 2400
F 0 "P2" H 10150 3250 50  0000 C CNN
F 1 "CONN_02X16" V 10150 2400 50  0000 C CNN
F 2 "w_conn_strip:vasch_strip_16x2" H 10150 1300 50  0001 C CNN
F 3 "" H 10150 1300 50  0000 C CNN
	1    10150 2400
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR01
U 1 1 57C5D6BA
P 10400 1300
F 0 "#PWR01" H 10400 1150 50  0001 C CNN
F 1 "+24V" H 10400 1440 50  0000 C CNN
F 2 "" H 10400 1300 50  0000 C CNN
F 3 "" H 10400 1300 50  0000 C CNN
	1    10400 1300
	1    0    0    -1  
$EndComp
$Comp
L CONN_02X05 P1
U 1 1 58474D6D
P 1250 1800
F 0 "P1" H 1250 2100 50  0000 C CNN
F 1 "CONN_02X05" H 1250 1500 50  0000 C CNN
F 2 "w_conn_strip:vasch_strip_5x2" H 1250 600 50  0001 C CNN
F 3 "" H 1250 600 50  0000 C CNN
	1    1250 1800
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 58474DA2
P 1550 2000
F 0 "#PWR02" H 1550 1750 50  0001 C CNN
F 1 "GND" H 1550 1850 50  0000 C CNN
F 2 "" H 1550 2000 50  0000 C CNN
F 3 "" H 1550 2000 50  0000 C CNN
	1    1550 2000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR03
U 1 1 58474E01
P 950 2000
F 0 "#PWR03" H 950 1750 50  0001 C CNN
F 1 "GND" H 950 1850 50  0000 C CNN
F 2 "" H 950 2000 50  0000 C CNN
F 3 "" H 950 2000 50  0000 C CNN
	1    950  2000
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR04
U 1 1 58474E29
P 1550 1900
F 0 "#PWR04" H 1550 1750 50  0001 C CNN
F 1 "+24V" H 1550 2040 50  0000 C CNN
F 2 "" H 1550 1900 50  0000 C CNN
F 3 "" H 1550 1900 50  0000 C CNN
	1    1550 1900
	0    1    1    0   
$EndComp
$Comp
L +24V #PWR05
U 1 1 58474E53
P 950 1900
F 0 "#PWR05" H 950 1750 50  0001 C CNN
F 1 "+24V" H 950 2040 50  0000 C CNN
F 2 "" H 950 1900 50  0000 C CNN
F 3 "" H 950 1900 50  0000 C CNN
	1    950  1900
	0    -1   -1   0   
$EndComp
$Comp
L +3V3 #PWR06
U 1 1 58474E7D
P 950 1600
F 0 "#PWR06" H 950 1450 50  0001 C CNN
F 1 "+3V3" H 950 1740 50  0000 C CNN
F 2 "" H 950 1600 50  0000 C CNN
F 3 "" H 950 1600 50  0000 C CNN
	1    950  1600
	1    0    0    -1  
$EndComp
NoConn ~ 1500 1700
NoConn ~ 1500 1800
NoConn ~ 1000 1800
Text Label 1550 1600 0    60   ~ 0
SCL
Text Label 800  1700 0    60   ~ 0
SDA
$Comp
L +3V3 #PWR07
U 1 1 58475789
P 3700 6150
F 0 "#PWR07" H 3700 6000 50  0001 C CNN
F 1 "+3V3" H 3700 6290 50  0000 C CNN
F 2 "" H 3700 6150 50  0000 C CNN
F 3 "" H 3700 6150 50  0000 C CNN
	1    3700 6150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR08
U 1 1 584757B3
P 2050 7450
F 0 "#PWR08" H 2050 7200 50  0001 C CNN
F 1 "GND" H 2050 7300 50  0000 C CNN
F 2 "" H 2050 7450 50  0000 C CNN
F 3 "" H 2050 7450 50  0000 C CNN
	1    2050 7450
	1    0    0    -1  
$EndComp
$Comp
L C C1
U 1 1 584758D0
P 1100 6400
F 0 "C1" H 1125 6500 50  0000 L CNN
F 1 "u1" H 1125 6300 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1138 6250 50  0001 C CNN
F 3 "" H 1100 6400 50  0000 C CNN
	1    1100 6400
	1    0    0    -1  
$EndComp
$Comp
L C C2
U 1 1 58475971
P 1300 6400
F 0 "C2" H 1325 6500 50  0000 L CNN
F 1 "10u" H 1325 6300 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 1338 6250 50  0001 C CNN
F 3 "" H 1300 6400 50  0000 C CNN
	1    1300 6400
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR09
U 1 1 584759A2
P 1200 6200
F 0 "#PWR09" H 1200 6050 50  0001 C CNN
F 1 "+3V3" H 1200 6340 50  0000 C CNN
F 2 "" H 1200 6200 50  0000 C CNN
F 3 "" H 1200 6200 50  0000 C CNN
	1    1200 6200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR010
U 1 1 584759D0
P 1200 6600
F 0 "#PWR010" H 1200 6350 50  0001 C CNN
F 1 "GND" H 1200 6450 50  0000 C CNN
F 2 "" H 1200 6600 50  0000 C CNN
F 3 "" H 1200 6600 50  0000 C CNN
	1    1200 6600
	1    0    0    -1  
$EndComp
$Comp
L PCA9685 U9
U 1 1 5847666B
P 2850 6800
F 0 "U9" H 2850 6800 60  0000 C CNN
F 1 "PCA9685" H 2850 7050 60  0000 C CNN
F 2 "sensact:TSSOP-28_4.4x9.7mm_Pitch0.65mm_handsolder" H 2850 6800 60  0001 C CNN
F 3 "" H 2850 6800 60  0000 C CNN
	1    2850 6800
	1    0    0    -1  
$EndComp
Text Label 3700 6250 0    60   ~ 0
SDA
Text Label 3700 6350 0    60   ~ 0
SCL
$Comp
L GND #PWR011
U 1 1 5847BC0D
P 4350 6650
F 0 "#PWR011" H 4350 6400 50  0001 C CNN
F 1 "GND" H 4350 6500 50  0000 C CNN
F 2 "" H 4350 6650 50  0000 C CNN
F 3 "" H 4350 6650 50  0000 C CNN
	1    4350 6650
	0    -1   -1   0   
$EndComp
$Comp
L JUMPER3 JP1
U 1 1 5847BCC3
P 10450 3900
F 0 "JP1" H 10500 3800 50  0000 L CNN
F 1 "A0" H 10450 4000 50  0000 C BNN
F 2 "sensact:SMD-solder-bridge-3pin" H 10450 3900 50  0001 C CNN
F 3 "" H 10450 3900 50  0000 C CNN
	1    10450 3900
	1    0    0    -1  
$EndComp
$Comp
L JUMPER3 JP2
U 1 1 5847BF30
P 10450 4300
F 0 "JP2" H 10500 4200 50  0000 L CNN
F 1 "A1" H 10450 4400 50  0000 C BNN
F 2 "sensact:SMD-solder-bridge-3pin" H 10450 4300 50  0001 C CNN
F 3 "" H 10450 4300 50  0000 C CNN
	1    10450 4300
	1    0    0    -1  
$EndComp
$Comp
L JUMPER3 JP3
U 1 1 5847BF69
P 10450 4700
F 0 "JP3" H 10500 4600 50  0000 L CNN
F 1 "A2" H 10450 4800 50  0000 C BNN
F 2 "sensact:SMD-solder-bridge-3pin" H 10450 4700 50  0001 C CNN
F 3 "" H 10450 4700 50  0000 C CNN
	1    10450 4700
	1    0    0    -1  
$EndComp
$Comp
L JUMPER3 JP4
U 1 1 5847BFA6
P 10450 5100
F 0 "JP4" H 10500 5000 50  0000 L CNN
F 1 "A3" H 10450 5200 50  0000 C BNN
F 2 "sensact:SMD-solder-bridge-3pin" H 10450 5100 50  0001 C CNN
F 3 "" H 10450 5100 50  0000 C CNN
	1    10450 5100
	1    0    0    -1  
$EndComp
$Comp
L JUMPER3 JP5
U 1 1 5847BFE7
P 10450 5500
F 0 "JP5" H 10500 5400 50  0000 L CNN
F 1 "A4" H 10450 5600 50  0000 C BNN
F 2 "sensact:SMD-solder-bridge-3pin" H 10450 5500 50  0001 C CNN
F 3 "" H 10450 5500 50  0000 C CNN
	1    10450 5500
	1    0    0    -1  
$EndComp
Text Label 10250 4000 0    60   ~ 0
A0
Text Label 10250 4400 0    60   ~ 0
A1
Text Label 10250 4800 0    60   ~ 0
A2
Text Label 10250 5200 0    60   ~ 0
A3
Text Label 10250 5600 0    60   ~ 0
A4
$Comp
L GND #PWR012
U 1 1 5847C8CA
P 10700 6000
F 0 "#PWR012" H 10700 5750 50  0001 C CNN
F 1 "GND" H 10700 5850 50  0000 C CNN
F 2 "" H 10700 6000 50  0000 C CNN
F 3 "" H 10700 6000 50  0000 C CNN
	1    10700 6000
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR013
U 1 1 5847C902
P 10200 3800
F 0 "#PWR013" H 10200 3650 50  0001 C CNN
F 1 "+3V3" H 10200 3940 50  0000 C CNN
F 2 "" H 10200 3800 50  0000 C CNN
F 3 "" H 10200 3800 50  0000 C CNN
	1    10200 3800
	1    0    0    -1  
$EndComp
Text Label 1800 6150 0    60   ~ 0
A0
Text Label 1800 6250 0    60   ~ 0
A1
Text Label 1800 6350 0    60   ~ 0
A2
Text Label 1800 6450 0    60   ~ 0
A3
Text Label 1800 6550 0    60   ~ 0
A4
$Comp
L GND #PWR014
U 1 1 5847D112
P 8350 3300
F 0 "#PWR014" H 8350 3050 50  0001 C CNN
F 1 "GND" H 8350 3150 50  0000 C CNN
F 2 "" H 8350 3300 50  0000 C CNN
F 3 "" H 8350 3300 50  0000 C CNN
	1    8350 3300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR015
U 1 1 5847D146
P 9350 3450
F 0 "#PWR015" H 9350 3200 50  0001 C CNN
F 1 "GND" H 9350 3300 50  0000 C CNN
F 2 "" H 9350 3450 50  0000 C CNN
F 3 "" H 9350 3450 50  0000 C CNN
	1    9350 3450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR016
U 1 1 5847D17A
P 9450 4050
F 0 "#PWR016" H 9450 3800 50  0001 C CNN
F 1 "GND" H 9450 3900 50  0000 C CNN
F 2 "" H 9450 4050 50  0000 C CNN
F 3 "" H 9450 4050 50  0000 C CNN
	1    9450 4050
	1    0    0    -1  
$EndComp
Text Label 1800 6650 0    60   ~ 0
LED0
Text Label 1800 6750 0    60   ~ 0
LED1
Text Label 1800 6850 0    60   ~ 0
LED2
Text Label 1800 6950 0    60   ~ 0
LED3
Text Label 1800 7050 0    60   ~ 0
LED4
Text Label 1800 7150 0    60   ~ 0
LED5
Text Label 1800 7250 0    60   ~ 0
LED6
Text Label 1800 7350 0    60   ~ 0
LED7
Text Label 3650 6750 0    60   ~ 0
LED15
Text Label 3650 6850 0    60   ~ 0
LED14
Text Label 3650 6950 0    60   ~ 0
LED13
Text Label 3650 7050 0    60   ~ 0
LED12
Text Label 3650 7150 0    60   ~ 0
LED11
Text Label 3650 7250 0    60   ~ 0
LED10
Text Label 3650 7350 0    60   ~ 0
LED9
Text Label 3650 7450 0    60   ~ 0
LED8
Text Label 1750 4300 0    60   ~ 0
LED0
Text Label 1750 4400 0    60   ~ 0
LED1
Text Label 1750 4500 0    60   ~ 0
LED2
Text Label 1750 4600 0    60   ~ 0
LED3
Text Label 1750 4700 0    60   ~ 0
LED4
Text Label 1750 4800 0    60   ~ 0
LED5
Text Label 1750 4900 0    60   ~ 0
LED6
Text Label 1750 5000 0    60   ~ 0
LED7
Text Label 1750 5200 0    60   ~ 0
LED8
Text Label 1750 5300 0    60   ~ 0
LED9
Text Label 1750 5400 0    60   ~ 0
LED10
Text Label 1750 5500 0    60   ~ 0
LED11
Text Label 1750 5600 0    60   ~ 0
LED12
Text Label 1750 5700 0    60   ~ 0
LED13
Text Label 1750 5800 0    60   ~ 0
LED14
Text Label 1750 5900 0    60   ~ 0
LED15
$Comp
L JUMPER3 JP6
U 1 1 584A6420
P 10450 5900
F 0 "JP6" H 10500 5800 50  0000 L CNN
F 1 "A5" H 10450 6000 50  0000 C BNN
F 2 "sensact:SMD-solder-bridge-3pin" H 10450 5900 50  0001 C CNN
F 3 "" H 10450 5900 50  0000 C CNN
	1    10450 5900
	1    0    0    -1  
$EndComp
Text Label 10250 6100 0    60   ~ 0
A5
Text Label 3800 6550 0    60   ~ 0
A5
$Comp
L Q_NMOS_GSD Q16
U 1 1 58555E5E
P 9500 3650
F 0 "Q16" H 9800 3700 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 10150 3600 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 9700 3750 50  0001 C CNN
F 3 "" H 9500 3650 50  0000 C CNN
	1    9500 3650
	1    0    0    -1  
$EndComp
$Comp
L R R32
U 1 1 585562A9
P 9300 3850
F 0 "R32" V 9380 3850 50  0000 C CNN
F 1 "10k" V 9300 3850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 9230 3850 50  0001 C CNN
F 3 "" H 9300 3850 50  0000 C CNN
	1    9300 3850
	1    0    0    -1  
$EndComp
$Comp
L R R31
U 1 1 5855632B
P 9150 3850
F 0 "R31" V 9230 3850 50  0000 C CNN
F 1 "1k" V 9150 3850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 9080 3850 50  0001 C CNN
F 3 "" H 9150 3850 50  0000 C CNN
	1    9150 3850
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q15
U 1 1 58556C2A
P 9400 3250
F 0 "Q15" H 9700 3300 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 10050 3200 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 9600 3350 50  0001 C CNN
F 3 "" H 9400 3250 50  0000 C CNN
	1    9400 3250
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q13
U 1 1 58556C79
P 8400 3050
F 0 "Q13" H 8700 3100 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 9050 3000 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 8600 3150 50  0001 C CNN
F 3 "" H 8400 3050 50  0000 C CNN
	1    8400 3050
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q14
U 1 1 58556CB4
P 8500 3450
F 0 "Q14" H 8800 3500 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 9150 3400 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 8700 3550 50  0001 C CNN
F 3 "" H 8500 3450 50  0000 C CNN
	1    8500 3450
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q11
U 1 1 58556CE7
P 7400 2850
F 0 "Q11" H 7700 2900 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 8050 2800 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 7600 2950 50  0001 C CNN
F 3 "" H 7400 2850 50  0000 C CNN
	1    7400 2850
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q12
U 1 1 58556D1C
P 7500 3250
F 0 "Q12" H 7800 3300 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 8150 3200 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 7700 3350 50  0001 C CNN
F 3 "" H 7500 3250 50  0000 C CNN
	1    7500 3250
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q9
U 1 1 58556D53
P 6400 2650
F 0 "Q9" H 6700 2700 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 7050 2600 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 6600 2750 50  0001 C CNN
F 3 "" H 6400 2650 50  0000 C CNN
	1    6400 2650
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q1
U 1 1 58556D8C
P 2400 1850
F 0 "Q1" H 2700 1900 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 3050 1800 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 2600 1950 50  0001 C CNN
F 3 "" H 2400 1850 50  0000 C CNN
	1    2400 1850
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q8
U 1 1 58556DC7
P 5500 2900
F 0 "Q8" H 5800 2950 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 6150 2850 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 5700 3000 50  0001 C CNN
F 3 "" H 5500 2900 50  0000 C CNN
	1    5500 2900
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q10
U 1 1 58556E0A
P 6500 3100
F 0 "Q10" H 6800 3150 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 7150 3050 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 6700 3200 50  0001 C CNN
F 3 "" H 6500 3100 50  0000 C CNN
	1    6500 3100
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q5
U 1 1 58556E53
P 4400 2250
F 0 "Q5" H 4700 2300 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 5050 2200 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 4600 2350 50  0001 C CNN
F 3 "" H 4400 2250 50  0000 C CNN
	1    4400 2250
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q7
U 1 1 58556EA0
P 5400 2450
F 0 "Q7" H 5700 2500 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 6050 2400 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 5600 2550 50  0001 C CNN
F 3 "" H 5400 2450 50  0000 C CNN
	1    5400 2450
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q6
U 1 1 58556EEF
P 4500 2700
F 0 "Q6" H 4800 2750 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 5150 2650 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 4700 2800 50  0001 C CNN
F 3 "" H 4500 2700 50  0000 C CNN
	1    4500 2700
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q3
U 1 1 58556F3E
P 3400 2050
F 0 "Q3" H 3700 2100 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 4050 2000 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 3600 2150 50  0001 C CNN
F 3 "" H 3400 2050 50  0000 C CNN
	1    3400 2050
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q4
U 1 1 58556F85
P 3500 2500
F 0 "Q4" H 3800 2550 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 4150 2450 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 3700 2600 50  0001 C CNN
F 3 "" H 3500 2500 50  0000 C CNN
	1    3500 2500
	1    0    0    -1  
$EndComp
$Comp
L Q_NMOS_GSD Q2
U 1 1 58556FCE
P 2500 2300
F 0 "Q2" H 2800 2350 50  0000 R CNN
F 1 "Q_NMOS_GSD" H 3150 2250 50  0000 R CNN
F 2 "TO_SOT_Packages_SMD:SOT-23_Handsoldering" H 2700 2400 50  0001 C CNN
F 3 "" H 2500 2300 50  0000 C CNN
	1    2500 2300
	1    0    0    -1  
$EndComp
$Comp
L R R30
U 1 1 585574E4
P 9150 3400
F 0 "R30" V 9230 3400 50  0000 C CNN
F 1 "10k" V 9150 3400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 9080 3400 50  0001 C CNN
F 3 "" H 9150 3400 50  0000 C CNN
	1    9150 3400
	1    0    0    -1  
$EndComp
$Comp
L R R29
U 1 1 5855753B
P 9000 3400
F 0 "R29" V 9080 3400 50  0000 C CNN
F 1 "1k" V 9000 3400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8930 3400 50  0001 C CNN
F 3 "" H 9000 3400 50  0000 C CNN
	1    9000 3400
	1    0    0    -1  
$EndComp
$Comp
L R R27
U 1 1 5855758A
P 8150 3200
F 0 "R27" V 8230 3200 50  0000 C CNN
F 1 "10k" V 8150 3200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8080 3200 50  0001 C CNN
F 3 "" H 8150 3200 50  0000 C CNN
	1    8150 3200
	1    0    0    -1  
$EndComp
$Comp
L R R25
U 1 1 585575E5
P 8000 3200
F 0 "R25" V 8080 3200 50  0000 C CNN
F 1 "1k" V 8000 3200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 7930 3200 50  0001 C CNN
F 3 "" H 8000 3200 50  0000 C CNN
	1    8000 3200
	1    0    0    -1  
$EndComp
$Comp
L R R28
U 1 1 5855763C
P 8250 3600
F 0 "R28" V 8330 3600 50  0000 C CNN
F 1 "10k" V 8250 3600 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8180 3600 50  0001 C CNN
F 3 "" H 8250 3600 50  0000 C CNN
	1    8250 3600
	1    0    0    -1  
$EndComp
$Comp
L R R26
U 1 1 58557695
P 8100 3600
F 0 "R26" V 8180 3600 50  0000 C CNN
F 1 "1k" V 8100 3600 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8030 3600 50  0001 C CNN
F 3 "" H 8100 3600 50  0000 C CNN
	1    8100 3600
	1    0    0    -1  
$EndComp
$Comp
L R R15
U 1 1 585576F0
P 5150 2600
F 0 "R15" V 5230 2600 50  0000 C CNN
F 1 "10k" V 5150 2600 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5080 2600 50  0001 C CNN
F 3 "" H 5150 2600 50  0000 C CNN
	1    5150 2600
	1    0    0    -1  
$EndComp
$Comp
L R R17
U 1 1 5855774B
P 6000 2800
F 0 "R17" V 6080 2800 50  0000 C CNN
F 1 "1k" V 6000 2800 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5930 2800 50  0001 C CNN
F 3 "" H 6000 2800 50  0000 C CNN
	1    6000 2800
	1    0    0    -1  
$EndComp
$Comp
L R R21
U 1 1 585577A6
P 7000 3000
F 0 "R21" V 7080 3000 50  0000 C CNN
F 1 "1k" V 7000 3000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 6930 3000 50  0001 C CNN
F 3 "" H 7000 3000 50  0000 C CNN
	1    7000 3000
	1    0    0    -1  
$EndComp
$Comp
L R R22
U 1 1 58557803
P 7100 3400
F 0 "R22" V 7180 3400 50  0000 C CNN
F 1 "1k" V 7100 3400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 7030 3400 50  0001 C CNN
F 3 "" H 7100 3400 50  0000 C CNN
	1    7100 3400
	1    0    0    -1  
$EndComp
$Comp
L R R24
U 1 1 58557862
P 7250 3400
F 0 "R24" V 7330 3400 50  0000 C CNN
F 1 "10k" V 7250 3400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 7180 3400 50  0001 C CNN
F 3 "" H 7250 3400 50  0000 C CNN
	1    7250 3400
	1    0    0    -1  
$EndComp
$Comp
L R R9
U 1 1 585578C3
P 4000 2400
F 0 "R9" V 4080 2400 50  0000 C CNN
F 1 "1k" V 4000 2400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3930 2400 50  0001 C CNN
F 3 "" H 4000 2400 50  0000 C CNN
	1    4000 2400
	1    0    0    -1  
$EndComp
$Comp
L R R11
U 1 1 5855792C
P 4150 2400
F 0 "R11" V 4230 2400 50  0000 C CNN
F 1 "10k" V 4150 2400 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4080 2400 50  0001 C CNN
F 3 "" H 4150 2400 50  0000 C CNN
	1    4150 2400
	1    0    0    -1  
$EndComp
$Comp
L R R10
U 1 1 58557991
P 4100 2850
F 0 "R10" V 4180 2850 50  0000 C CNN
F 1 "1k" V 4100 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4030 2850 50  0001 C CNN
F 3 "" H 4100 2850 50  0000 C CNN
	1    4100 2850
	1    0    0    -1  
$EndComp
$Comp
L R R12
U 1 1 585579F8
P 4250 2850
F 0 "R12" V 4330 2850 50  0000 C CNN
F 1 "10k" V 4250 2850 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4180 2850 50  0001 C CNN
F 3 "" H 4250 2850 50  0000 C CNN
	1    4250 2850
	1    0    0    -1  
$EndComp
$Comp
L R R14
U 1 1 58557A61
P 5100 3050
F 0 "R14" V 5180 3050 50  0000 C CNN
F 1 "1k" V 5100 3050 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5030 3050 50  0001 C CNN
F 3 "" H 5100 3050 50  0000 C CNN
	1    5100 3050
	1    0    0    -1  
$EndComp
$Comp
L R R16
U 1 1 58557ACC
P 5250 3050
F 0 "R16" V 5330 3050 50  0000 C CNN
F 1 "10k" V 5250 3050 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 5180 3050 50  0001 C CNN
F 3 "" H 5250 3050 50  0000 C CNN
	1    5250 3050
	1    0    0    -1  
$EndComp
$Comp
L R R13
U 1 1 58557B39
P 5000 2600
F 0 "R13" V 5080 2600 50  0000 C CNN
F 1 "1k" V 5000 2600 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4930 2600 50  0001 C CNN
F 3 "" H 5000 2600 50  0000 C CNN
	1    5000 2600
	1    0    0    -1  
$EndComp
$Comp
L R R18
U 1 1 58557BA8
P 6100 3250
F 0 "R18" V 6180 3250 50  0000 C CNN
F 1 "1k" V 6100 3250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 6030 3250 50  0001 C CNN
F 3 "" H 6100 3250 50  0000 C CNN
	1    6100 3250
	1    0    0    -1  
$EndComp
$Comp
L R R20
U 1 1 58557C19
P 6250 3250
F 0 "R20" V 6330 3250 50  0000 C CNN
F 1 "10k" V 6250 3250 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 6180 3250 50  0001 C CNN
F 3 "" H 6250 3250 50  0000 C CNN
	1    6250 3250
	1    0    0    -1  
$EndComp
$Comp
L R R19
U 1 1 58557C8C
P 6150 2800
F 0 "R19" V 6230 2800 50  0000 C CNN
F 1 "10k" V 6150 2800 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 6080 2800 50  0001 C CNN
F 3 "" H 6150 2800 50  0000 C CNN
	1    6150 2800
	1    0    0    -1  
$EndComp
$Comp
L R R23
U 1 1 58557D01
P 7150 3000
F 0 "R23" V 7230 3000 50  0000 C CNN
F 1 "10k" V 7150 3000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 7080 3000 50  0001 C CNN
F 3 "" H 7150 3000 50  0000 C CNN
	1    7150 3000
	1    0    0    -1  
$EndComp
$Comp
L R R7
U 1 1 5855D93B
P 3150 2200
F 0 "R7" V 3230 2200 50  0000 C CNN
F 1 "10k" V 3150 2200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3080 2200 50  0001 C CNN
F 3 "" H 3150 2200 50  0000 C CNN
	1    3150 2200
	1    0    0    -1  
$EndComp
$Comp
L R R5
U 1 1 5855D9BC
P 3000 2200
F 0 "R5" V 3080 2200 50  0000 C CNN
F 1 "1k" V 3000 2200 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2930 2200 50  0001 C CNN
F 3 "" H 3000 2200 50  0000 C CNN
	1    3000 2200
	1    0    0    -1  
$EndComp
$Comp
L R R8
U 1 1 5855DA41
P 3250 2650
F 0 "R8" V 3330 2650 50  0000 C CNN
F 1 "10k" V 3250 2650 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3180 2650 50  0001 C CNN
F 3 "" H 3250 2650 50  0000 C CNN
	1    3250 2650
	1    0    0    -1  
$EndComp
$Comp
L R R6
U 1 1 5855DB88
P 3100 2650
F 0 "R6" V 3180 2650 50  0000 C CNN
F 1 "1k" V 3100 2650 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3030 2650 50  0001 C CNN
F 3 "" H 3100 2650 50  0000 C CNN
	1    3100 2650
	1    0    0    -1  
$EndComp
$Comp
L R R4
U 1 1 5855DC09
P 2250 2450
F 0 "R4" V 2330 2450 50  0000 C CNN
F 1 "10k" V 2250 2450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2180 2450 50  0001 C CNN
F 3 "" H 2250 2450 50  0000 C CNN
	1    2250 2450
	1    0    0    -1  
$EndComp
$Comp
L R R2
U 1 1 5855DCA0
P 2100 2450
F 0 "R2" V 2180 2450 50  0000 C CNN
F 1 "1k" V 2100 2450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2030 2450 50  0001 C CNN
F 3 "" H 2100 2450 50  0000 C CNN
	1    2100 2450
	1    0    0    -1  
$EndComp
$Comp
L R R3
U 1 1 5855DD2D
P 2150 2000
F 0 "R3" V 2230 2000 50  0000 C CNN
F 1 "10k" V 2150 2000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2080 2000 50  0001 C CNN
F 3 "" H 2150 2000 50  0000 C CNN
	1    2150 2000
	1    0    0    -1  
$EndComp
$Comp
L R R1
U 1 1 5855DDC6
P 2000 2000
F 0 "R1" V 2080 2000 50  0000 C CNN
F 1 "1k" V 2000 2000 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 1930 2000 50  0001 C CNN
F 3 "" H 2000 2000 50  0000 C CNN
	1    2000 2000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR017
U 1 1 58562AC3
P 2350 2100
F 0 "#PWR017" H 2350 1850 50  0001 C CNN
F 1 "GND" H 2350 1950 50  0000 C CNN
F 2 "" H 2350 2100 50  0000 C CNN
F 3 "" H 2350 2100 50  0000 C CNN
	1    2350 2100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR018
U 1 1 58562B4B
P 2400 2550
F 0 "#PWR018" H 2400 2300 50  0001 C CNN
F 1 "GND" H 2400 2400 50  0000 C CNN
F 2 "" H 2400 2550 50  0000 C CNN
F 3 "" H 2400 2550 50  0000 C CNN
	1    2400 2550
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR019
U 1 1 58562BD1
P 3300 2300
F 0 "#PWR019" H 3300 2050 50  0001 C CNN
F 1 "GND" H 3300 2150 50  0000 C CNN
F 2 "" H 3300 2300 50  0000 C CNN
F 3 "" H 3300 2300 50  0000 C CNN
	1    3300 2300
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR020
U 1 1 58562C57
P 3450 2750
F 0 "#PWR020" H 3450 2500 50  0001 C CNN
F 1 "GND" H 3450 2600 50  0000 C CNN
F 2 "" H 3450 2750 50  0000 C CNN
F 3 "" H 3450 2750 50  0000 C CNN
	1    3450 2750
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR021
U 1 1 58562CDD
P 4350 2500
F 0 "#PWR021" H 4350 2250 50  0001 C CNN
F 1 "GND" H 4350 2350 50  0000 C CNN
F 2 "" H 4350 2500 50  0000 C CNN
F 3 "" H 4350 2500 50  0000 C CNN
	1    4350 2500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR022
U 1 1 58562D63
P 4450 2950
F 0 "#PWR022" H 4450 2700 50  0001 C CNN
F 1 "GND" H 4450 2800 50  0000 C CNN
F 2 "" H 4450 2950 50  0000 C CNN
F 3 "" H 4450 2950 50  0000 C CNN
	1    4450 2950
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR023
U 1 1 58563C33
P 5350 2700
F 0 "#PWR023" H 5350 2450 50  0001 C CNN
F 1 "GND" H 5350 2550 50  0000 C CNN
F 2 "" H 5350 2700 50  0000 C CNN
F 3 "" H 5350 2700 50  0000 C CNN
	1    5350 2700
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR024
U 1 1 585643DE
P 5450 3150
F 0 "#PWR024" H 5450 2900 50  0001 C CNN
F 1 "GND" H 5450 3000 50  0000 C CNN
F 2 "" H 5450 3150 50  0000 C CNN
F 3 "" H 5450 3150 50  0000 C CNN
	1    5450 3150
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR025
U 1 1 58564464
P 6350 2900
F 0 "#PWR025" H 6350 2650 50  0001 C CNN
F 1 "GND" H 6350 2750 50  0000 C CNN
F 2 "" H 6350 2900 50  0000 C CNN
F 3 "" H 6350 2900 50  0000 C CNN
	1    6350 2900
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR026
U 1 1 585644EA
P 6450 3350
F 0 "#PWR026" H 6450 3100 50  0001 C CNN
F 1 "GND" H 6450 3200 50  0000 C CNN
F 2 "" H 6450 3350 50  0000 C CNN
F 3 "" H 6450 3350 50  0000 C CNN
	1    6450 3350
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR027
U 1 1 58565082
P 7350 3100
F 0 "#PWR027" H 7350 2850 50  0001 C CNN
F 1 "GND" H 7350 2950 50  0000 C CNN
F 2 "" H 7350 3100 50  0000 C CNN
F 3 "" H 7350 3100 50  0000 C CNN
	1    7350 3100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR028
U 1 1 5856510A
P 7450 3500
F 0 "#PWR028" H 7450 3250 50  0001 C CNN
F 1 "GND" H 7450 3350 50  0000 C CNN
F 2 "" H 7450 3500 50  0000 C CNN
F 3 "" H 7450 3500 50  0000 C CNN
	1    7450 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	10400 1300 10400 3150
Connection ~ 10400 1750
Connection ~ 10400 1850
Connection ~ 10400 1950
Connection ~ 10400 2050
Connection ~ 10400 2150
Connection ~ 10400 2250
Connection ~ 10400 2350
Connection ~ 10400 2450
Connection ~ 10400 2550
Connection ~ 10400 2650
Connection ~ 10400 2750
Connection ~ 10400 2850
Connection ~ 10400 2950
Connection ~ 10400 3050
Connection ~ 10400 3150
Connection ~ 10400 1650
Connection ~ 10400 1400
Wire Wire Line
	1750 4500 3000 4500
Wire Wire Line
	1750 4600 3100 4600
Wire Wire Line
	1750 4700 4000 4700
Wire Wire Line
	1750 4800 4100 4800
Wire Wire Line
	1750 4900 5000 4900
Wire Wire Line
	1750 5000 5100 5000
Wire Wire Line
	1750 5200 6000 5200
Wire Wire Line
	1750 5300 6100 5300
Wire Wire Line
	1750 5400 7000 5400
Wire Wire Line
	1750 5500 7100 5500
Wire Wire Line
	1750 5600 8000 5600
Wire Wire Line
	1750 5700 8100 5700
Wire Wire Line
	1750 5800 9000 5800
Wire Wire Line
	1750 5900 9150 5900
Wire Wire Line
	9600 3450 9600 3150
Wire Wire Line
	9600 3150 9900 3150
Wire Wire Line
	9500 3050 9900 3050
Wire Wire Line
	8600 2950 9900 2950
Wire Wire Line
	8500 2850 9900 2850
Wire Wire Line
	7600 2750 9900 2750
Wire Wire Line
	7500 2650 9900 2650
Wire Wire Line
	6600 2550 9900 2550
Wire Wire Line
	6500 2450 9900 2450
Wire Wire Line
	5600 2350 9900 2350
Wire Wire Line
	5500 2250 9900 2250
Wire Wire Line
	4600 2150 9900 2150
Wire Wire Line
	4500 2050 9900 2050
Wire Wire Line
	3600 1950 9900 1950
Wire Wire Line
	3500 1850 9900 1850
Wire Wire Line
	2600 1750 9900 1750
Wire Wire Line
	2500 1650 9900 1650
Wire Wire Line
	1000 1600 950  1600
Wire Wire Line
	1000 1900 950  1900
Wire Wire Line
	1550 1900 1500 1900
Wire Wire Line
	1550 2000 1500 2000
Wire Wire Line
	1000 2000 950  2000
Wire Wire Line
	1500 1600 1750 1600
Wire Wire Line
	1000 1700 800  1700
Wire Wire Line
	1100 6250 1100 6200
Wire Wire Line
	1100 6200 1300 6200
Wire Wire Line
	1300 6200 1300 6250
Connection ~ 1200 6200
Wire Wire Line
	1300 6600 1300 6550
Wire Wire Line
	1100 6600 1300 6600
Wire Wire Line
	1100 6550 1100 6600
Connection ~ 1200 6600
Wire Wire Line
	3650 6150 3700 6150
Wire Wire Line
	3650 6250 3950 6250
Wire Wire Line
	3650 6350 3950 6350
Wire Wire Line
	3650 6650 4350 6650
Wire Wire Line
	10450 4000 10250 4000
Wire Wire Line
	10450 4400 10250 4400
Wire Wire Line
	10450 4800 10250 4800
Wire Wire Line
	10450 5200 10250 5200
Wire Wire Line
	10450 5600 10250 5600
Wire Wire Line
	10200 3800 10200 5900
Connection ~ 10200 4300
Connection ~ 10200 4700
Connection ~ 10200 5100
Wire Wire Line
	10700 3900 10700 6000
Connection ~ 10700 5100
Connection ~ 10700 4700
Connection ~ 10700 4300
Connection ~ 10200 3900
Connection ~ 10700 5500
Wire Wire Line
	2100 6150 1800 6150
Wire Wire Line
	2100 6250 1800 6250
Wire Wire Line
	2100 6350 1800 6350
Wire Wire Line
	2100 6450 1800 6450
Wire Wire Line
	2100 6550 1800 6550
Wire Wire Line
	3650 6550 4000 6550
Wire Wire Line
	2100 6650 1800 6650
Wire Wire Line
	2100 6750 1800 6750
Wire Wire Line
	2100 6850 1800 6850
Wire Wire Line
	2100 6950 1800 6950
Wire Wire Line
	2100 7050 1800 7050
Wire Wire Line
	2100 7150 1800 7150
Wire Wire Line
	2100 7250 1800 7250
Wire Wire Line
	2100 7350 1800 7350
Wire Wire Line
	2100 7450 2050 7450
Wire Wire Line
	3650 6750 3950 6750
Wire Wire Line
	3650 6850 3950 6850
Wire Wire Line
	3650 6950 3950 6950
Wire Wire Line
	3650 7050 3950 7050
Wire Wire Line
	3650 7150 3950 7150
Wire Wire Line
	3650 7250 3950 7250
Wire Wire Line
	3650 7350 3950 7350
Wire Wire Line
	3650 7450 3950 7450
Wire Wire Line
	1750 4400 2100 4400
Wire Wire Line
	1750 4300 2050 4300
Wire Wire Line
	3650 6450 4250 6450
Connection ~ 10200 5500
Connection ~ 10700 5900
Wire Wire Line
	10450 6000 10450 6100
Wire Wire Line
	10450 6100 10250 6100
Wire Wire Line
	4250 6450 4250 6650
Connection ~ 4250 6650
Wire Wire Line
	9300 4000 9600 4000
Wire Wire Line
	9600 4000 9600 3850
Wire Wire Line
	9300 3650 9300 3700
Wire Wire Line
	9300 3650 9150 3650
Wire Wire Line
	9150 3650 9150 3700
Wire Wire Line
	9450 4050 9450 4000
Connection ~ 9450 4000
Wire Wire Line
	9250 3450 9500 3450
Wire Wire Line
	9250 3450 9250 3550
Wire Wire Line
	9250 3550 9150 3550
Connection ~ 9350 3450
Wire Wire Line
	9000 3250 9200 3250
Connection ~ 9150 3250
Wire Wire Line
	8600 2950 8600 3250
Wire Wire Line
	8500 3250 8250 3250
Wire Wire Line
	8250 3250 8250 3350
Wire Wire Line
	8250 3350 8150 3350
Wire Wire Line
	8000 3050 8200 3050
Connection ~ 8150 3050
Wire Wire Line
	8100 3450 8300 3450
Connection ~ 8250 3450
Wire Wire Line
	8250 3750 8600 3750
Wire Wire Line
	8600 3750 8600 3650
Wire Wire Line
	8350 3250 8350 3300
Connection ~ 8350 3250
Wire Wire Line
	2600 1750 2600 2100
Wire Wire Line
	3600 1950 3600 2300
Wire Wire Line
	4600 2150 4600 2500
Wire Wire Line
	5600 2350 5600 2700
Wire Wire Line
	6600 2550 6600 2900
Wire Wire Line
	7100 3250 7300 3250
Connection ~ 7250 3250
Wire Wire Line
	7000 2850 7200 2850
Connection ~ 7150 2850
Wire Wire Line
	7500 3050 7250 3050
Wire Wire Line
	7250 3050 7250 3150
Wire Wire Line
	7250 3150 7150 3150
Wire Wire Line
	7600 3450 7350 3450
Wire Wire Line
	7350 3450 7350 3550
Wire Wire Line
	7350 3550 7250 3550
Wire Wire Line
	6600 3300 6350 3300
Wire Wire Line
	6350 3300 6350 3400
Wire Wire Line
	6350 3400 6250 3400
Wire Wire Line
	6100 3100 6300 3100
Connection ~ 6250 3100
Wire Wire Line
	6000 2650 6200 2650
Connection ~ 6150 2650
Wire Wire Line
	6150 2950 6250 2950
Wire Wire Line
	6250 2950 6250 2850
Wire Wire Line
	6250 2850 6500 2850
Wire Wire Line
	5000 2450 5200 2450
Connection ~ 5150 2450
Wire Wire Line
	5500 2650 5250 2650
Wire Wire Line
	5250 2650 5250 2750
Wire Wire Line
	5250 2750 5150 2750
Wire Wire Line
	5100 2900 5300 2900
Connection ~ 5250 2900
Wire Wire Line
	5600 3100 5350 3100
Wire Wire Line
	5350 3100 5350 3200
Wire Wire Line
	5350 3200 5250 3200
Wire Wire Line
	4100 2700 4300 2700
Connection ~ 4250 2700
Wire Wire Line
	4600 2900 4350 2900
Wire Wire Line
	4350 2900 4350 3000
Wire Wire Line
	4350 3000 4250 3000
Wire Wire Line
	4500 2450 4250 2450
Wire Wire Line
	4250 2450 4250 2550
Wire Wire Line
	4250 2550 4150 2550
Wire Wire Line
	4000 2250 4200 2250
Connection ~ 4150 2250
Wire Wire Line
	3000 2050 3200 2050
Connection ~ 3150 2050
Wire Wire Line
	3500 2250 3250 2250
Wire Wire Line
	3250 2250 3250 2350
Wire Wire Line
	3250 2350 3150 2350
Wire Wire Line
	3600 2700 3350 2700
Wire Wire Line
	3350 2700 3350 2800
Wire Wire Line
	3350 2800 3250 2800
Wire Wire Line
	3100 2500 3300 2500
Connection ~ 3250 2500
Wire Wire Line
	2600 2500 2350 2500
Wire Wire Line
	2350 2500 2350 2600
Wire Wire Line
	2350 2600 2250 2600
Wire Wire Line
	2100 2300 2300 2300
Connection ~ 2250 2300
Wire Wire Line
	2500 2050 2250 2050
Wire Wire Line
	2250 2050 2250 2150
Wire Wire Line
	2250 2150 2150 2150
Wire Wire Line
	2000 1850 2200 1850
Connection ~ 2150 1850
Wire Wire Line
	7600 2750 7600 3050
Wire Wire Line
	7350 3100 7350 3050
Connection ~ 7350 3050
Wire Wire Line
	7450 3500 7450 3450
Connection ~ 7450 3450
Wire Wire Line
	6450 3350 6450 3300
Connection ~ 6450 3300
Wire Wire Line
	6350 2900 6350 2850
Connection ~ 6350 2850
Wire Wire Line
	5450 3150 5450 3100
Connection ~ 5450 3100
Wire Wire Line
	5350 2700 5350 2650
Connection ~ 5350 2650
Wire Wire Line
	4450 2950 4450 2900
Connection ~ 4450 2900
Wire Wire Line
	4350 2500 4350 2450
Connection ~ 4350 2450
Wire Wire Line
	3300 2300 3300 2250
Connection ~ 3300 2250
Wire Wire Line
	3450 2750 3450 2700
Connection ~ 3450 2700
Wire Wire Line
	2400 2550 2400 2500
Connection ~ 2400 2500
Wire Wire Line
	2350 2100 2350 2050
Connection ~ 2350 2050
Wire Wire Line
	9150 5900 9150 4000
Wire Wire Line
	9000 5800 9000 3550
Wire Wire Line
	8100 5700 8100 3750
Wire Wire Line
	8000 5600 8000 3350
Wire Wire Line
	7100 5500 7100 3550
Wire Wire Line
	7000 5400 7000 3150
Wire Wire Line
	6100 5300 6100 3400
Wire Wire Line
	6000 5200 6000 2950
Wire Wire Line
	5100 5000 5100 3200
Wire Wire Line
	5000 4900 5000 2750
Wire Wire Line
	4100 4800 4100 3000
Wire Wire Line
	4000 4700 4000 2550
Wire Wire Line
	3100 4600 3100 2800
Wire Wire Line
	3000 4500 3000 2350
Wire Wire Line
	2100 4400 2100 2600
Wire Wire Line
	2000 2150 2000 4150
Wire Wire Line
	2000 4150 2050 4150
Wire Wire Line
	2050 4150 2050 4300
$Comp
L GND #PWR029
U 1 1 5857C577
P 8450 3800
F 0 "#PWR029" H 8450 3550 50  0001 C CNN
F 1 "GND" H 8450 3650 50  0000 C CNN
F 2 "" H 8450 3800 50  0000 C CNN
F 3 "" H 8450 3800 50  0000 C CNN
	1    8450 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	8450 3800 8450 3750
Connection ~ 8450 3750
$Comp
L CONN_01X01 M1
U 1 1 585B2347
P 5900 6550
F 0 "M1" H 5900 6650 50  0000 C CNN
F 1 "CONN_01X01" V 6000 6550 50  0001 C CNN
F 2 "Mounting_Holes:MountingHole_4.3mm_M4_DIN965" H 5900 6550 50  0001 C CNN
F 3 "" H 5900 6550 50  0000 C CNN
	1    5900 6550
	1    0    0    -1  
$EndComp
$Comp
L CONN_01X01 M2
U 1 1 585B247A
P 5900 7100
F 0 "M2" H 5900 7200 50  0000 C CNN
F 1 "CONN_01X01" V 6000 7100 50  0001 C CNN
F 2 "Mounting_Holes:MountingHole_4.3mm_M4_DIN965" H 5900 7100 50  0001 C CNN
F 3 "" H 5900 7100 50  0000 C CNN
	1    5900 7100
	1    0    0    -1  
$EndComp
$EndSCHEMATC
