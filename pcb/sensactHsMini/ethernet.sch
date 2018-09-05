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
LIBS:stm32
LIBS:sensact
LIBS:w_connectors
LIBS:w_device
LIBS:relays
LIBS:sensactHsMini-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 3 4
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
L MODULE_LAN8720 U2
U 1 1 5A6148AB
P 4200 2750
F 0 "U2" H 4350 2800 60  0000 L CNN
F 1 "MODULE_LAN8720" H 4350 2700 60  0000 L CNN
F 2 "sensact:MODULE_LAN8720" H 4200 2750 60  0001 C CNN
F 3 "" H 4200 2750 60  0001 C CNN
	1    4200 2750
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR012
U 1 1 5A6148DB
P 3850 3350
F 0 "#PWR012" H 3850 3200 50  0001 C CNN
F 1 "+3V3" H 3850 3490 50  0000 C CNN
F 2 "" H 3850 3350 50  0000 C CNN
F 3 "" H 3850 3350 50  0000 C CNN
	1    3850 3350
	0    -1   -1   0   
$EndComp
$Comp
L GND #PWR013
U 1 1 5A6148F1
P 3850 3150
F 0 "#PWR013" H 3850 2900 50  0001 C CNN
F 1 "GND" H 3850 3000 50  0000 C CNN
F 2 "" H 3850 3150 50  0000 C CNN
F 3 "" H 3850 3150 50  0000 C CNN
	1    3850 3150
	0    1    1    0   
$EndComp
Wire Wire Line
	3900 3100 3850 3100
Wire Wire Line
	3850 3100 3850 3200
Wire Wire Line
	3850 3200 3900 3200
Connection ~ 3850 3150
Wire Wire Line
	3900 3300 3850 3300
Wire Wire Line
	3850 3300 3850 3400
Wire Wire Line
	3850 3400 3900 3400
Connection ~ 3850 3350
Text GLabel 3850 2100 0    60   Input ~ 0
ETH_TX1
Text GLabel 3850 2300 0    60   Input ~ 0
ETH_TXEN
Text GLabel 3850 2400 0    60   Input ~ 0
ETH_TX0
Text GLabel 3850 2500 0    60   Output ~ 0
ETH_RX0
Text GLabel 3850 2600 0    60   Output ~ 0
ETH_RX1
Text GLabel 3850 2800 0    60   Output ~ 0
ETH_CRS
Text GLabel 3850 2900 0    60   BiDi ~ 0
ETH_MDIO
Text GLabel 3850 3000 0    60   Input ~ 0
ETH_MDC
Wire Wire Line
	3900 2100 3850 2100
Wire Wire Line
	3900 2300 3850 2300
Wire Wire Line
	3900 2400 3850 2400
Wire Wire Line
	3900 2500 3850 2500
Wire Wire Line
	3900 2600 3850 2600
Wire Wire Line
	3900 2800 3850 2800
Wire Wire Line
	3900 2900 3850 2900
Wire Wire Line
	3900 3000 3850 3000
NoConn ~ 3900 2200
$EndSCHEMATC
