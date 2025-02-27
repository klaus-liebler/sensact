EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 2 3
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
L Connector_Generic:Conn_01x02 J?
U 1 1 60B7BC42
P 5300 3600
AR Path="/5FDDB245/60B7BC42" Ref="J?"  Part="1" 
AR Path="/60B762EC/60B7BC42" Ref="J10"  Part="1" 
AR Path="/60FC7D97/60B7BC42" Ref="J7"  Part="1" 
F 0 "J7" H 5380 3592 50  0000 L CNN
F 1 "SPK" H 5380 3501 50  0000 L CNN
F 2 "Connector_PinHeader_2.54mm:PinHeader_1x02_P2.54mm_Vertical" H 5300 3600 50  0001 C CNN
F 3 "~" H 5300 3600 50  0001 C CNN
	1    5300 3600
	1    0    0    1   
$EndComp
Text GLabel 2900 4250 1    50   Input ~ 0
SPK_OUT
$Comp
L Device:R_POT RV1
U 1 1 60B8D423
P 2900 4400
F 0 "RV1" H 2831 4446 50  0000 R CNN
F 1 "5k" H 2831 4355 50  0000 R CNN
F 2 "Potentiometer_SMD:Potentiometer_Bourns_3314G_Vertical" H 2900 4400 50  0001 C CNN
F 3 "~" H 2900 4400 50  0001 C CNN
	1    2900 4400
	1    0    0    -1  
$EndComp
$Comp
L Device:C C8
U 1 1 60B7E733
P 5800 3550
F 0 "C8" H 5915 3596 50  0000 L CNN
F 1 "u1" H 5915 3505 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 5838 3400 50  0001 C CNN
F 3 "~" H 5800 3550 50  0001 C CNN
	1    5800 3550
	1    0    0    -1  
$EndComp
$Comp
L liebler_SEMICONDUCTORS:MD8002A IC1
U 1 1 60B8825A
P 3700 3500
AR Path="/60B8825A" Ref="IC1"  Part="1" 
AR Path="/60B762EC/60B8825A" Ref="IC1"  Part="1" 
AR Path="/60FC7D97/60B8825A" Ref="IC1"  Part="1" 
F 0 "IC1" H 4200 3765 50  0000 C CNN
F 1 "MD8002A" H 4200 3674 50  0000 C CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_handsolder" H 4550 3600 50  0001 L CNN
F 3 "https://www.sunrom.com/get/408862" H 4550 3500 50  0001 L CNN
F 4 "Audio Amplifier, SOP8" H 4550 3400 50  0001 L CNN "Description"
F 5 "1.75" H 4550 3300 50  0001 L CNN "Height"
F 6 "Sunrom" H 4550 3200 50  0001 L CNN "Manufacturer_Name"
F 7 "MD8002A" H 4550 3100 50  0001 L CNN "Manufacturer_Part_Number"
F 8 "" H 4550 3000 50  0001 L CNN "Mouser Part Number"
F 9 "" H 4550 2900 50  0001 L CNN "Mouser Price/Stock"
F 10 "" H 4550 2800 50  0001 L CNN "RS Part Number"
F 11 "" H 4550 2700 50  0001 L CNN "RS Price/Stock"
	1    3700 3500
	1    0    0    -1  
$EndComp
$Comp
L Device:C C6
U 1 1 60B8A210
P 3150 3750
F 0 "C6" H 3265 3796 50  0000 L CNN
F 1 "1u" H 3265 3705 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3188 3600 50  0001 C CNN
F 3 "~" H 3150 3750 50  0001 C CNN
	1    3150 3750
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 3700 3700 3600
Wire Wire Line
	3150 3600 3700 3600
Connection ~ 3700 3600
$Comp
L Device:C C7
U 1 1 60B8BBBE
P 3500 4250
F 0 "C7" H 3615 4296 50  0000 L CNN
F 1 "u1" H 3615 4205 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 3538 4100 50  0001 C CNN
F 3 "~" H 3500 4250 50  0001 C CNN
	1    3500 4250
	1    0    0    -1  
$EndComp
Wire Wire Line
	3700 3800 3500 3800
$Comp
L Device:R R7
U 1 1 60B8CF3F
P 3500 3950
F 0 "R7" H 3570 3996 50  0000 L CNN
F 1 "10k" H 3570 3905 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3430 3950 50  0001 C CNN
F 3 "~" H 3500 3950 50  0001 C CNN
	1    3500 3950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R8
U 1 1 60B8DCAD
P 4200 4150
F 0 "R8" V 4100 4150 50  0000 C CNN
F 1 "22k" V 4200 4150 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4130 4150 50  0001 C CNN
F 3 "~" H 4200 4150 50  0001 C CNN
	1    4200 4150
	0    1    1    0   
$EndComp
Wire Wire Line
	4700 3800 4700 4150
Wire Wire Line
	4700 4150 4350 4150
Wire Wire Line
	3700 3800 3700 4150
Wire Wire Line
	3700 4150 4050 4150
Connection ~ 3700 3800
Wire Wire Line
	4700 3500 5100 3500
Wire Wire Line
	4700 3800 5100 3800
Connection ~ 4700 3800
Wire Wire Line
	5100 3600 5100 3800
Wire Wire Line
	3050 4400 3500 4400
$Comp
L Device:C C11
U 1 1 60B8A568
P 6150 3550
F 0 "C11" H 6265 3596 50  0000 L CNN
F 1 "10u" H 6265 3505 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 6188 3400 50  0001 C CNN
F 3 "~" H 6150 3550 50  0001 C CNN
	1    6150 3550
	1    0    0    -1  
$EndComp
Wire Wire Line
	6150 3400 5800 3400
Wire Wire Line
	6150 3700 5800 3700
Connection ~ 5800 3400
Wire Wire Line
	5800 3400 5600 3400
Wire Wire Line
	5600 3400 5600 3700
Wire Wire Line
	5600 3700 4700 3700
$Comp
L power:+3.3V #PWR0114
U 1 1 60F36C59
P 5800 3400
F 0 "#PWR0114" H 5800 3250 50  0001 C CNN
F 1 "+3.3V" H 5815 3573 50  0000 C CNN
F 2 "" H 5800 3400 50  0001 C CNN
F 3 "" H 5800 3400 50  0001 C CNN
	1    5800 3400
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0109
U 1 1 60F19340
P 5800 3700
F 0 "#PWR0109" H 5800 3450 50  0001 C CNN
F 1 "GNDD" H 5804 3545 50  0000 C CNN
F 2 "" H 5800 3700 50  0001 C CNN
F 3 "" H 5800 3700 50  0001 C CNN
	1    5800 3700
	1    0    0    -1  
$EndComp
Connection ~ 5800 3700
$Comp
L power:GNDD #PWR0112
U 1 1 60F19863
P 3700 3500
F 0 "#PWR0112" H 3700 3250 50  0001 C CNN
F 1 "GNDD" V 3704 3390 50  0000 R CNN
F 2 "" H 3700 3500 50  0001 C CNN
F 3 "" H 3700 3500 50  0001 C CNN
	1    3700 3500
	0    1    1    0   
$EndComp
$Comp
L power:GNDD #PWR0113
U 1 1 60F19E60
P 2900 4550
F 0 "#PWR0113" H 2900 4300 50  0001 C CNN
F 1 "GNDD" H 2904 4395 50  0000 C CNN
F 2 "" H 2900 4550 50  0001 C CNN
F 3 "" H 2900 4550 50  0001 C CNN
	1    2900 4550
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0111
U 1 1 60F1BF01
P 3150 3900
F 0 "#PWR0111" H 3150 3650 50  0001 C CNN
F 1 "GNDD" H 3154 3745 50  0000 C CNN
F 2 "" H 3150 3900 50  0001 C CNN
F 3 "" H 3150 3900 50  0001 C CNN
	1    3150 3900
	1    0    0    -1  
$EndComp
$Comp
L power:GNDD #PWR0110
U 1 1 60F1C77F
P 4700 3600
F 0 "#PWR0110" H 4700 3350 50  0001 C CNN
F 1 "GNDD" V 4704 3490 50  0000 R CNN
F 2 "" H 4700 3600 50  0001 C CNN
F 3 "" H 4700 3600 50  0001 C CNN
	1    4700 3600
	0    -1   -1   0   
$EndComp
$EndSCHEMATC
