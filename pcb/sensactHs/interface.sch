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
Sheet 22 22
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
L SN65HVD230 U?
U 1 1 57EF75EB
P 3650 1950
F 0 "U?" H 3250 2300 60  0000 L CNN
F 1 "SN65HVD230" H 3250 2200 50  0000 L CNN
F 2 "kl_footprints:SOIC-8_3.9x4.9mm_Pitch1.27mm_handsolder" H 3650 1950 60  0001 C CNN
F 3 "" H 3650 1950 60  0000 C CNN
	1    3650 1950
	1    0    0    -1  
$EndComp
$Comp
L JUMPER JP?
U 1 1 57EF75F2
P 5200 2200
F 0 "JP?" H 5200 2350 50  0000 C CNN
F 1 "CAN_TERM" H 5200 2120 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 5200 2200 60  0001 C CNN
F 3 "" H 5200 2200 60  0000 C CNN
	1    5200 2200
	0    1    1    0   
$EndComp
$Comp
L GND #PWR?
U 1 1 57EF75F9
P 2600 1400
F 0 "#PWR?" H 2600 1150 60  0001 C CNN
F 1 "GND" H 2600 1250 60  0000 C CNN
F 2 "" H 2600 1400 60  0000 C CNN
F 3 "" H 2600 1400 60  0000 C CNN
	1    2600 1400
	-1   0    0    1   
$EndComp
$Comp
L +3V3 #PWR?
U 1 1 57EF75FF
P 2450 2000
F 0 "#PWR?" H 2450 1850 60  0001 C CNN
F 1 "+3V3" H 2450 2140 60  0000 C CNN
F 2 "" H 2450 2000 60  0000 C CNN
F 3 "" H 2450 2000 60  0000 C CNN
	1    2450 2000
	0    -1   -1   0   
$EndComp
Text Notes 3850 2300 0    60   ~ 0
CAN-PHY
$Comp
L R R?
U 1 1 57EF7606
P 3550 1500
F 0 "R?" V 3630 1500 50  0000 C CNN
F 1 "10k" V 3550 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3480 1500 30  0001 C CNN
F 3 "" H 3550 1500 30  0000 C CNN
	1    3550 1500
	0    1    1    0   
$EndComp
$Comp
L R R?
U 1 1 57EF760D
P 4900 2550
F 0 "R?" V 4980 2550 50  0000 C CNN
F 1 "120R" V 4900 2550 50  0000 C CNN
F 2 "Resistors_SMD:R_1206_HandSoldering" V 4830 2550 30  0001 C CNN
F 3 "" H 4900 2550 30  0000 C CNN
	1    4900 2550
	0    1    1    0   
$EndComp
NoConn ~ 4250 2100
Wire Wire Line
	2600 1900 3050 1900
Wire Wire Line
	4250 1800 4250 1500
Wire Wire Line
	2600 1400 2600 1900
Wire Wire Line
	2450 2000 3050 2000
Connection ~ 2600 1500
Wire Wire Line
	4250 1500 3700 1500
Wire Wire Line
	3400 1500 2600 1500
Wire Wire Line
	1800 2450 4500 2450
Wire Wire Line
	4500 2450 4500 1900
Wire Wire Line
	4250 1900 5200 1900
Wire Wire Line
	1700 2550 4750 2550
Wire Wire Line
	4600 2550 4600 2000
Wire Wire Line
	4600 2000 4250 2000
Connection ~ 4600 2550
Wire Wire Line
	5050 2550 5200 2550
Wire Wire Line
	5200 2550 5200 2500
Connection ~ 4500 1900
Text Label 4750 1900 0    60   ~ 0
CANH
Text Label 4600 2000 0    60   ~ 0
CANL
Text GLabel 3000 1800 0    60   BiDi ~ 0
CANT
Text GLabel 3000 2100 0    60   BiDi ~ 0
CANR
Wire Wire Line
	3050 1800 3000 1800
Wire Wire Line
	3050 2100 3000 2100
$EndSCHEMATC
