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
Sheet 4 4
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
L GND #PWR024
U 1 1 58851BF1
P 1100 1600
F 0 "#PWR024" H 1100 1350 50  0001 C CNN
F 1 "GND" H 1100 1450 50  0000 C CNN
F 2 "" H 1100 1600 50  0000 C CNN
F 3 "" H 1100 1600 50  0000 C CNN
	1    1100 1600
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR025
U 1 1 58851C05
P 1100 850
F 0 "#PWR025" H 1100 700 50  0001 C CNN
F 1 "+3V3" H 1100 990 50  0000 C CNN
F 2 "" H 1100 850 50  0000 C CNN
F 3 "" H 1100 850 50  0000 C CNN
	1    1100 850 
	1    0    0    -1  
$EndComp
$Comp
L C C26
U 1 1 58851C19
P 1100 1400
F 0 "C26" H 1125 1500 50  0000 L CNN
F 1 "u1" H 1125 1300 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1138 1250 50  0001 C CNN
F 3 "" H 1100 1400 50  0000 C CNN
	1    1100 1400
	1    0    0    -1  
$EndComp
$Comp
L R R16
U 1 1 58851C4B
P 1100 1050
F 0 "R16" V 1180 1050 50  0000 C CNN
F 1 "4k7" V 1100 1050 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 1030 1050 50  0001 C CNN
F 3 "" H 1100 1050 50  0000 C CNN
	1    1100 1050
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 850  1100 900 
Wire Wire Line
	1100 1200 1100 1250
Wire Wire Line
	1100 1550 1100 1600
$Comp
L SWITCH SW1
U 1 1 58851C91
P 850 1400
F 0 "SW1" V 930 1400 50  0000 C CNN
F 1 "SWITCH" V 750 1400 50  0000 C CNN
F 2 "Buttons_Switches_THT:SW_PUSH_6mm" V 780 1400 30  0001 C CNN
F 3 "" H 850 1400 30  0000 C CNN
	1    850  1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	1100 1600 850  1600
Wire Wire Line
	850  1600 850  1550
Wire Wire Line
	850  1250 1450 1250
Connection ~ 1100 1250
Text Label 1300 1250 0    60   ~ 0
RST
$Comp
L CONN_01X10 P1
U 1 1 58851FED
P 1600 3450
F 0 "P1" H 1600 4000 50  0000 C CNN
F 1 "CONN_01X10" V 1700 3450 50  0000 C CNN
F 2 "Connectors:IDC_Header_Straight_10pins" H 1600 3450 50  0001 C CNN
F 3 "" H 1600 3450 50  0000 C CNN
	1    1600 3450
	1    0    0    -1  
$EndComp
$Comp
L SN65HVD230 U4
U 1 1 5885207A
P 4300 1500
F 0 "U4" H 3900 1850 60  0000 L CNN
F 1 "SN65HVD230" H 3900 1750 60  0000 L CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_handsolder" H 4300 1500 60  0001 C CNN
F 3 "" H 4300 1500 60  0000 C CNN
	1    4300 1500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR026
U 1 1 588520DD
P 3450 1300
F 0 "#PWR026" H 3450 1050 50  0001 C CNN
F 1 "GND" H 3450 1150 50  0000 C CNN
F 2 "" H 3450 1300 50  0000 C CNN
F 3 "" H 3450 1300 50  0000 C CNN
	1    3450 1300
	-1   0    0    1   
$EndComp
$Comp
L +3V3 #PWR027
U 1 1 588520FB
P 3450 1700
F 0 "#PWR027" H 3450 1550 50  0001 C CNN
F 1 "+3V3" H 3450 1840 50  0000 C CNN
F 2 "" H 3450 1700 50  0000 C CNN
F 3 "" H 3450 1700 50  0000 C CNN
	1    3450 1700
	-1   0    0    1   
$EndComp
$Comp
L C C27
U 1 1 58852132
P 3450 1500
F 0 "C27" H 3475 1600 50  0000 L CNN
F 1 "u1" H 3475 1400 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3488 1350 50  0001 C CNN
F 3 "" H 3450 1500 50  0000 C CNN
	1    3450 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3450 1300 3450 1350
Wire Wire Line
	3450 1700 3450 1650
Wire Wire Line
	3700 1550 3600 1550
Wire Wire Line
	3600 1550 3600 1650
Wire Wire Line
	3600 1650 3450 1650
Wire Wire Line
	3700 1450 3600 1450
Wire Wire Line
	3600 1450 3600 1350
Wire Wire Line
	3600 1350 3300 1350
$Comp
L R R17
U 1 1 588521C3
P 4250 950
F 0 "R17" V 4330 950 50  0000 C CNN
F 1 "10k" V 4250 950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4180 950 50  0001 C CNN
F 3 "" H 4250 950 50  0000 C CNN
	1    4250 950 
	0    1    1    0   
$EndComp
Wire Wire Line
	4900 1350 4900 950 
Wire Wire Line
	4900 950  4400 950 
Wire Wire Line
	4100 950  3300 950 
Wire Wire Line
	3300 950  3300 1350
Connection ~ 3450 1350
Wire Wire Line
	3700 1350 3700 1100
Wire Wire Line
	3700 1100 3350 1100
Wire Wire Line
	3700 1650 3700 2000
Wire Wire Line
	3700 2000 3350 2000
Text Label 3350 1100 0    60   ~ 0
CANT
Text Label 3350 2000 0    60   ~ 0
CANR
NoConn ~ 4900 1650
Wire Wire Line
	4900 1450 5300 1450
Wire Wire Line
	4900 1550 5300 1550
Text Label 4950 1450 0    60   ~ 0
CANH
Text Label 4950 1550 0    60   ~ 0
CANL
$Comp
L LMR14006 U5
U 1 1 58852743
P 4300 2700
F 0 "U5" H 4100 3000 60  0000 L CNN
F 1 "LMR14006" H 4100 2900 60  0000 L CNN
F 2 "sensact:SOT-23-6_handsolder" H 4300 2700 60  0001 C CNN
F 3 "" H 4300 2700 60  0000 C CNN
	1    4300 2700
	1    0    0    -1  
$EndComp
$Comp
L C C31
U 1 1 5885277E
P 4850 2450
F 0 "C31" H 4875 2550 50  0000 L CNN
F 1 "u15" H 4875 2350 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 4888 2300 50  0001 C CNN
F 3 "" H 4850 2450 50  0000 C CNN
	1    4850 2450
	0    1    1    0   
$EndComp
$Comp
L R R18
U 1 1 588527B7
P 4750 2950
F 0 "R18" V 4830 2950 50  0000 C CNN
F 1 "1k" V 4750 2950 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4680 2950 50  0001 C CNN
F 3 "" H 4750 2950 50  0000 C CNN
	1    4750 2950
	1    0    0    -1  
$EndComp
$Comp
L R R19
U 1 1 588527F6
P 4950 2800
F 0 "R19" V 5030 2800 50  0000 C CNN
F 1 "3k3" V 4950 2800 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 4880 2800 50  0001 C CNN
F 3 "" H 4950 2800 50  0000 C CNN
	1    4950 2800
	0    1    1    0   
$EndComp
Wire Wire Line
	4700 2700 5000 2700
Wire Wire Line
	5000 2700 5000 2450
Wire Wire Line
	4700 2600 4700 2450
Wire Wire Line
	4700 2800 4800 2800
Connection ~ 4750 2800
Wire Wire Line
	3900 3100 3900 2800
$Comp
L SCHOTTKY D1
U 1 1 5885292A
P 5200 2650
F 0 "D1" H 5200 2750 40  0000 C CNN
F 1 "SCHOTTKY" H 5200 2550 40  0000 C CNN
F 2 "sensact:do214ac_handsolder" H 5200 2650 60  0001 C CNN
F 3 "" H 5200 2650 60  0000 C CNN
	1    5200 2650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	5000 2450 5250 2450
Wire Wire Line
	5200 2450 5200 2550
Wire Wire Line
	5200 3100 5200 2700
Connection ~ 4750 3100
$Comp
L L L3
U 1 1 588529ED
P 5400 2450
F 0 "L3" V 5350 2450 50  0000 C CNN
F 1 "L" V 5475 2450 50  0000 C CNN
F 2 "sensact:Inductor_CD104_handsolder" H 5400 2450 50  0001 C CNN
F 3 "" H 5400 2450 50  0000 C CNN
	1    5400 2450
	0    1    1    0   
$EndComp
Connection ~ 5200 2450
Wire Wire Line
	5550 2800 5550 2450
$Comp
L C C32
U 1 1 58852B53
P 5350 2950
F 0 "C32" H 5375 3050 50  0000 L CNN
F 1 "10u" H 5375 2850 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 5388 2800 50  0001 C CNN
F 3 "" H 5350 2950 50  0000 C CNN
	1    5350 2950
	1    0    0    -1  
$EndComp
$Comp
L C C33
U 1 1 58852BCA
P 5550 2950
F 0 "C33" H 5575 3050 50  0000 L CNN
F 1 "m1" H 5575 2850 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 5588 2800 50  0001 C CNN
F 3 "" H 5550 2950 50  0000 C CNN
	1    5550 2950
	1    0    0    -1  
$EndComp
Connection ~ 5200 3100
Connection ~ 5550 2800
Wire Wire Line
	5100 2800 5650 2800
Connection ~ 5350 2800
Wire Wire Line
	3550 3100 5550 3100
Connection ~ 5350 3100
$Comp
L +3V3 #PWR028
U 1 1 58852F2A
P 5650 2800
F 0 "#PWR028" H 5650 2650 50  0001 C CNN
F 1 "+3V3" H 5650 2940 50  0000 C CNN
F 2 "" H 5650 2800 50  0000 C CNN
F 3 "" H 5650 2800 50  0000 C CNN
	1    5650 2800
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2600 3900 2700
Wire Wire Line
	3550 2600 3900 2600
$Comp
L +24V #PWR029
U 1 1 5885316E
P 3850 2600
F 0 "#PWR029" H 3850 2450 50  0001 C CNN
F 1 "+24V" H 3850 2740 50  0000 C CNN
F 2 "" H 3850 2600 50  0000 C CNN
F 3 "" H 3850 2600 50  0000 C CNN
	1    3850 2600
	1    0    0    -1  
$EndComp
$Comp
L C C30
U 1 1 588531DC
P 3750 2850
F 0 "C30" H 3775 2950 50  0000 L CNN
F 1 "4u7" H 3775 2750 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3788 2700 50  0001 C CNN
F 3 "" H 3750 2850 50  0000 C CNN
	1    3750 2850
	1    0    0    -1  
$EndComp
$Comp
L C C28
U 1 1 58853257
P 3550 2850
F 0 "C28" H 3575 2950 50  0000 L CNN
F 1 "4u7" H 3575 2750 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3588 2700 50  0001 C CNN
F 3 "" H 3550 2850 50  0000 C CNN
	1    3550 2850
	1    0    0    -1  
$EndComp
Wire Wire Line
	3750 3100 3750 3000
Connection ~ 3900 3100
Wire Wire Line
	3550 3100 3550 3000
Connection ~ 3750 3100
Wire Wire Line
	3750 2600 3750 2700
Connection ~ 3850 2600
Wire Wire Line
	3550 2600 3550 2700
Connection ~ 3750 2600
$Comp
L CONN_01X08 P2
U 1 1 588534B9
P 1600 4600
F 0 "P2" H 1600 5050 50  0000 C CNN
F 1 "CONN_01X08" V 1700 4600 50  0000 C CNN
F 2 "sensact:KF141R-8p" H 1600 4600 50  0001 C CNN
F 3 "" H 1600 4600 50  0000 C CNN
	1    1600 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	1400 4650 1000 4650
Wire Wire Line
	1400 4750 1000 4750
Wire Wire Line
	1400 4850 1150 4850
Wire Wire Line
	1400 2500 1400 3000
Wire Wire Line
	1200 3100 1400 3100
Wire Wire Line
	800  3200 1400 3200
Wire Wire Line
	1400 3300 1350 3300
Wire Wire Line
	1400 3400 1350 3400
Wire Wire Line
	1400 3500 1350 3500
Wire Wire Line
	1400 3600 1000 3600
Wire Wire Line
	1000 3700 1400 3700
Wire Wire Line
	1400 3800 1000 3800
Wire Wire Line
	1400 3900 1000 3900
$Comp
L +24V #PWR030
U 1 1 58853C34
P 1350 4250
F 0 "#PWR030" H 1350 4100 50  0001 C CNN
F 1 "+24V" H 1350 4390 50  0000 C CNN
F 2 "" H 1350 4250 50  0000 C CNN
F 3 "" H 1350 4250 50  0000 C CNN
	1    1350 4250
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR031
U 1 1 58853C6C
P 1000 4350
F 0 "#PWR031" H 1000 4100 50  0001 C CNN
F 1 "GND" H 1000 4200 50  0000 C CNN
F 2 "" H 1000 4350 50  0000 C CNN
F 3 "" H 1000 4350 50  0000 C CNN
	1    1000 4350
	0    1    1    0   
$EndComp
$Comp
L +3V3 #PWR032
U 1 1 58853CA4
P 1150 4850
F 0 "#PWR032" H 1150 4700 50  0001 C CNN
F 1 "+3V3" H 1150 4990 50  0000 C CNN
F 2 "" H 1150 4850 50  0000 C CNN
F 3 "" H 1150 4850 50  0000 C CNN
	1    1150 4850
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR033
U 1 1 58853CDC
P 1300 4950
F 0 "#PWR033" H 1300 4800 50  0001 C CNN
F 1 "+5V" H 1300 5090 50  0000 C CNN
F 2 "" H 1300 4950 50  0000 C CNN
F 3 "" H 1300 4950 50  0000 C CNN
	1    1300 4950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1400 4950 1300 4950
Wire Wire Line
	1400 4350 1000 4350
Wire Wire Line
	1000 4550 1400 4550
Text Label 1000 4450 0    60   ~ 0
CANH
Text Label 1050 4650 0    60   ~ 0
CANL
$Comp
L DS2482 U6
U 1 1 58856394
P 4450 4750
F 0 "U6" H 4050 5100 60  0000 L CNN
F 1 "DS2482" H 4050 5000 60  0000 L CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_handsolder" H 4450 4750 60  0001 C CNN
F 3 "" H 4450 4750 60  0000 C CNN
	1    4450 4750
	1    0    0    -1  
$EndComp
$Comp
L C C29
U 1 1 58856608
P 3700 4700
F 0 "C29" H 3725 4800 50  0000 L CNN
F 1 "C" H 3725 4600 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 3738 4550 50  0001 C CNN
F 3 "" H 3700 4700 50  0000 C CNN
	1    3700 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3850 4600 3850 4550
Wire Wire Line
	3850 4550 3700 4550
Wire Wire Line
	3850 4800 3850 4850
Wire Wire Line
	3850 4850 3700 4850
Wire Wire Line
	3850 4700 3350 4700
NoConn ~ 5050 4800
Wire Wire Line
	5050 4900 5500 4900
Wire Wire Line
	3850 4900 3850 5000
Wire Wire Line
	3850 5000 5500 5000
$Comp
L GND #PWR034
U 1 1 58856A6B
P 3700 4900
F 0 "#PWR034" H 3700 4650 50  0001 C CNN
F 1 "GND" H 3700 4750 50  0000 C CNN
F 2 "" H 3700 4900 50  0000 C CNN
F 3 "" H 3700 4900 50  0000 C CNN
	1    3700 4900
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR035
U 1 1 58856AA7
P 3700 4500
F 0 "#PWR035" H 3700 4350 50  0001 C CNN
F 1 "+3V3" H 3700 4640 50  0000 C CNN
F 2 "" H 3700 4500 50  0000 C CNN
F 3 "" H 3700 4500 50  0000 C CNN
	1    3700 4500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 4550 3700 4500
Wire Wire Line
	3700 4850 3700 4900
$Comp
L +3V3 #PWR036
U 1 1 58856BB4
P 5100 4600
F 0 "#PWR036" H 5100 4450 50  0001 C CNN
F 1 "+3V3" H 5100 4740 50  0000 C CNN
F 2 "" H 5100 4600 50  0000 C CNN
F 3 "" H 5100 4600 50  0000 C CNN
	1    5100 4600
	1    0    0    -1  
$EndComp
Wire Wire Line
	5050 4700 5100 4700
Wire Wire Line
	5100 4700 5100 4600
Wire Wire Line
	5100 4600 5050 4600
Text Label 3350 4700 0    60   ~ 0
1Wa
$Comp
L +3V3 #PWR037
U 1 1 588571C5
P 1400 2500
F 0 "#PWR037" H 1400 2350 50  0001 C CNN
F 1 "+3V3" H 1400 2640 50  0000 C CNN
F 2 "" H 1400 2500 50  0000 C CNN
F 3 "" H 1400 2500 50  0000 C CNN
	1    1400 2500
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR038
U 1 1 58857201
P 1000 3900
F 0 "#PWR038" H 1000 3650 50  0001 C CNN
F 1 "GND" H 1000 3750 50  0000 C CNN
F 2 "" H 1000 3900 50  0000 C CNN
F 3 "" H 1000 3900 50  0000 C CNN
	1    1000 3900
	1    0    0    -1  
$EndComp
Wire Wire Line
	1000 3900 1000 3800
$Comp
L +24V #PWR039
U 1 1 588572DF
P 1000 3650
F 0 "#PWR039" H 1000 3500 50  0001 C CNN
F 1 "+24V" H 1000 3790 50  0000 C CNN
F 2 "" H 1000 3650 50  0000 C CNN
F 3 "" H 1000 3650 50  0000 C CNN
	1    1000 3650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	1000 3600 1000 3700
Connection ~ 1000 3650
$Comp
L CONN_01X10 P3
U 1 1 5887D0C8
P 2900 3450
F 0 "P3" H 2900 4000 50  0000 C CNN
F 1 "CONN_01X10" V 3000 3450 50  0000 C CNN
F 2 "Connectors:IDC_Header_Straight_10pins" H 2900 3450 50  0001 C CNN
F 3 "" H 2900 3450 50  0000 C CNN
	1    2900 3450
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR040
U 1 1 5887D11F
P 2550 3900
F 0 "#PWR040" H 2550 3650 50  0001 C CNN
F 1 "GND" H 2550 3750 50  0000 C CNN
F 2 "" H 2550 3900 50  0000 C CNN
F 3 "" H 2550 3900 50  0000 C CNN
	1    2550 3900
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR041
U 1 1 5887D161
P 2550 3700
F 0 "#PWR041" H 2550 3550 50  0001 C CNN
F 1 "+24V" H 2550 3840 50  0000 C CNN
F 2 "" H 2550 3700 50  0000 C CNN
F 3 "" H 2550 3700 50  0000 C CNN
	1    2550 3700
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR042
U 1 1 5887D1F2
P 2700 2500
F 0 "#PWR042" H 2700 2350 50  0001 C CNN
F 1 "+3V3" H 2700 2640 50  0000 C CNN
F 2 "" H 2700 2500 50  0000 C CNN
F 3 "" H 2700 2500 50  0000 C CNN
	1    2700 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2700 2500 2700 3000
Wire Wire Line
	2500 3100 2700 3100
Wire Wire Line
	2050 3200 2700 3200
Wire Wire Line
	2700 3300 2650 3300
Wire Wire Line
	2700 3400 2650 3400
Wire Wire Line
	2700 3500 2650 3500
Wire Wire Line
	2700 3600 2700 3700
Wire Wire Line
	2700 3700 2550 3700
Wire Wire Line
	2700 3900 2700 3800
Wire Wire Line
	2700 3900 2550 3900
Text GLabel 3350 2000 0    60   BiDi ~ 0
CANR
Text GLabel 3350 1100 0    60   BiDi ~ 0
CANT
$Comp
L GND #PWR043
U 1 1 5887E227
P 4500 3150
F 0 "#PWR043" H 4500 2900 50  0001 C CNN
F 1 "GND" H 4500 3000 50  0000 C CNN
F 2 "" H 4500 3150 50  0000 C CNN
F 3 "" H 4500 3150 50  0000 C CNN
	1    4500 3150
	1    0    0    -1  
$EndComp
Wire Wire Line
	4500 3100 4500 3150
Connection ~ 4500 3100
Text Notes 4000 6250 0    60   ~ 0
MP3-Modul\nVerstärker\nBananaPI\n5V-Supply\n12V-Supply
$Comp
L Jumper JP1
U 1 1 588915EB
P 5600 1350
F 0 "JP1" H 5600 1500 50  0000 C CNN
F 1 "Jumper" H 5600 1270 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 5600 1350 50  0001 C CNN
F 3 "" H 5600 1350 50  0000 C CNN
	1    5600 1350
	1    0    0    -1  
$EndComp
$Comp
L R R20
U 1 1 58891640
P 5600 1650
F 0 "R20" V 5680 1650 50  0000 C CNN
F 1 "120R" V 5600 1650 50  0000 C CNN
F 2 "Resistors_SMD:R_1206_HandSoldering" V 5530 1650 50  0001 C CNN
F 3 "" H 5600 1650 50  0000 C CNN
	1    5600 1650
	0    1    1    0   
$EndComp
Wire Wire Line
	5300 1450 5300 1350
Wire Wire Line
	5900 1350 5900 1650
Wire Wire Line
	5900 1650 5750 1650
Wire Wire Line
	5450 1650 5300 1650
Wire Wire Line
	5300 1650 5300 1550
Wire Wire Line
	1400 4250 1350 4250
Wire Wire Line
	1400 4450 1000 4450
Wire Wire Line
	1000 4450 1000 4550
Wire Wire Line
	1000 4750 1000 4650
$Comp
L DFPlayerMini U7
U 1 1 58892854
P 7300 1800
F 0 "U7" H 6900 2250 60  0000 L CNN
F 1 "DFPlayerMini" H 6900 2350 60  0000 L CNN
F 2 "sensact:DFPlayerMini" H 7300 1800 60  0001 C CNN
F 3 "" H 7300 1800 60  0001 C CNN
	1    7300 1800
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR044
U 1 1 58892AE3
P 6700 1400
F 0 "#PWR044" H 6700 1250 50  0001 C CNN
F 1 "+3V3" H 6700 1540 50  0000 C CNN
F 2 "" H 6700 1400 50  0000 C CNN
F 3 "" H 6700 1400 50  0000 C CNN
	1    6700 1400
	1    0    0    -1  
$EndComp
Wire Wire Line
	6700 1400 6700 1450
Text GLabel 6650 1550 0    60   Input ~ 0
RX2
Text GLabel 6650 1650 0    60   Output ~ 0
TX2
Wire Wire Line
	6700 1550 6650 1550
Wire Wire Line
	6700 1650 6650 1650
$Comp
L CONN_01X03 P6
U 1 1 58893D52
P 6250 1850
F 0 "P6" H 6250 2050 50  0000 C CNN
F 1 "CONN_01X03" V 6350 1850 50  0000 C CNN
F 2 "sensact:KF141R-3p" H 6250 1850 50  0001 C CNN
F 3 "" H 6250 1850 50  0000 C CNN
	1    6250 1850
	-1   0    0    -1  
$EndComp
Wire Wire Line
	6700 2050 6450 2050
Wire Wire Line
	6450 1950 6450 2300
NoConn ~ 6700 1950
NoConn ~ 6700 2150
$Comp
L GND #PWR045
U 1 1 58894071
P 7350 2300
F 0 "#PWR045" H 7350 2050 50  0001 C CNN
F 1 "GND" H 7350 2150 50  0000 C CNN
F 2 "" H 7350 2300 50  0000 C CNN
F 3 "" H 7350 2300 50  0000 C CNN
	1    7350 2300
	1    0    0    -1  
$EndComp
Wire Wire Line
	7900 2050 7950 2050
Wire Wire Line
	7950 2050 7950 2300
Wire Wire Line
	7950 2300 6450 2300
Connection ~ 6450 2050
Connection ~ 7350 2300
NoConn ~ 7900 1450
NoConn ~ 7900 1550
NoConn ~ 7900 1650
NoConn ~ 7900 1750
NoConn ~ 7900 1850
NoConn ~ 7900 1950
NoConn ~ 7900 2150
$Comp
L LM2596Board U8
U 1 1 58894893
P 7600 3150
F 0 "U8" H 7350 3300 60  0000 L CNN
F 1 "LM2596Board" H 7350 3400 60  0000 L CNN
F 2 "sensact:LM2596Board" H 7600 3150 60  0001 C CNN
F 3 "" H 7600 3150 60  0001 C CNN
	1    7600 3150
	1    0    0    -1  
$EndComp
$Comp
L LM2596Board U9
U 1 1 58894BF2
P 7600 3600
F 0 "U9" H 7350 3750 60  0000 L CNN
F 1 "LM2596Board" H 7350 3850 60  0000 L CNN
F 2 "sensact:LM2596Board" H 7600 3600 60  0001 C CNN
F 3 "" H 7600 3600 60  0001 C CNN
	1    7600 3600
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR046
U 1 1 58894C49
P 7000 3650
F 0 "#PWR046" H 7000 3400 50  0001 C CNN
F 1 "GND" H 7000 3500 50  0000 C CNN
F 2 "" H 7000 3650 50  0000 C CNN
F 3 "" H 7000 3650 50  0000 C CNN
	1    7000 3650
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR047
U 1 1 58894C95
P 7000 3200
F 0 "#PWR047" H 7000 2950 50  0001 C CNN
F 1 "GND" H 7000 3050 50  0000 C CNN
F 2 "" H 7000 3200 50  0000 C CNN
F 3 "" H 7000 3200 50  0000 C CNN
	1    7000 3200
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR048
U 1 1 58894CE1
P 7100 3100
F 0 "#PWR048" H 7100 2950 50  0001 C CNN
F 1 "+24V" H 7100 3240 50  0000 C CNN
F 2 "" H 7100 3100 50  0000 C CNN
F 3 "" H 7100 3100 50  0000 C CNN
	1    7100 3100
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR049
U 1 1 58894DDB
P 7100 3550
F 0 "#PWR049" H 7100 3400 50  0001 C CNN
F 1 "+24V" H 7100 3690 50  0000 C CNN
F 2 "" H 7100 3550 50  0000 C CNN
F 3 "" H 7100 3550 50  0000 C CNN
	1    7100 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	7150 3100 7100 3100
Wire Wire Line
	7150 3550 7100 3550
Wire Wire Line
	7000 3650 7150 3650
Wire Wire Line
	7000 3200 7150 3200
Wire Wire Line
	7100 3200 7100 3300
Wire Wire Line
	7100 3300 8000 3300
Wire Wire Line
	8000 3300 8000 3200
Connection ~ 7100 3200
Wire Wire Line
	7100 3650 7100 3750
Wire Wire Line
	7100 3750 8300 3750
Wire Wire Line
	8000 3750 8000 3650
Connection ~ 7100 3650
$Comp
L +5V #PWR050
U 1 1 588952B9
P 8050 3100
F 0 "#PWR050" H 8050 2950 50  0001 C CNN
F 1 "+5V" H 8050 3240 50  0000 C CNN
F 2 "" H 8050 3100 50  0000 C CNN
F 3 "" H 8050 3100 50  0000 C CNN
	1    8050 3100
	1    0    0    -1  
$EndComp
Wire Wire Line
	8000 3100 8300 3100
Connection ~ 8050 3100
Wire Wire Line
	8300 3750 8300 3300
Connection ~ 8000 3750
Wire Wire Line
	8000 3550 8200 3550
Wire Wire Line
	8200 3550 8200 3200
Wire Wire Line
	8200 3200 8300 3200
$Comp
L +12V #PWR051
U 1 1 58895BD4
P 8050 3500
F 0 "#PWR051" H 8050 3350 50  0001 C CNN
F 1 "+12V" H 8050 3640 50  0000 C CNN
F 2 "" H 8050 3500 50  0000 C CNN
F 3 "" H 8050 3500 50  0000 C CNN
	1    8050 3500
	1    0    0    -1  
$EndComp
Wire Wire Line
	8050 3500 8050 3550
Connection ~ 8050 3550
$Comp
L CONN_01X05 P7
U 1 1 58897FBE
P 10000 1350
F 0 "P7" H 10000 1650 50  0000 C CNN
F 1 "CONN_01X05" V 10100 1350 50  0000 C CNN
F 2 "sensact:miniusb_vert" H 10000 1350 50  0001 C CNN
F 3 "" H 10000 1350 50  0000 C CNN
	1    10000 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 1250 9250 1250
$Comp
L GND #PWR052
U 1 1 588981E1
P 9250 1300
F 0 "#PWR052" H 9250 1050 50  0001 C CNN
F 1 "GND" H 9250 1150 50  0000 C CNN
F 2 "" H 9250 1300 50  0000 C CNN
F 3 "" H 9250 1300 50  0000 C CNN
	1    9250 1300
	1    0    0    -1  
$EndComp
Wire Wire Line
	9250 1250 9250 1300
$Comp
L CONN_01X01 P5
U 1 1 588982DC
P 10000 850
F 0 "P5" H 10000 950 50  0000 C CNN
F 1 "CONN_01X01" V 10100 850 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x01_Pitch2.54mm" H 10000 850 50  0001 C CNN
F 3 "" H 10000 850 50  0000 C CNN
	1    10000 850 
	1    0    0    -1  
$EndComp
$Comp
L +3V3 #PWR053
U 1 1 5889833F
P 9800 800
F 0 "#PWR053" H 9800 650 50  0001 C CNN
F 1 "+3V3" H 9800 940 50  0000 C CNN
F 2 "" H 9800 800 50  0000 C CNN
F 3 "" H 9800 800 50  0000 C CNN
	1    9800 800 
	1    0    0    -1  
$EndComp
Wire Wire Line
	9800 800  9800 850 
Text GLabel 9650 1150 0    60   BiDi ~ 0
SWDIO
Text GLabel 9750 1350 0    60   BiDi ~ 0
SWCLK
$Comp
L R R21
U 1 1 5889874E
P 9600 1450
F 0 "R21" V 9680 1450 50  0000 C CNN
F 1 "100R" V 9600 1450 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 9530 1450 50  0001 C CNN
F 3 "" H 9600 1450 50  0000 C CNN
	1    9600 1450
	0    1    1    0   
$EndComp
$Comp
L R R22
U 1 1 588987B1
P 9600 1650
F 0 "R22" V 9680 1650 50  0000 C CNN
F 1 "100R" V 9600 1650 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 9530 1650 50  0001 C CNN
F 3 "" H 9600 1650 50  0000 C CNN
	1    9600 1650
	0    1    1    0   
$EndComp
Wire Wire Line
	9800 1450 9750 1450
Wire Wire Line
	9800 1550 9800 1650
Wire Wire Line
	9800 1650 9750 1650
Wire Wire Line
	9450 1450 9450 1550
Wire Wire Line
	9450 1550 9000 1550
Wire Wire Line
	9450 1650 9000 1650
Text GLabel 9000 1550 0    60   Input ~ 0
TX1
Text GLabel 9000 1650 0    60   Output ~ 0
RX1
Wire Wire Line
	9800 1150 9650 1150
Wire Wire Line
	9800 1350 9750 1350
Text GLabel 1450 1250 2    60   Output ~ 0
RST
Text GLabel 1200 3100 0    60   BiDi ~ 0
SCL1
Text GLabel 800  3200 0    60   BiDi ~ 0
SDA1
Text GLabel 2500 3100 0    60   BiDi ~ 0
SCL2
Text GLabel 2050 3200 0    60   BiDi ~ 0
SDA2
Text GLabel 1350 3300 0    60   BiDi ~ 0
INT1_1
Text GLabel 1350 3400 0    60   BiDi ~ 0
INT1_2
Text GLabel 1350 3500 0    60   BiDi ~ 0
INT1_3
Text GLabel 2650 3300 0    60   Output ~ 0
INT2_1
Text GLabel 2650 3400 0    60   Output ~ 0
INT2_2
Text GLabel 2650 3500 0    60   Output ~ 0
INT2_3
Text GLabel 5500 4900 2    60   BiDi ~ 0
SCL1
Text GLabel 5500 5000 2    60   BiDi ~ 0
SDA1
$Comp
L R R23
U 1 1 588C61BC
P 850 2700
F 0 "R23" V 930 2700 50  0000 C CNN
F 1 "4k7" V 850 2700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 780 2700 50  0001 C CNN
F 3 "" H 850 2700 50  0000 C CNN
	1    850  2700
	1    0    0    -1  
$EndComp
$Comp
L R R24
U 1 1 588C621F
P 1250 2700
F 0 "R24" V 1330 2700 50  0000 C CNN
F 1 "4k7" V 1250 2700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 1180 2700 50  0001 C CNN
F 3 "" H 1250 2700 50  0000 C CNN
	1    1250 2700
	1    0    0    -1  
$EndComp
$Comp
L R R25
U 1 1 588C627C
P 2100 2700
F 0 "R25" V 2180 2700 50  0000 C CNN
F 1 "4k7" V 2100 2700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2030 2700 50  0001 C CNN
F 3 "" H 2100 2700 50  0000 C CNN
	1    2100 2700
	1    0    0    -1  
$EndComp
$Comp
L R R26
U 1 1 588C62D9
P 2550 2700
F 0 "R26" V 2630 2700 50  0000 C CNN
F 1 "4k7" V 2550 2700 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 2480 2700 50  0001 C CNN
F 3 "" H 2550 2700 50  0000 C CNN
	1    2550 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	850  2850 850  3200
Connection ~ 850  3200
Wire Wire Line
	1250 2850 1250 3100
Connection ~ 1250 3100
Wire Wire Line
	850  2550 1400 2550
Connection ~ 1400 2550
Connection ~ 1250 2550
Wire Wire Line
	2100 2850 2100 3200
Connection ~ 2100 3200
Wire Wire Line
	2550 2850 2550 3100
Connection ~ 2550 3100
Wire Wire Line
	2100 2550 2700 2550
Connection ~ 2700 2550
Connection ~ 2550 2550
$Comp
L CONN_01X08 P4
U 1 1 588C8519
P 10250 3200
F 0 "P4" H 10250 3650 50  0000 C CNN
F 1 "CONN_01X08" V 10350 3200 50  0000 C CNN
F 2 "sensact:KF141R-8p" H 10250 3200 50  0001 C CNN
F 3 "" H 10250 3200 50  0000 C CNN
	1    10250 3200
	1    0    0    -1  
$EndComp
Wire Wire Line
	10050 2850 10050 2850
Wire Wire Line
	10050 2950 9700 2950
Wire Wire Line
	9700 3050 10050 3050
Wire Wire Line
	10050 3150 10000 3150
Wire Wire Line
	10050 3250 9850 3250
Wire Wire Line
	10050 3350 9700 3350
Wire Wire Line
	10050 3450 9700 3450
Wire Wire Line
	10050 3550 9700 3550
$Comp
L GND #PWR054
U 1 1 588C8F45
P 9700 3550
F 0 "#PWR054" H 9700 3300 50  0001 C CNN
F 1 "GND" H 9700 3400 50  0000 C CNN
F 2 "" H 9700 3550 50  0000 C CNN
F 3 "" H 9700 3550 50  0000 C CNN
	1    9700 3550
	0    1    1    0   
$EndComp
$Comp
L GND #PWR055
U 1 1 588C8F9F
P 10000 3150
F 0 "#PWR055" H 10000 2900 50  0001 C CNN
F 1 "GND" H 10000 3000 50  0000 C CNN
F 2 "" H 10000 3150 50  0000 C CNN
F 3 "" H 10000 3150 50  0000 C CNN
	1    10000 3150
	0    1    1    0   
$EndComp
$Comp
L +5V #PWR056
U 1 1 588C8FF9
P 9700 3000
F 0 "#PWR056" H 9700 2850 50  0001 C CNN
F 1 "+5V" H 9700 3140 50  0000 C CNN
F 2 "" H 9700 3000 50  0000 C CNN
F 3 "" H 9700 3000 50  0000 C CNN
	1    9700 3000
	0    -1   -1   0   
$EndComp
$Comp
L +5V #PWR057
U 1 1 588C9053
P 9700 3350
F 0 "#PWR057" H 9700 3200 50  0001 C CNN
F 1 "+5V" H 9700 3490 50  0000 C CNN
F 2 "" H 9700 3350 50  0000 C CNN
F 3 "" H 9700 3350 50  0000 C CNN
	1    9700 3350
	0    -1   -1   0   
$EndComp
$Comp
L +12V #PWR058
U 1 1 588C90AD
P 10050 2850
F 0 "#PWR058" H 10050 2700 50  0001 C CNN
F 1 "+12V" H 10050 2990 50  0000 C CNN
F 2 "" H 10050 2850 50  0000 C CNN
F 3 "" H 10050 2850 50  0000 C CNN
	1    10050 2850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	9700 2950 9700 3050
Connection ~ 9700 3000
$Comp
L +24V #PWR059
U 1 1 588C954D
P 9850 3250
F 0 "#PWR059" H 9850 3100 50  0001 C CNN
F 1 "+24V" H 9850 3390 50  0000 C CNN
F 2 "" H 9850 3250 50  0000 C CNN
F 3 "" H 9850 3250 50  0000 C CNN
	1    9850 3250
	0    -1   -1   0   
$EndComp
Text Label 9700 3450 0    60   ~ 0
1Wa
Wire Wire Line
	6700 1750 6700 1800
Wire Wire Line
	6700 1800 6450 1800
Wire Wire Line
	6450 1800 6450 1850
Wire Wire Line
	6700 1850 6600 1850
Wire Wire Line
	6600 1850 6600 1750
Wire Wire Line
	6600 1750 6450 1750
$Comp
L CONN_01X02 P8
U 1 1 588CB913
P 10250 4100
F 0 "P8" H 10250 4250 50  0000 C CNN
F 1 "CONN_01X02" V 10350 4100 50  0000 C CNN
F 2 "Pin_Headers:Pin_Header_Straight_1x02_Pitch2.54mm" H 10250 4100 50  0001 C CNN
F 3 "" H 10250 4100 50  0000 C CNN
	1    10250 4100
	1    0    0    -1  
$EndComp
$Comp
L GND #PWR060
U 1 1 588CB97C
P 10000 4150
F 0 "#PWR060" H 10000 3900 50  0001 C CNN
F 1 "GND" H 10000 4000 50  0000 C CNN
F 2 "" H 10000 4150 50  0000 C CNN
F 3 "" H 10000 4150 50  0000 C CNN
	1    10000 4150
	1    0    0    -1  
$EndComp
$Comp
L +24V #PWR061
U 1 1 588CBB96
P 10000 4050
F 0 "#PWR061" H 10000 3900 50  0001 C CNN
F 1 "+24V" H 10000 4190 50  0000 C CNN
F 2 "" H 10000 4050 50  0000 C CNN
F 3 "" H 10000 4050 50  0000 C CNN
	1    10000 4050
	1    0    0    -1  
$EndComp
Wire Wire Line
	10050 4050 10000 4050
Wire Wire Line
	10050 4150 10000 4150
$EndSCHEMATC