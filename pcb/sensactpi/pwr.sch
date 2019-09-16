EESchema Schematic File Version 4
LIBS:sensactpi-cache
EELAYER 26 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 4 5
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
L sensact:TPS5430 U9
U 1 1 5AA6F53E
P 2350 1500
F 0 "U9" H 2050 1750 60  0000 L CNN
F 1 "TPS5430" H 2050 1850 60  0000 L CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_pad_handsolder" H 2550 1500 60  0001 C CNN
F 3 "" H 2550 1500 60  0001 C CNN
	1    2350 1500
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR071
U 1 1 5AA6F5EA
P 3200 1450
F 0 "#PWR071" H 3200 1300 50  0001 C CNN
F 1 "+24V" H 3200 1590 50  0000 C CNN
F 2 "" H 3200 1450 50  0000 C CNN
F 3 "" H 3200 1450 50  0000 C CNN
	1    3200 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:C C20
U 1 1 5AA6F60E
P 3300 1600
F 0 "C20" H 3325 1700 50  0000 L CNN
F 1 "4u7" H 3325 1500 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3338 1450 50  0001 C CNN
F 3 "" H 3300 1600 50  0000 C CNN
	1    3300 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C18
U 1 1 5AA6F635
P 3100 1600
F 0 "C18" H 3125 1700 50  0000 L CNN
F 1 "4u7" H 3125 1500 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3138 1450 50  0001 C CNN
F 3 "" H 3100 1600 50  0000 C CNN
	1    3100 1600
	1    0    0    -1  
$EndComp
Connection ~ 3100 1450
Connection ~ 3300 1450
Wire Wire Line
	2900 1550 3000 1550
Wire Wire Line
	3000 1550 3000 1950
Wire Wire Line
	3000 1750 3700 1750
Connection ~ 3100 1750
Wire Wire Line
	2900 1450 3300 1450
Connection ~ 3200 1450
$Comp
L power:GND #PWR072
U 1 1 5AA6F6D9
P 3200 1750
F 0 "#PWR072" H 3200 1500 50  0001 C CNN
F 1 "GND" H 3200 1600 50  0000 C CNN
F 2 "" H 3200 1750 50  0000 C CNN
F 3 "" H 3200 1750 50  0000 C CNN
	1    3200 1750
	1    0    0    -1  
$EndComp
Connection ~ 3200 1750
Connection ~ 3300 1750
$Comp
L Device:C C16
U 1 1 5AA6F705
P 1750 1200
F 0 "C16" H 1775 1300 50  0000 L CNN
F 1 "10n" H 1775 1100 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1788 1050 50  0001 C CNN
F 3 "" H 1750 1200 50  0000 C CNN
	1    1750 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Schottky D2
U 1 1 5AA6F72C
P 3100 1050
F 0 "D2" H 3100 1150 50  0000 C CNN
F 1 "D_Schottky" H 3100 950 50  0000 C CNN
F 2 "sensact:do214aa_handsolder" H 3100 1050 50  0001 C CNN
F 3 "" H 3100 1050 50  0000 C CNN
	1    3100 1050
	0    1    1    0   
$EndComp
Wire Wire Line
	1750 1050 1750 900 
Wire Wire Line
	1750 900  3200 900 
Wire Wire Line
	1800 1350 1750 1350
$Comp
L Device:L L3
U 1 1 5AA6F7C5
P 3350 900
F 0 "L3" V 3300 900 50  0000 C CNN
F 1 "22u" V 3425 900 50  0000 C CNN
F 2 "sensact:Inductor_CD104_handsolder" H 3350 900 50  0001 C CNN
F 3 "" H 3350 900 50  0000 C CNN
	1    3350 900 
	0    1    1    0   
$EndComp
Connection ~ 3000 1750
Connection ~ 2900 900 
$Comp
L Device:C C22
U 1 1 5AA6F83E
P 3500 1600
F 0 "C22" H 3525 1700 50  0000 L CNN
F 1 "m1" H 3525 1500 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3538 1450 50  0001 C CNN
F 3 "" H 3500 1600 50  0000 C CNN
	1    3500 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C24
U 1 1 5AA6F86D
P 3700 1600
F 0 "C24" H 3725 1700 50  0000 L CNN
F 1 "m1" H 3725 1500 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3738 1450 50  0001 C CNN
F 3 "" H 3700 1600 50  0000 C CNN
	1    3700 1600
	1    0    0    -1  
$EndComp
Connection ~ 3500 1750
Wire Wire Line
	3500 900  3500 1450
Wire Wire Line
	3500 900  3950 900 
Wire Wire Line
	3700 800  3700 1450
$Comp
L Device:R R14
U 1 1 5AA6FC66
P 1750 1800
F 0 "R14" V 1830 1800 50  0000 C CNN
F 1 "3K3" V 1750 1800 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 1680 1800 50  0001 C CNN
F 3 "" H 1750 1800 50  0000 C CNN
	1    1750 1800
	1    0    0    -1  
$EndComp
$Comp
L Device:R R12
U 1 1 5AA6FC93
P 1450 1500
F 0 "R12" V 1530 1500 50  0000 C CNN
F 1 "10k" V 1450 1500 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 1380 1500 50  0001 C CNN
F 3 "" H 1450 1500 50  0000 C CNN
	1    1450 1500
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 1950 1750 1950
Wire Wire Line
	2350 1900 2350 1950
Connection ~ 2350 1950
Wire Wire Line
	1450 1650 1800 1650
Wire Wire Line
	3700 800  1450 800 
Wire Wire Line
	1450 800  1450 1350
Connection ~ 3700 900 
Connection ~ 1750 1650
$Comp
L power:+5V #PWR073
U 1 1 5AA6FE7A
P 3950 900
F 0 "#PWR073" H 3950 750 50  0001 C CNN
F 1 "+5V" H 3950 1040 50  0000 C CNN
F 2 "" H 3950 900 50  0000 C CNN
F 3 "" H 3950 900 50  0000 C CNN
	1    3950 900 
	1    0    0    -1  
$EndComp
$Comp
L sensact:TPS5430 U10
U 1 1 5AA70070
P 2350 2900
F 0 "U10" H 2050 3150 60  0000 L CNN
F 1 "TPS5430" H 2050 3250 60  0000 L CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_pad_handsolder" H 2550 2900 60  0001 C CNN
F 3 "" H 2550 2900 60  0001 C CNN
	1    2350 2900
	1    0    0    -1  
$EndComp
$Comp
L power:+24V #PWR074
U 1 1 5AA70076
P 3200 2850
F 0 "#PWR074" H 3200 2700 50  0001 C CNN
F 1 "+24V" H 3200 2990 50  0000 C CNN
F 2 "" H 3200 2850 50  0000 C CNN
F 3 "" H 3200 2850 50  0000 C CNN
	1    3200 2850
	1    0    0    -1  
$EndComp
$Comp
L Device:C C21
U 1 1 5AA7007C
P 3300 3000
F 0 "C21" H 3325 3100 50  0000 L CNN
F 1 "4u7" H 3325 2900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3338 2850 50  0001 C CNN
F 3 "" H 3300 3000 50  0000 C CNN
	1    3300 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C19
U 1 1 5AA70082
P 3100 3000
F 0 "C19" H 3125 3100 50  0000 L CNN
F 1 "4u7" H 3125 2900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3138 2850 50  0001 C CNN
F 3 "" H 3100 3000 50  0000 C CNN
	1    3100 3000
	1    0    0    -1  
$EndComp
Connection ~ 3100 2850
Connection ~ 3300 2850
Wire Wire Line
	2900 2950 3000 2950
Wire Wire Line
	3000 2950 3000 3350
Wire Wire Line
	3000 3150 3700 3150
Connection ~ 3100 3150
Wire Wire Line
	2900 2850 3300 2850
Connection ~ 3200 2850
$Comp
L power:GND #PWR075
U 1 1 5AA70090
P 3200 3150
F 0 "#PWR075" H 3200 2900 50  0001 C CNN
F 1 "GND" H 3200 3000 50  0000 C CNN
F 2 "" H 3200 3150 50  0000 C CNN
F 3 "" H 3200 3150 50  0000 C CNN
	1    3200 3150
	1    0    0    -1  
$EndComp
Connection ~ 3200 3150
Connection ~ 3300 3150
$Comp
L Device:C C17
U 1 1 5AA70098
P 1650 2600
F 0 "C17" H 1675 2700 50  0000 L CNN
F 1 "10n" H 1675 2500 50  0000 L CNN
F 2 "Capacitors_SMD:C_0805_HandSoldering" H 1688 2450 50  0001 C CNN
F 3 "" H 1650 2600 50  0000 C CNN
	1    1650 2600
	1    0    0    -1  
$EndComp
$Comp
L Device:D_Schottky D3
U 1 1 5AA7009E
P 3100 2450
F 0 "D3" H 3100 2550 50  0000 C CNN
F 1 "D_Schottky" H 3100 2350 50  0000 C CNN
F 2 "sensact:do214aa_handsolder" H 3100 2450 50  0001 C CNN
F 3 "" H 3100 2450 50  0000 C CNN
	1    3100 2450
	0    1    1    0   
$EndComp
Wire Wire Line
	1650 2450 1650 2300
Wire Wire Line
	1650 2300 3150 2300
Wire Wire Line
	2900 2300 2900 2750
Wire Wire Line
	1800 2750 1650 2750
$Comp
L Device:L L2
U 1 1 5AA700A9
P 3300 2300
F 0 "L2" V 3250 2300 50  0000 C CNN
F 1 "47u" V 3375 2300 50  0000 C CNN
F 2 "sensact:Inductor_CD104_handsolder" H 3300 2300 50  0001 C CNN
F 3 "" H 3300 2300 50  0000 C CNN
	1    3300 2300
	0    1    1    0   
$EndComp
Connection ~ 3000 3150
Connection ~ 2900 2300
$Comp
L Device:C C23
U 1 1 5AA700B1
P 3500 3000
F 0 "C23" H 3525 3100 50  0000 L CNN
F 1 "m1" H 3525 2900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3538 2850 50  0001 C CNN
F 3 "" H 3500 3000 50  0000 C CNN
	1    3500 3000
	1    0    0    -1  
$EndComp
$Comp
L Device:C C25
U 1 1 5AA700B7
P 3700 3000
F 0 "C25" H 3725 3100 50  0000 L CNN
F 1 "m1" H 3725 2900 50  0000 L CNN
F 2 "Capacitors_SMD:C_1206_HandSoldering" H 3738 2850 50  0001 C CNN
F 3 "" H 3700 3000 50  0000 C CNN
	1    3700 3000
	1    0    0    -1  
$EndComp
Connection ~ 3500 3150
Wire Wire Line
	3500 2300 3500 2850
Wire Wire Line
	3450 2300 3950 2300
Wire Wire Line
	3700 2200 3700 2850
$Comp
L Device:R R15
U 1 1 5AA700C1
P 1350 2900
F 0 "R15" V 1430 2900 50  0000 C CNN
F 1 "1k5" V 1350 2900 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 1280 2900 50  0001 C CNN
F 3 "" H 1350 2900 50  0000 C CNN
	1    1350 2900
	1    0    0    -1  
$EndComp
$Comp
L Device:R R13
U 1 1 5AA700C7
P 1350 2500
F 0 "R13" V 1430 2500 50  0000 C CNN
F 1 "10k" V 1350 2500 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 1280 2500 50  0001 C CNN
F 3 "" H 1350 2500 50  0000 C CNN
	1    1350 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 3300 2350 3350
Connection ~ 2350 3350
Wire Wire Line
	1350 3050 1800 3050
Wire Wire Line
	3700 2200 1350 2200
Wire Wire Line
	1350 2200 1350 2350
Connection ~ 3700 2300
Connection ~ 1750 3050
$Comp
L power:+3V3 #PWR076
U 1 1 5AA70106
P 3950 2300
F 0 "#PWR076" H 3950 2150 50  0001 C CNN
F 1 "+3V3" H 3950 2440 50  0000 C CNN
F 2 "" H 3950 2300 50  0000 C CNN
F 3 "" H 3950 2300 50  0000 C CNN
	1    3950 2300
	1    0    0    -1  
$EndComp
$Comp
L Device:R R16
U 1 1 5AA701BC
P 1950 3350
F 0 "R16" V 2030 3350 50  0000 C CNN
F 1 "6k8" V 1950 3350 50  0000 C CNN
F 2 "Resistors_SMD:R_0805_HandSoldering" V 1880 3350 50  0001 C CNN
F 3 "" H 1950 3350 50  0000 C CNN
	1    1950 3350
	0    1    1    0   
$EndComp
Wire Wire Line
	3000 3350 2100 3350
Wire Wire Line
	1350 3350 1800 3350
Connection ~ 3500 2300
Wire Wire Line
	2900 1350 2900 900 
$Comp
L power:GND #PWR077
U 1 1 5AA9D1C9
P 3100 1250
F 0 "#PWR077" H 3100 1000 50  0001 C CNN
F 1 "GND" H 3100 1100 50  0000 C CNN
F 2 "" H 3100 1250 50  0000 C CNN
F 3 "" H 3100 1250 50  0000 C CNN
	1    3100 1250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 1200 3100 1250
Connection ~ 3100 900 
Wire Wire Line
	3100 2300 3100 2300
Connection ~ 3100 2300
$Comp
L power:GND #PWR078
U 1 1 5AA9D3D0
P 3100 2650
F 0 "#PWR078" H 3100 2400 50  0001 C CNN
F 1 "GND" H 3100 2500 50  0000 C CNN
F 2 "" H 3100 2650 50  0000 C CNN
F 3 "" H 3100 2650 50  0000 C CNN
	1    3100 2650
	1    0    0    -1  
$EndComp
Wire Wire Line
	3100 2600 3100 2650
Text Label 2900 1300 0    60   ~ 0
SW5V
Text Label 2900 2700 0    60   ~ 0
SW3V3
Text Label 1800 1350 0    60   ~ 0
BOOT5V
Text Label 1800 2750 0    60   ~ 0
BOOT3V3
$Comp
L power:GND #PWR079
U 1 1 5AC52D5C
P 1800 2900
F 0 "#PWR079" H 1800 2650 50  0001 C CNN
F 1 "GND" H 1800 2750 50  0000 C CNN
F 2 "" H 1800 2900 50  0000 C CNN
F 3 "" H 1800 2900 50  0000 C CNN
	1    1800 2900
	0    1    1    0   
$EndComp
Wire Wire Line
	1800 2850 1800 2950
Connection ~ 1800 2900
$Comp
L power:GND #PWR080
U 1 1 5AC5310A
P 1800 1500
F 0 "#PWR080" H 1800 1250 50  0001 C CNN
F 1 "GND" H 1800 1350 50  0000 C CNN
F 2 "" H 1800 1500 50  0000 C CNN
F 3 "" H 1800 1500 50  0000 C CNN
	1    1800 1500
	0    1    1    0   
$EndComp
Wire Wire Line
	1800 1450 1800 1550
Connection ~ 1800 1500
Wire Wire Line
	1350 2750 1350 2650
Wire Wire Line
	1350 3350 1350 3050
Connection ~ 1350 3050
NoConn ~ 2900 1650
NoConn ~ 2900 3050
$Comp
L power:PWR_FLAG #FLG081
U 1 1 5AF17CFB
P 3800 900
F 0 "#FLG081" H 3800 995 50  0001 C CNN
F 1 "PWR_FLAG" H 3800 1080 50  0000 C CNN
F 2 "" H 3800 900 50  0000 C CNN
F 3 "" H 3800 900 50  0000 C CNN
	1    3800 900 
	1    0    0    -1  
$EndComp
$Comp
L power:PWR_FLAG #FLG082
U 1 1 5AF17D39
P 3800 2300
F 0 "#FLG082" H 3800 2395 50  0001 C CNN
F 1 "PWR_FLAG" H 3800 2480 50  0000 C CNN
F 2 "" H 3800 2300 50  0000 C CNN
F 3 "" H 3800 2300 50  0000 C CNN
	1    3800 2300
	1    0    0    -1  
$EndComp
Connection ~ 3800 900 
Connection ~ 3800 2300
$EndSCHEMATC