EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 8
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
L Regulator_Switching:TPS5430DDA U7
U 1 1 5FB2BDEA
P 2750 1500
F 0 "U7" H 2750 1967 50  0000 C CNN
F 1 "TPS5430DDA" H 2750 1876 50  0000 C CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_pad_handsolder" H 2800 1150 50  0001 L CIN
F 3 "http://www.ti.com/lit/ds/symlink/tps5430.pdf" H 2750 1500 50  0001 C CNN
	1    2750 1500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C35
U 1 1 60E48DF8
P 3400 1300
F 0 "C35" V 3148 1300 50  0000 C CNN
F 1 "10n" V 3239 1300 50  0000 C CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3438 1150 50  0001 C CNN
F 3 "~" H 3400 1300 50  0001 C CNN
	1    3400 1300
	0    1    1    0   
$EndComp
$Comp
L Device:D_Schottky D2
U 1 1 60E49B84
P 3650 1450
F 0 "D2" V 3604 1530 50  0000 L CNN
F 1 "2A" V 3695 1530 50  0000 L CNN
F 2 "Diode_SMD:D_SMB_Handsoldering" H 3650 1450 50  0001 C CNN
F 3 "~" H 3650 1450 50  0001 C CNN
	1    3650 1450
	0    1    1    0   
$EndComp
Wire Wire Line
	3250 1500 3550 1500
Wire Wire Line
	3550 1500 3550 1300
$Comp
L Device:L L5
U 1 1 60E4A919
P 3950 1300
F 0 "L5" V 3769 1300 50  0000 C CNN
F 1 "22u" V 3860 1300 50  0000 C CNN
F 2 "liebler_PASSIVES:Inductor_CDRH104_handsolder" H 3950 1300 50  0001 C CNN
F 3 "~" H 3950 1300 50  0001 C CNN
	1    3950 1300
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR0118
U 1 1 60E4BE26
P 2700 1900
F 0 "#PWR0118" H 2700 1650 50  0001 C CNN
F 1 "GND" H 2705 1727 50  0000 C CNN
F 2 "" H 2700 1900 50  0001 C CNN
F 3 "" H 2700 1900 50  0001 C CNN
	1    2700 1900
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR0119
U 1 1 60E4C3EA
P 3650 1600
F 0 "#PWR0119" H 3650 1350 50  0001 C CNN
F 1 "GND" H 3655 1427 50  0000 C CNN
F 2 "" H 3650 1600 50  0001 C CNN
F 3 "" H 3650 1600 50  0001 C CNN
	1    3650 1600
	1    0    0    -1  
$EndComp
Wire Wire Line
	2650 1900 2700 1900
Connection ~ 2700 1900
Wire Wire Line
	2700 1900 2750 1900
Wire Wire Line
	3550 1300 3650 1300
Connection ~ 3550 1300
Connection ~ 3650 1300
Wire Wire Line
	3650 1300 3800 1300
$Comp
L Device:C C36
U 1 1 60E4D12A
P 4500 1450
F 0 "C36" H 4615 1496 50  0000 L CNN
F 1 "m1" H 4615 1405 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.33x2.70mm_HandSolder" H 4538 1300 50  0001 C CNN
F 3 "~" H 4500 1450 50  0001 C CNN
	1    4500 1450
	1    0    0    -1  
$EndComp
Connection ~ 4500 1300
Wire Wire Line
	4500 1300 4800 1300
Wire Wire Line
	4500 1600 4800 1600
$Comp
L Device:C C37
U 1 1 60E4E2F7
P 4800 1450
F 0 "C37" H 4915 1496 50  0000 L CNN
F 1 "m1" H 4915 1405 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.33x2.70mm_HandSolder" H 4838 1300 50  0001 C CNN
F 3 "~" H 4800 1450 50  0001 C CNN
	1    4800 1450
	1    0    0    -1  
$EndComp
Connection ~ 4800 1300
Wire Wire Line
	4800 1300 5100 1300
Connection ~ 4800 1600
Wire Wire Line
	4800 1600 5100 1600
$Comp
L Device:C C38
U 1 1 60E4E6D5
P 5100 1450
F 0 "C38" H 5215 1496 50  0000 L CNN
F 1 "m1" H 5215 1405 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.33x2.70mm_HandSolder" H 5138 1300 50  0001 C CNN
F 3 "~" H 5100 1450 50  0001 C CNN
	1    5100 1450
	1    0    0    -1  
$EndComp
Connection ~ 5100 1300
Wire Wire Line
	5100 1300 5400 1300
Connection ~ 5100 1600
Wire Wire Line
	5100 1600 5400 1600
$Comp
L Device:C C39
U 1 1 60E4E9C0
P 5400 1450
F 0 "C39" H 5515 1496 50  0000 L CNN
F 1 "m1" H 5515 1405 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.33x2.70mm_HandSolder" H 5438 1300 50  0001 C CNN
F 3 "~" H 5400 1450 50  0001 C CNN
	1    5400 1450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R22
U 1 1 60E4FFF6
P 4250 1450
F 0 "R22" H 4320 1496 50  0000 L CNN
F 1 "10k" H 4320 1405 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4180 1450 50  0001 C CNN
F 3 "~" H 4250 1450 50  0001 C CNN
	1    4250 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	4100 1300 4250 1300
Connection ~ 4250 1300
Wire Wire Line
	4250 1300 4500 1300
Wire Wire Line
	3250 1700 3550 1700
Wire Wire Line
	3550 1700 3550 1850
Wire Wire Line
	3550 1850 3800 1850
Wire Wire Line
	3800 1850 3800 1600
Wire Wire Line
	3800 1600 4250 1600
$Comp
L Device:R R23
U 1 1 60E50E92
P 4250 1750
F 0 "R23" H 4320 1796 50  0000 L CNN
F 1 "4k7" H 4320 1705 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4180 1750 50  0001 C CNN
F 3 "~" H 4250 1750 50  0001 C CNN
	1    4250 1750
	1    0    0    -1  
$EndComp
Connection ~ 4250 1600
Connection ~ 4500 1600
$Comp
L Device:C C34
U 1 1 60E51878
P 1950 1450
F 0 "C34" H 2065 1496 50  0000 L CNN
F 1 "10u" H 2065 1405 50  0000 L CNN
F 2 "Capacitor_SMD:C_1210_3225Metric_Pad1.33x2.70mm_HandSolder" H 1988 1300 50  0001 C CNN
F 3 "~" H 1950 1450 50  0001 C CNN
	1    1950 1450
	1    0    0    -1  
$EndComp
Wire Wire Line
	2250 1700 2250 1300
Connection ~ 2250 1300
Connection ~ 2650 1900
Connection ~ 2750 1900
Wire Wire Line
	1950 1300 2250 1300
Wire Wire Line
	1950 1600 1950 1900
Wire Wire Line
	1950 1900 2650 1900
Connection ~ 1950 1300
$Comp
L power:+24V #PWR0120
U 1 1 60E54BD0
P 1950 1300
F 0 "#PWR0120" H 1950 1150 50  0001 C CNN
F 1 "+24V" V 1965 1428 50  0000 L CNN
F 2 "" H 1950 1300 50  0001 C CNN
F 3 "" H 1950 1300 50  0001 C CNN
	1    1950 1300
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V3 #PWR0121
U 1 1 60E55689
P 5400 1300
F 0 "#PWR0121" H 5400 1150 50  0001 C CNN
F 1 "+3V3" V 5415 1428 50  0000 L CNN
F 2 "" H 5400 1300 50  0001 C CNN
F 3 "" H 5400 1300 50  0001 C CNN
	1    5400 1300
	0    1    1    0   
$EndComp
Connection ~ 5400 1300
$Comp
L Device:R R24
U 1 1 60E45099
P 4250 2050
F 0 "R24" H 4320 2096 50  0000 L CNN
F 1 "1k" H 4320 2005 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4180 2050 50  0001 C CNN
F 3 "~" H 4250 2050 50  0001 C CNN
	1    4250 2050
	1    0    0    -1  
$EndComp
Wire Wire Line
	4250 2200 4500 2200
Wire Wire Line
	4500 1600 4500 2200
Wire Wire Line
	4250 2200 3900 2200
Wire Wire Line
	3900 2200 3900 1900
Connection ~ 4250 2200
Wire Wire Line
	3900 1900 2750 1900
Text Label 3350 1500 0    50   ~ 0
SW
Text Label 3350 1700 0    50   ~ 0
VSENSE
$EndSCHEMATC