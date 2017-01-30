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
LIBS:sensactHsMini-cache
EELAYER 25 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 4
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
L STM32F407VETx U1
U 1 1 5884A419
P 5900 3800
F 0 "U1" H 1600 6625 50  0000 L BNN
F 1 "STM32F407VETx" H 10200 6625 50  0000 R BNN
F 2 "sensact:LQFP-100_14x14mm_Pitch0.5mm_hnadsolder" H 10200 6575 50  0000 R TNN
F 3 "" H 5900 3800 50  0000 C CNN
	1    5900 3800
	1    0    0    -1  
$EndComp
Wire Wire Line
	10400 5000 10850 5000
Wire Wire Line
	10400 4200 10850 4200
Wire Wire Line
	10400 4300 10850 4300
Wire Wire Line
	10400 4900 10850 4900
Wire Wire Line
	10400 3800 10850 3800
Wire Wire Line
	10400 4100 10850 4100
Wire Wire Line
	10400 1400 10850 1400
Wire Wire Line
	10400 5100 10850 5100
Wire Wire Line
	10400 5200 10850 5200
Wire Wire Line
	10400 1500 10850 1500
Wire Wire Line
	10400 2000 10850 2000
Wire Wire Line
	10400 2400 10850 2400
Wire Wire Line
	10400 2500 10850 2500
Wire Wire Line
	10400 2600 10850 2600
Wire Wire Line
	10400 2700 10850 2700
Wire Wire Line
	10400 3000 10850 3000
Wire Wire Line
	10400 3100 10850 3100
Wire Wire Line
	10400 3200 10850 3200
Wire Wire Line
	10400 4000 10850 4000
Text Label 10600 4900 0    60   ~ 0
TXD2
Text Label 10600 5000 0    60   ~ 0
TXC
Text Label 10600 5100 0    60   ~ 0
RXD0
Text Label 10600 5200 0    60   ~ 0
RXD1
Text Label 10600 4300 0    60   ~ 0
TXD1
Text Label 10600 4200 0    60   ~ 0
TXD0
Text Label 10600 4100 0    60   ~ 0
TXEN
Text Label 10600 4000 0    60   ~ 0
RXER
Text Label 10600 3800 0    60   ~ 0
TXD3
Text Label 10600 3100 0    60   ~ 0
RXD3
Text Label 10600 3000 0    60   ~ 0
RXD2
Text Label 10600 2700 0    60   ~ 0
SWCLK
Text Label 10600 2600 0    60   ~ 0
SWDIO
Text Label 10600 2500 0    60   ~ 0
USBP
Text Label 10600 2400 0    60   ~ 0
USBM
Text Label 10600 2000 0    60   ~ 0
RXDV
Text Label 10600 1500 0    60   ~ 0
MDIO
Text Label 10600 1400 0    60   ~ 0
RXC
Text Label 10600 3200 0    60   ~ 0
BOOT1
Wire Wire Line
	10400 1300 10850 1300
Text Label 10600 1300 0    60   ~ 0
CRS
Wire Wire Line
	10400 1600 10850 1600
Text Label 10600 1600 0    60   ~ 0
COL
Wire Wire Line
	10400 4800 10850 4800
Text Label 10600 4800 0    60   ~ 0
MDC
Wire Wire Line
	10400 2100 10850 2100
Wire Wire Line
	10400 2200 10850 2200
Wire Wire Line
	10400 2300 10850 2300
Text Label 10600 2200 0    60   ~ 0
TX
Text Label 10600 2300 0    60   ~ 0
RX
Wire Wire Line
	10400 3700 10850 3700
Wire Wire Line
	10400 3600 10850 3600
Text Label 10600 3600 0    60   ~ 0
SCL1
Text Label 10600 3700 0    60   ~ 0
SDA1
$Comp
L +3V3 #PWR01
U 1 1 588527C2
P 800 6850
F 0 "#PWR01" H 800 6700 50  0001 C CNN
F 1 "+3V3" H 800 6990 50  0000 C CNN
F 2 "" H 800 6850 50  0000 C CNN
F 3 "" H 800 6850 50  0000 C CNN
	1    800  6850
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR02
U 1 1 588527D8
P 850 7350
F 0 "#PWR02" H 850 7100 50  0001 C CNN
F 1 "GND" H 850 7200 50  0000 C CNN
F 2 "" H 850 7350 50  0000 C CNN
F 3 "" H 850 7350 50  0000 C CNN
	1    850  7350
	1    0    0    -1  
$EndComp
$Comp
L C C3
U 1 1 588527F7
P 800 7050
F 0 "C3" H 825 7150 50  0000 L CNN
F 1 "10u" H 825 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 838 6900 50  0001 C CNN
F 3 "" H 800 7050 50  0000 C CNN
	1    800  7050
	1    0    0    -1  
$EndComp
$Comp
L C C4
U 1 1 58852881
P 1000 7050
F 0 "C4" H 1025 7150 50  0000 L CNN
F 1 "u1" H 1025 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1038 6900 50  0001 C CNN
F 3 "" H 1000 7050 50  0000 C CNN
	1    1000 7050
	1    0    0    -1  
$EndComp
$Comp
L C C6
U 1 1 5885289A
P 1200 7050
F 0 "C6" H 1225 7150 50  0000 L CNN
F 1 "u1" H 1225 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1238 6900 50  0001 C CNN
F 3 "" H 1200 7050 50  0000 C CNN
	1    1200 7050
	1    0    0    -1  
$EndComp
$Comp
L C C8
U 1 1 588528BD
P 1400 7050
F 0 "C8" H 1425 7150 50  0000 L CNN
F 1 "u1" H 1425 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1438 6900 50  0001 C CNN
F 3 "" H 1400 7050 50  0000 C CNN
	1    1400 7050
	1    0    0    -1  
$EndComp
$Comp
L C C9
U 1 1 588528DC
P 1600 7050
F 0 "C9" H 1625 7150 50  0000 L CNN
F 1 "u1" H 1625 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1638 6900 50  0001 C CNN
F 3 "" H 1600 7050 50  0000 C CNN
	1    1600 7050
	1    0    0    -1  
$EndComp
$Comp
L C C10
U 1 1 588528FB
P 1800 7050
F 0 "C10" H 1825 7150 50  0000 L CNN
F 1 "u1" H 1825 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1838 6900 50  0001 C CNN
F 3 "" H 1800 7050 50  0000 C CNN
	1    1800 7050
	1    0    0    -1  
$EndComp
$Comp
L C C11
U 1 1 5885291E
P 2000 7050
F 0 "C11" H 2025 7150 50  0000 L CNN
F 1 "u1" H 2025 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2038 6900 50  0001 C CNN
F 3 "" H 2000 7050 50  0000 C CNN
	1    2000 7050
	1    0    0    -1  
$EndComp
Wire Wire Line
	800  7200 2950 7200
Connection ~ 1800 7200
Connection ~ 1600 7200
Connection ~ 1400 7200
Connection ~ 1200 7200
Connection ~ 1000 7200
Wire Wire Line
	850  7350 850  7200
Connection ~ 850  7200
Wire Wire Line
	800  6900 2200 6900
Connection ~ 1000 6900
Connection ~ 1200 6900
Connection ~ 1400 6900
Connection ~ 1600 6900
Connection ~ 1800 6900
$Comp
L Ferrite_Bead L1
U 1 1 58852C1C
P 2350 6900
F 0 "L1" V 2200 6925 50  0000 C CNN
F 1 "Ferrite_Bead" V 2500 6900 50  0000 C CNN
F 2 "Resistors_SMD:R_1206_HandSoldering" V 2280 6900 50  0001 C CNN
F 3 "" H 2350 6900 50  0000 C CNN
	1    2350 6900
	0    1    1    0   
$EndComp
$Comp
L C C12
U 1 1 58852C5D
P 2750 7050
F 0 "C12" H 2775 7150 50  0000 L CNN
F 1 "1u" H 2775 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2788 6900 50  0001 C CNN
F 3 "" H 2750 7050 50  0000 C CNN
	1    2750 7050
	1    0    0    -1  
$EndComp
$Comp
L C C13
U 1 1 58852CA7
P 2950 7050
F 0 "C13" H 2975 7150 50  0000 L CNN
F 1 "u1" H 2975 6950 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 2988 6900 50  0001 C CNN
F 3 "" H 2950 7050 50  0000 C CNN
	1    2950 7050
	1    0    0    -1  
$EndComp
Connection ~ 2000 6900
Wire Wire Line
	2500 6900 2950 6900
Connection ~ 2750 6900
Connection ~ 2000 7200
Connection ~ 2750 7200
Wire Wire Line
	6200 800  6650 800 
Text Label 6350 800  0    60   ~ 0
VDDA
Text Label 2900 6900 0    60   ~ 0
VDDA
Wire Wire Line
	1400 1800 950  1800
Text Label 950  1800 0    60   ~ 0
VDDA
$Comp
L C C5
U 1 1 58853193
P 1050 2150
F 0 "C5" H 1075 2250 50  0000 L CNN
F 1 "2u2" H 1075 2050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1088 2000 50  0001 C CNN
F 3 "" H 1050 2150 50  0000 C CNN
	1    1050 2150
	1    0    0    -1  
$EndComp
$Comp
L C C7
U 1 1 588531C2
P 1250 2150
F 0 "C7" H 1275 2250 50  0000 L CNN
F 1 "2u2" H 1275 2050 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1288 2000 50  0001 C CNN
F 3 "" H 1250 2150 50  0000 C CNN
	1    1250 2150
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 2000 1250 2000
Wire Wire Line
	1400 1900 1050 1900
Wire Wire Line
	1050 1900 1050 2000
$Comp
L GND #PWR03
U 1 1 588532A0
P 1150 2300
F 0 "#PWR03" H 1150 2050 50  0001 C CNN
F 1 "GND" H 1150 2150 50  0000 C CNN
F 2 "" H 1150 2300 50  0000 C CNN
F 3 "" H 1150 2300 50  0000 C CNN
	1    1150 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	600  2300 1250 2300
Connection ~ 1150 2300
$Comp
L +3V3 #PWR04
U 1 1 5885335A
P 1300 1700
F 0 "#PWR04" H 1300 1550 50  0001 C CNN
F 1 "+3V3" H 1300 1840 50  0000 C CNN
F 2 "" H 1300 1700 50  0000 C CNN
F 3 "" H 1300 1700 50  0000 C CNN
	1    1300 1700
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 1700 1300 1700
Wire Wire Line
	1400 1300 1000 1300
Text Label 1000 1300 0    60   ~ 0
RST
Wire Wire Line
	1400 4700 900  4700
Wire Wire Line
	1400 4800 900  4800
Text Label 900  4700 0    60   ~ 0
CANR
Text Label 900  4800 0    60   ~ 0
CANT
Wire Wire Line
	5550 800  6100 800 
Connection ~ 5700 800 
Connection ~ 5800 800 
Connection ~ 5900 800 
Connection ~ 6000 800 
$Comp
L +3V3 #PWR05
U 1 1 58853F25
P 5550 800
F 0 "#PWR05" H 5550 650 50  0001 C CNN
F 1 "+3V3" H 5550 940 50  0000 C CNN
F 2 "" H 5550 800 50  0000 C CNN
F 3 "" H 5550 800 50  0000 C CNN
	1    5550 800 
	1    0    0    -1  
$EndComp
Connection ~ 5600 800 
Wire Wire Line
	5650 6700 6100 6700
Connection ~ 5800 6700
Connection ~ 5900 6700
Connection ~ 6000 6700
$Comp
L GND #PWR06
U 1 1 5885425E
P 5650 6700
F 0 "#PWR06" H 5650 6450 50  0001 C CNN
F 1 "GND" H 5650 6550 50  0000 C CNN
F 2 "" H 5650 6700 50  0000 C CNN
F 3 "" H 5650 6700 50  0000 C CNN
	1    5650 6700
	1    0    0    -1  
$EndComp
Connection ~ 5700 6700
Text Label 10600 2100 0    60   ~ 0
SCL2
Text Label 10450 5600 0    60   ~ 0
SDA2
Wire Wire Line
	10400 5600 10850 5600
$Comp
L Crystal Y1
U 1 1 58855A14
P 1150 2750
F 0 "Y1" H 1150 2900 50  0000 C CNN
F 1 "8MHz" H 1150 2600 50  0000 C CNN
F 2 "Crystals:Crystal_HC49-U_Vertical" H 1150 2750 50  0001 C CNN
F 3 "" H 1150 2750 50  0000 C CNN
	1    1150 2750
	0    1    1    0   
$EndComp
Wire Wire Line
	1400 2600 1400 2700
Wire Wire Line
	900  2600 1400 2600
Wire Wire Line
	1400 2900 1400 2800
Wire Wire Line
	900  2900 1400 2900
$Comp
L C C1
U 1 1 58855B04
P 750 2600
F 0 "C1" H 775 2700 50  0000 L CNN
F 1 "22p" H 775 2500 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 788 2450 50  0001 C CNN
F 3 "" H 750 2600 50  0000 C CNN
	1    750  2600
	0    1    1    0   
$EndComp
$Comp
L C C2
U 1 1 58855B43
P 750 2900
F 0 "C2" H 775 3000 50  0000 L CNN
F 1 "22p" H 775 2800 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 788 2750 50  0001 C CNN
F 3 "" H 750 2900 50  0000 C CNN
	1    750  2900
	0    1    1    0   
$EndComp
Connection ~ 1150 2600
Connection ~ 1150 2900
Wire Wire Line
	600  2300 600  2900
Connection ~ 1050 2300
Connection ~ 600  2600
Text GLabel 900  4700 0    60   Input ~ 0
CANR
Text GLabel 900  4800 0    60   Output ~ 0
CANT
Text GLabel 10850 1300 2    60   BiDi ~ 0
CRS
Text GLabel 10850 1400 2    60   BiDi ~ 0
RXC
Text GLabel 10850 1500 2    60   BiDi ~ 0
MDIO
Text GLabel 10850 1600 2    60   BiDi ~ 0
COL
Text GLabel 10850 2000 2    60   BiDi ~ 0
RXDV
Text GLabel 10850 2100 2    60   Output ~ 0
SCL2
Text GLabel 10850 2200 2    60   Output ~ 0
TX1
Text GLabel 10850 2300 2    60   Input ~ 0
RX1
Text GLabel 10850 2400 2    60   BiDi ~ 0
USBM
Text GLabel 10850 2500 2    60   BiDi ~ 0
USBP
Text GLabel 10850 2600 2    60   BiDi ~ 0
SWDIO
Text GLabel 10850 2700 2    60   BiDi ~ 0
SWCLK
Text GLabel 10850 3000 2    60   BiDi ~ 0
RXD2
Text GLabel 10850 3100 2    60   BiDi ~ 0
RXD3
Text GLabel 10850 3200 2    60   BiDi ~ 0
BOOT1
Text GLabel 10850 3600 2    60   Output ~ 0
SCL1
Text GLabel 10850 3700 2    60   BiDi ~ 0
SDA1
Text GLabel 10850 3800 2    60   BiDi ~ 0
TXD3
Text GLabel 10850 4000 2    60   BiDi ~ 0
RXER
Text GLabel 10850 4100 2    60   BiDi ~ 0
TXEN
Text GLabel 10850 4200 2    60   BiDi ~ 0
TXD0
Text GLabel 10850 4300 2    60   BiDi ~ 0
TXD1
Text GLabel 10850 4800 2    60   BiDi ~ 0
MDC
Text GLabel 10850 4900 2    60   BiDi ~ 0
TXD2
Text GLabel 10850 5000 2    60   BiDi ~ 0
TXC
Text GLabel 10850 5100 2    60   BiDi ~ 0
RXD0
Text GLabel 10850 5200 2    60   BiDi ~ 0
RXD1
Text GLabel 10850 5600 2    60   BiDi ~ 0
SDA2
Wire Wire Line
	800  6900 800  6850
Text GLabel 10500 5700 2    60   Output ~ 0
TX2
Text GLabel 10500 5800 2    60   Input ~ 0
RX2
Wire Wire Line
	10500 5700 10400 5700
Wire Wire Line
	10500 5800 10400 5800
Text GLabel 1000 1300 0    60   Input ~ 0
RST
Wire Wire Line
	10400 3300 10850 3300
Wire Wire Line
	10400 3400 10850 3400
Wire Wire Line
	10400 3500 10850 3500
Text GLabel 10850 3300 2    60   BiDi ~ 0
INT1_3
Text GLabel 10850 3400 2    60   BiDi ~ 0
INT1_2
Text GLabel 10850 3500 2    60   BiDi ~ 0
INT1_1
Wire Wire Line
	1400 4900 1300 4900
Wire Wire Line
	1400 5000 1300 5000
Wire Wire Line
	1400 5100 1300 5100
Text GLabel 1300 4900 0    60   Input ~ 0
INT2_1
Text GLabel 1300 5000 0    60   Input ~ 0
INT2_2
Text GLabel 1300 5100 0    60   Input ~ 0
INT2_3
$EndSCHEMATC
