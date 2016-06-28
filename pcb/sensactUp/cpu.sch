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
Sheet 4 4
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
L CRYSTAL X1
U 1 1 54E37D6B
P 3450 2250
F 0 "X1" H 3450 2400 50  0000 C CNN
F 1 "8MHz" H 3450 2100 50  0000 C CNN
F 2 "Crystals:Crystal_HC49-U_Vertical" H 3450 2250 60  0001 C CNN
F 3 "" H 3450 2250 60  0000 C CNN
	1    3450 2250
	0    1    1    0   
$EndComp
$Comp
L C-RESCUE-multiupbox C5
U 1 1 54E37E57
P 3050 1950
F 0 "C5" H 3100 2050 50  0000 L CNN
F 1 "27p" H 3100 1850 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3088 1800 30  0001 C CNN
F 3 "" H 3050 1950 60  0000 C CNN
	1    3050 1950
	0    1    1    0   
$EndComp
$Comp
L C-RESCUE-multiupbox C6
U 1 1 54E37EEC
P 3050 2550
F 0 "C6" H 3100 2650 50  0000 L CNN
F 1 "27p" H 3100 2450 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3088 2400 30  0001 C CNN
F 3 "" H 3050 2550 60  0000 C CNN
	1    3050 2550
	0    1    1    0   
$EndComp
$Comp
L R-RESCUE-multiupbox R5
U 1 1 54E37F27
P 3750 2250
F 0 "R5" V 3830 2250 50  0000 C CNN
F 1 "1M" V 3757 2251 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3680 2250 30  0001 C CNN
F 3 "" H 3750 2250 30  0000 C CNN
	1    3750 2250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR077
U 1 1 54E37FB8
P 2500 2350
F 0 "#PWR077" H 2500 2100 60  0001 C CNN
F 1 "GND" H 2500 2200 60  0000 C CNN
F 2 "" H 2500 2350 60  0000 C CNN
F 3 "" H 2500 2350 60  0000 C CNN
	1    2500 2350
	0    1    1    0   
$EndComp
$Comp
L SW_PUSH SW1
U 1 1 54E3803D
P 3800 1550
F 0 "SW1" H 3950 1660 50  0000 C CNN
F 1 "SW_PUSH" H 3800 1470 50  0000 C CNN
F 2 "Discret:SW_PUSH_SMALL" H 3800 1550 60  0001 C CNN
F 3 "" H 3800 1550 60  0000 C CNN
	1    3800 1550
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-multiupbox C4
U 1 1 54E380E8
P 3900 1200
F 0 "C4" H 3950 1300 50  0000 L CNN
F 1 "100n" H 3950 1100 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3938 1050 30  0001 C CNN
F 3 "" H 3900 1200 60  0000 C CNN
	1    3900 1200
	0    1    1    0   
$EndComp
$Comp
L R-RESCUE-multiupbox R3
U 1 1 54E381F7
P 4350 1200
F 0 "R3" V 4430 1200 50  0000 C CNN
F 1 "10k" V 4357 1201 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4280 1200 30  0001 C CNN
F 3 "" H 4350 1200 30  0000 C CNN
	1    4350 1200
	0    1    1    0   
$EndComp
$Comp
L INDUCTOR L1
U 1 1 54E3830D
P 5600 1000
F 0 "L1" V 5550 1000 50  0000 C CNN
F 1 "INDUCTOR" V 5700 1000 50  0000 C CNN
F 2 "Inductors_NEOSID:Neosid_Inductor_SM1206" H 5600 1000 60  0001 C CNN
F 3 "" H 5600 1000 60  0000 C CNN
	1    5600 1000
	0    1    1    0   
$EndComp
$Comp
L C-RESCUE-multiupbox C8
U 1 1 54E384D3
P 6450 1000
F 0 "C8" H 6500 1100 50  0000 L CNN
F 1 "100n" H 6500 900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6488 850 30  0001 C CNN
F 3 "" H 6450 1000 60  0000 C CNN
	1    6450 1000
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-multiupbox C9
U 1 1 54E384F6
P 6700 1000
F 0 "C9" H 6750 1100 50  0000 L CNN
F 1 "100n" H 6750 900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6738 850 30  0001 C CNN
F 3 "" H 6700 1000 60  0000 C CNN
	1    6700 1000
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-multiupbox C10
U 1 1 54E38515
P 6950 1000
F 0 "C10" H 7000 1100 50  0000 L CNN
F 1 "100n" H 7000 900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6988 850 30  0001 C CNN
F 3 "" H 6950 1000 60  0000 C CNN
	1    6950 1000
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR078
U 1 1 54E38856
P 6950 1400
F 0 "#PWR078" H 6950 1150 60  0001 C CNN
F 1 "GND" H 6950 1250 60  0000 C CNN
F 2 "" H 6950 1400 60  0000 C CNN
F 3 "" H 6950 1400 60  0000 C CNN
	1    6950 1400
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR079
U 1 1 54E3886A
P 6300 750
F 0 "#PWR079" H 6300 600 60  0001 C CNN
F 1 "+3V3" H 6300 890 60  0000 C CNN
F 2 "" H 6300 750 60  0000 C CNN
F 3 "" H 6300 750 60  0000 C CNN
	1    6300 750 
	1    0    0    -1  
$EndComp
$Comp
L C-RESCUE-multiupbox C7
U 1 1 54E38AE4
P 6050 1200
F 0 "C7" H 6100 1300 50  0000 L CNN
F 1 "100n" H 6100 1100 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 6088 1050 30  0001 C CNN
F 3 "" H 6050 1200 60  0000 C CNN
	1    6050 1200
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR080
U 1 1 54E38EFA
P 5750 5050
F 0 "#PWR080" H 5750 4800 60  0001 C CNN
F 1 "GND" H 5750 4900 60  0000 C CNN
F 2 "" H 5750 5050 60  0000 C CNN
F 3 "" H 5750 5050 60  0000 C CNN
	1    5750 5050
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR081
U 1 1 54E39415
P 9300 3500
F 0 "#PWR081" H 9300 3250 60  0001 C CNN
F 1 "GND" H 9300 3350 60  0000 C CNN
F 2 "" H 9300 3500 60  0000 C CNN
F 3 "" H 9300 3500 60  0000 C CNN
	1    9300 3500
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR082
U 1 1 54E394A4
P 9300 3100
F 0 "#PWR082" H 9300 2950 60  0001 C CNN
F 1 "+3V3" H 9300 3240 60  0000 C CNN
F 2 "" H 9300 3100 60  0000 C CNN
F 3 "" H 9300 3100 60  0000 C CNN
	1    9300 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2200 4200 2200
Wire Wire Line
	3900 1950 3900 2200
Wire Wire Line
	3900 2350 4200 2350
Wire Wire Line
	3900 2550 3900 2350
Wire Wire Line
	3750 1950 3750 2000
Wire Wire Line
	3250 1950 3900 1950
Wire Wire Line
	3750 2550 3750 2500
Wire Wire Line
	3250 2550 3900 2550
Connection ~ 3450 2550
Connection ~ 3450 1950
Wire Wire Line
	2500 1950 2850 1950
Wire Wire Line
	2500 2550 2850 2550
Connection ~ 2500 2350
Connection ~ 3750 1950
Connection ~ 3750 2550
Connection ~ 2500 1950
Wire Wire Line
	4600 1200 5750 1200
Wire Wire Line
	5300 800  5300 1500
Wire Wire Line
	5450 1200 5450 1500
Connection ~ 5300 1200
Wire Wire Line
	5600 1200 5600 1500
Connection ~ 5450 1200
Wire Wire Line
	5900 1000 5900 1500
Wire Wire Line
	5300 800  7450 800 
Connection ~ 6450 800 
Connection ~ 6700 800 
Wire Wire Line
	6300 750  6300 800 
Connection ~ 6300 800 
Connection ~ 5300 1000
Wire Wire Line
	6050 1400 7200 1400
Wire Wire Line
	5900 1000 6050 1000
Wire Wire Line
	6450 1400 6450 1200
Wire Wire Line
	6700 1400 6700 1200
Connection ~ 6450 1400
Wire Wire Line
	6950 1200 6950 1450
Connection ~ 6700 1400
Connection ~ 6950 1400
Wire Wire Line
	5300 5000 5900 5000
Connection ~ 5600 5000
Connection ~ 5450 5000
Wire Wire Line
	5750 5050 5750 5000
Connection ~ 5750 5000
Wire Wire Line
	4000 2000 4200 2000
Wire Wire Line
	4000 2000 4000 1750
Connection ~ 5900 1000
$Comp
L +3V3 #PWR083
U 1 1 54E39C96
P 4100 2550
F 0 "#PWR083" H 4100 2400 60  0001 C CNN
F 1 "+3V3" H 4100 2690 60  0000 C CNN
F 2 "" H 4100 2550 60  0000 C CNN
F 3 "" H 4100 2550 60  0000 C CNN
	1    4100 2550
	1    0    0    -1  
$EndComp
$Comp
L PWR_FLAG #FLG084
U 1 1 54E3A2D5
P 5950 1000
F 0 "#FLG084" H 5950 1095 30  0001 C CNN
F 1 "PWR_FLAG" H 5950 1180 30  0000 C CNN
F 2 "" H 5950 1000 60  0000 C CNN
F 3 "" H 5950 1000 60  0000 C CNN
	1    5950 1000
	1    0    0    -1  
$EndComp
Connection ~ 5950 1000
Wire Wire Line
	5750 1200 5750 1500
Connection ~ 5600 1200
Wire Wire Line
	5900 1500 6050 1500
Wire Wire Line
	4100 1850 4200 1850
$Comp
L C-RESCUE-multiupbox C11
U 1 1 54E4C78F
P 7200 1000
F 0 "C11" H 7250 1100 50  0000 L CNN
F 1 "100n" H 7250 900 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 7238 850 30  0001 C CNN
F 3 "" H 7200 1000 60  0000 C CNN
	1    7200 1000
	1    0    0    -1  
$EndComp
Connection ~ 6950 800 
Connection ~ 7200 800 
Wire Wire Line
	7200 1400 7200 1200
Wire Bus Line
	8050 1700 8050 3350
Entry Wire Line
	8050 1850 8150 1950
Entry Wire Line
	8050 1950 8150 2050
Entry Wire Line
	8050 2050 8150 2150
Entry Wire Line
	8050 2150 8150 2250
Entry Wire Line
	8050 2250 8150 2350
Entry Wire Line
	8050 2350 8150 2450
Entry Wire Line
	8050 2450 8150 2550
Entry Wire Line
	8050 2550 8150 2650
Entry Wire Line
	8050 2650 8150 2750
Entry Wire Line
	8050 2750 8150 2850
Entry Wire Line
	8050 2850 8150 2950
Entry Wire Line
	8050 2950 8150 3050
Entry Wire Line
	8050 3050 8150 3150
Wire Wire Line
	7300 1950 8150 1950
Wire Wire Line
	7300 2050 8150 2050
Wire Wire Line
	7300 2150 8150 2150
Wire Wire Line
	7300 2250 8150 2250
Wire Wire Line
	7300 2350 8150 2350
Wire Wire Line
	7300 2450 8150 2450
Wire Wire Line
	7300 2550 8150 2550
Wire Wire Line
	7300 2650 8150 2650
Wire Wire Line
	7300 2750 8150 2750
Entry Wire Line
	8050 3150 8150 3250
Entry Wire Line
	8050 3250 8150 3350
Entry Wire Line
	8050 3350 8150 3450
Wire Wire Line
	8150 3450 7300 3450
Wire Wire Line
	7300 3150 8150 3150
Wire Wire Line
	7300 3050 8150 3050
Wire Wire Line
	7300 2950 8150 2950
Wire Wire Line
	7300 2850 8150 2850
Text HLabel 8050 1750 0    60   BiDi ~ 0
PA[0..15]
Text Label 7400 1950 0    60   ~ 0
PA0
Text Label 7400 2050 0    60   ~ 0
PA1
Text Label 7400 2150 0    60   ~ 0
PA2
Text Label 7400 2250 0    60   ~ 0
PA3
Text Label 7400 2350 0    60   ~ 0
PA4
Text Label 7400 2450 0    60   ~ 0
PA5
Text Label 7400 2550 0    60   ~ 0
PA6
Text Label 7400 2650 0    60   ~ 0
PA7
Text Label 7400 2750 0    60   ~ 0
PA8
Text Label 7400 2850 0    60   ~ 0
PA9
Text Label 7400 2950 0    60   ~ 0
PA10
Text Label 7400 3050 0    60   ~ 0
PA11
Text Label 7400 3150 0    60   ~ 0
PA12
Text Label 7550 3450 0    60   ~ 0
PA15
Wire Bus Line
	3750 3000 3750 4750
Entry Wire Line
	3750 4750 3850 4850
Entry Wire Line
	3750 4650 3850 4750
Entry Wire Line
	3750 4550 3850 4650
Entry Wire Line
	3750 4450 3850 4550
Entry Wire Line
	3750 4350 3850 4450
Entry Wire Line
	3750 4250 3850 4350
Entry Wire Line
	3750 4150 3850 4250
Entry Wire Line
	3750 4050 3850 4150
Entry Wire Line
	3750 3950 3850 4050
Entry Wire Line
	3750 3850 3850 3950
Entry Wire Line
	3750 3750 3850 3850
Entry Wire Line
	3750 3650 3850 3750
Entry Wire Line
	3750 3550 3850 3650
Entry Wire Line
	3750 3350 3850 3450
Entry Wire Line
	3750 3250 3850 3350
Entry Wire Line
	3750 3150 3850 3250
Wire Wire Line
	4200 4850 3850 4850
Wire Wire Line
	4200 4750 3850 4750
Wire Wire Line
	3850 4650 4200 4650
Wire Wire Line
	3850 4550 4200 4550
Wire Wire Line
	3850 3650 4200 3650
Wire Wire Line
	3850 3750 4200 3750
Wire Wire Line
	3850 3850 4200 3850
Wire Wire Line
	3850 3950 4200 3950
Wire Wire Line
	3850 4050 4200 4050
Wire Wire Line
	3850 4150 4200 4150
Wire Wire Line
	3850 4250 4200 4250
Wire Wire Line
	3850 4350 4200 4350
Wire Wire Line
	3850 4450 4200 4450
Wire Wire Line
	3850 3250 4200 3250
Wire Wire Line
	3850 3350 4200 3350
Wire Wire Line
	3850 3450 4200 3450
Wire Wire Line
	4200 3050 4100 3050
Wire Wire Line
	5300 5600 5300 5800
Wire Wire Line
	5300 5700 6050 5700
Wire Wire Line
	5450 5700 5450 5600
Wire Wire Line
	5600 5700 5600 5600
Connection ~ 5450 5700
Wire Wire Line
	5750 5700 5750 5600
Connection ~ 5600 5700
Wire Wire Line
	6050 5700 6050 5600
Connection ~ 5750 5700
$Comp
L GND #PWR085
U 1 1 54E4EF16
P 5300 5800
F 0 "#PWR085" H 5300 5550 60  0001 C CNN
F 1 "GND" H 5300 5650 60  0000 C CNN
F 2 "" H 5300 5800 60  0000 C CNN
F 3 "" H 5300 5800 60  0000 C CNN
	1    5300 5800
	1    0    0    -1  
$EndComp
Connection ~ 5300 5700
Text HLabel 3750 3050 0    60   Input ~ 0
PC[0..15]
Text Label 3900 3650 0    60   ~ 0
PC0
Text Label 3900 3750 0    60   ~ 0
PC1
Text Label 3900 3850 0    60   ~ 0
PC2
Text Label 3900 3950 0    60   ~ 0
PC3
Text Label 3900 4050 0    60   ~ 0
PC4
Text Label 3900 4150 0    60   ~ 0
PC5
Text Label 3900 4250 0    60   ~ 0
PC6
Text Label 3900 4350 0    60   ~ 0
PC7
Text Label 3900 4450 0    60   ~ 0
PC8
Text Label 3900 4550 0    60   ~ 0
PC9
Text Label 3900 4650 0    60   ~ 0
PC10
Text Label 3900 4750 0    60   ~ 0
PC11
Text Label 3900 4850 0    60   ~ 0
PC12
Text Label 3900 3250 0    60   ~ 0
PC13
Text Label 3900 3350 0    60   ~ 0
PC14
Text Label 3900 3450 0    60   ~ 0
PC15
Wire Wire Line
	7300 3250 8150 3250
Wire Wire Line
	7300 3350 8150 3350
Text Label 7400 3250 0    60   ~ 0
PA13
Text Label 7400 3350 0    60   ~ 0
PA14
Wire Bus Line
	7650 3500 7650 5050
Entry Wire Line
	7650 3550 7750 3650
Entry Wire Line
	7650 3650 7750 3750
Entry Wire Line
	7650 3750 7750 3850
Entry Wire Line
	7650 3850 7750 3950
Entry Wire Line
	7650 3950 7750 4050
Entry Wire Line
	7650 4050 7750 4150
Entry Wire Line
	7650 4150 7750 4250
Entry Wire Line
	7650 4250 7750 4350
Entry Wire Line
	7650 4350 7750 4450
Entry Wire Line
	7650 4450 7750 4550
Entry Wire Line
	7650 4550 7750 4650
Entry Wire Line
	7650 4650 7750 4750
Entry Wire Line
	7650 4750 7750 4850
Entry Wire Line
	7650 4850 7750 4950
Entry Wire Line
	7650 4950 7750 5050
Entry Wire Line
	7650 5050 7750 5150
Wire Wire Line
	7300 3650 7750 3650
Wire Wire Line
	7300 3750 7750 3750
Wire Wire Line
	7300 3850 7750 3850
Wire Wire Line
	7300 3950 7750 3950
Wire Wire Line
	7300 4050 7750 4050
Wire Wire Line
	7300 4150 7750 4150
Wire Wire Line
	7300 4250 7750 4250
Wire Wire Line
	7300 4350 7750 4350
Wire Wire Line
	7300 4450 7750 4450
Wire Wire Line
	7300 4550 7750 4550
Wire Wire Line
	7300 4650 7750 4650
Wire Wire Line
	7300 4750 7750 4750
Wire Wire Line
	7300 4850 7750 4850
Wire Wire Line
	7300 4950 7750 4950
Wire Wire Line
	7300 5050 7750 5050
Wire Wire Line
	7300 5150 7750 5150
Text HLabel 7650 3500 0    60   BiDi ~ 0
PB[0..15]
Text Label 7400 3650 0    60   ~ 0
PB0
Text Label 7400 3750 0    60   ~ 0
PB1
Text Label 7400 3850 0    60   ~ 0
PB2
Text Label 7400 3950 0    60   ~ 0
PB3
Text Label 7400 4050 0    60   ~ 0
PB4
Text Label 7400 4150 0    60   ~ 0
PB5
Text Label 7400 4250 0    60   ~ 0
PB6
Text Label 7400 4350 0    60   ~ 0
PB7
Text Label 7400 4450 0    60   ~ 0
PB8
Text Label 7400 4550 0    60   ~ 0
PB9
Text Label 7400 4650 0    60   ~ 0
PB10
Text Label 7400 4750 0    60   ~ 0
PB11
Text Label 7400 4850 0    60   ~ 0
PB12
Text Label 7400 4950 0    60   ~ 0
PB13
Text Label 7400 5050 0    60   ~ 0
PB14
Text Label 7400 5150 0    60   ~ 0
PB15
Text HLabel 4100 3050 0    60   Input ~ 0
PD2
$Comp
L CONN_01X04 P17
U 1 1 54EF1D19
P 9500 3300
F 0 "P17" H 9500 3550 50  0000 C CNN
F 1 "CONN_01X04" V 9600 3300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04" H 9500 3300 60  0001 C CNN
F 3 "" H 9500 3300 60  0000 C CNN
	1    9500 3300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9300 3500 9300 3450
Wire Wire Line
	9300 3250 8950 3250
Wire Wire Line
	9300 3350 8950 3350
Text Label 9000 3250 0    60   ~ 0
PB10
Text Label 9000 3350 0    60   ~ 0
PB11
$Comp
L +3V3 #PWR086
U 1 1 54EF40F9
P 1050 1800
F 0 "#PWR086" H 1050 1650 60  0001 C CNN
F 1 "+3V3" H 1050 1940 60  0000 C CNN
F 2 "" H 1050 1800 60  0000 C CNN
F 3 "" H 1050 1800 60  0000 C CNN
	1    1050 1800
	1    0    0    -1  
$EndComp
Text Notes 8450 2050 0    60   ~ 0
Programmer-Anschluss
Text Notes 8950 2900 0    60   ~ 0
i2c-Anschluss
Wire Wire Line
	1250 5200 1950 5200
Wire Wire Line
	4200 2550 4100 2550
Wire Wire Line
	3500 1150 3500 1550
Wire Wire Line
	3500 1200 3700 1200
$Comp
L GND #PWR087
U 1 1 54FA0685
P 3500 1150
F 0 "#PWR087" H 3500 900 60  0001 C CNN
F 1 "GND" H 3500 1000 60  0000 C CNN
F 2 "" H 3500 1150 60  0000 C CNN
F 3 "" H 3500 1150 60  0000 C CNN
	1    3500 1150
	-1   0    0    1   
$EndComp
Connection ~ 3500 1200
Wire Wire Line
	2500 1750 2500 2550
Connection ~ 4100 1200
$Comp
L R-RESCUE-multiupbox R25
U 1 1 54FA09EF
P 3750 1750
F 0 "R25" V 3830 1750 50  0000 C CNN
F 1 "10k" V 3757 1751 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 3680 1750 30  0001 C CNN
F 3 "" H 3750 1750 30  0000 C CNN
	1    3750 1750
	0    1    1    0   
$EndComp
$Comp
L R-RESCUE-multiupbox R24
U 1 1 54FA0A46
P 2750 1750
F 0 "R24" V 2830 1750 50  0000 C CNN
F 1 "10k" V 2757 1751 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2680 1750 30  0001 C CNN
F 3 "" H 2750 1750 30  0000 C CNN
	1    2750 1750
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 1750 3500 1750
Wire Wire Line
	3050 1450 3050 1750
Connection ~ 3050 1750
$Comp
L +3V3 #PWR088
U 1 1 54FA0C6A
P 2450 1350
F 0 "#PWR088" H 2450 1200 60  0001 C CNN
F 1 "+3V3" H 2450 1490 60  0000 C CNN
F 2 "" H 2450 1350 60  0000 C CNN
F 3 "" H 2450 1350 60  0000 C CNN
	1    2450 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	2450 1350 2450 1450
Text HLabel 3050 1600 0    60   Input ~ 0
BOOT0
Wire Wire Line
	4100 1200 4100 1850
Text HLabel 4100 1400 2    60   Input ~ 0
NRST
Wire Wire Line
	7900 3850 8600 3850
$Comp
L GND #PWR089
U 1 1 54FA1078
P 9800 3850
F 0 "#PWR089" H 9800 3600 60  0001 C CNN
F 1 "GND" H 9800 3700 60  0000 C CNN
F 2 "" H 9800 3850 60  0000 C CNN
F 3 "" H 9800 3850 60  0000 C CNN
	1    9800 3850
	0    -1   -1   0   
$EndComp
Text Label 8150 3850 2    60   ~ 0
PB2
Connection ~ 4100 1550
$Comp
L SW_PUSH SW2
U 1 1 54FAC226
P 2750 1450
F 0 "SW2" H 2900 1560 50  0000 C CNN
F 1 "SW_PUSH" H 2750 1370 50  0000 C CNN
F 2 "Discret:SW_PUSH_SMALL" H 2750 1450 60  0001 C CNN
F 3 "" H 2750 1450 60  0000 C CNN
	1    2750 1450
	1    0    0    -1  
$EndComp
NoConn ~ 11600 1950
Wire Wire Line
	8450 2400 9050 2400
Wire Wire Line
	9050 2200 8450 2200
Text Label 8500 2200 0    60   ~ 0
PA14
Text Label 8500 2400 0    60   ~ 0
PA13
Wire Wire Line
	9300 3100 9300 3150
$Comp
L R-RESCUE-multiupbox R30
U 1 1 5501AE60
P 1050 2150
F 0 "R30" V 1130 2150 50  0000 C CNN
F 1 "330R" V 1057 2151 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 980 2150 30  0001 C CNN
F 3 "" H 1050 2150 30  0000 C CNN
	1    1050 2150
	1    0    0    -1  
$EndComp
$Comp
L LED-RESCUE-multiupbox D19
U 1 1 5501AE93
P 1050 2650
F 0 "D19" H 1050 2750 50  0000 C CNN
F 1 "LED" H 1050 2550 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" H 1050 2650 60  0001 C CNN
F 3 "" H 1050 2650 60  0000 C CNN
	1    1050 2650
	0    1    1    0   
$EndComp
Wire Wire Line
	1050 1800 1050 1800
Wire Wire Line
	1050 1800 1050 1900
Wire Wire Line
	1050 2400 1050 2450
Wire Wire Line
	1050 2850 1050 3450
Wire Wire Line
	1050 3450 2100 3450
Text Label 1600 3450 0    60   ~ 0
PA4
$Comp
L R-RESCUE-multiupbox R31
U 1 1 5503FC08
P 8850 3850
F 0 "R31" V 8930 3850 50  0000 C CNN
F 1 "10k" V 8857 3851 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 8780 3850 30  0001 C CNN
F 3 "" H 8850 3850 30  0000 C CNN
	1    8850 3850
	0    1    1    0   
$EndComp
$Comp
L R-RESCUE-multiupbox R32
U 1 1 5503FC57
P 9450 3850
F 0 "R32" V 9530 3850 50  0000 C CNN
F 1 "10k" V 9457 3851 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 9380 3850 30  0001 C CNN
F 3 "" H 9450 3850 30  0000 C CNN
	1    9450 3850
	0    1    1    0   
$EndComp
Wire Wire Line
	9100 3850 9200 3850
Wire Wire Line
	9800 3850 9700 3850
$Comp
L SW_PUSH SW3
U 1 1 5503FF20
P 9500 4150
F 0 "SW3" H 9650 4260 50  0000 C CNN
F 1 "SW_PUSH" H 9500 4070 50  0000 C CNN
F 2 "Discret:SW_PUSH_SMALL" H 9500 4150 60  0001 C CNN
F 3 "" H 9500 4150 60  0000 C CNN
	1    9500 4150
	1    0    0    -1  
$EndComp
Wire Wire Line
	9200 3850 9200 4150
$Comp
L +3V3 #PWR090
U 1 1 55040070
P 9900 4150
F 0 "#PWR090" H 9900 4000 60  0001 C CNN
F 1 "+3V3" H 9900 4290 60  0000 C CNN
F 2 "" H 9900 4150 60  0000 C CNN
F 3 "" H 9900 4150 60  0000 C CNN
	1    9900 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	9900 4150 9800 4150
Text Label 1450 5200 0    60   ~ 0
PB12
$Comp
L CONN_01X04 P16
U 1 1 5506ED85
P 1500 4250
F 0 "P16" H 1500 4500 50  0000 C CNN
F 1 "CONN_01X04" V 1600 4250 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x04" H 1500 4250 60  0001 C CNN
F 3 "" H 1500 4250 60  0000 C CNN
	1    1500 4250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR091
U 1 1 5506EF34
P 1100 4400
F 0 "#PWR091" H 1100 4150 60  0001 C CNN
F 1 "GND" H 1100 4250 60  0000 C CNN
F 2 "" H 1100 4400 60  0000 C CNN
F 3 "" H 1100 4400 60  0000 C CNN
	1    1100 4400
	0    1    1    0   
$EndComp
Wire Wire Line
	1100 4400 1300 4400
Wire Wire Line
	1250 4300 1300 4300
Wire Wire Line
	1250 4300 1250 5200
Text Notes 1300 3950 0    60   ~ 0
DCF77-Modul (Pollin)
$Comp
L +3V3 #PWR092
U 1 1 5506FC59
P 1100 4100
F 0 "#PWR092" H 1100 3950 60  0001 C CNN
F 1 "+3V3" H 1100 4240 60  0000 C CNN
F 2 "" H 1100 4100 60  0000 C CNN
F 3 "" H 1100 4100 60  0000 C CNN
	1    1100 4100
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1300 4100 1100 4100
Wire Wire Line
	1300 4200 1200 4200
Wire Wire Line
	1200 4200 1200 4400
Connection ~ 1200 4400
$Comp
L CONN_01X02 P3
U 1 1 5509B846
P 9250 2300
F 0 "P3" H 9250 2450 50  0000 C CNN
F 1 "CONN_01X02" V 9350 2300 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02" H 9250 2300 60  0001 C CNN
F 3 "" H 9250 2300 60  0000 C CNN
	1    9250 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9050 2200 9050 2250
Wire Wire Line
	9050 2400 9050 2350
$Comp
L STM32F103CB U?
U 1 1 55D23170
P 5800 3250
F 0 "U?" H 4500 4900 50  0000 C CNN
F 1 "STM32F103CB" H 6850 1600 50  0000 C CNN
F 2 "LQFP48" H 5800 3250 50  0000 C CNN
F 3 "" H 5800 3250 60  0000 C CNN
	1    5800 3250
	1    0    0    -1  
$EndComp
$EndSCHEMATC