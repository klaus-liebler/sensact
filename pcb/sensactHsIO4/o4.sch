EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 6 7
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
Text HLabel 2450 3150 0    50   BiDi ~ 0
OUT1
$Comp
L power:GND #PWR035
U 1 1 60587D3D
P 2450 3650
AR Path="/5E2885AD/60587D3D" Ref="#PWR035"  Part="1" 
AR Path="/5E2D1ED3/60587D3D" Ref="#PWR049"  Part="1" 
AR Path="/5E2D2B0D/60587D3D" Ref="#PWR?"  Part="1" 
AR Path="/5E2D3093/60587D3D" Ref="#PWR077"  Part="1" 
AR Path="/5E2D3748/60587D3D" Ref="#PWR063"  Part="1" 
AR Path="/5FF2049F/60587D3D" Ref="#PWR054"  Part="1" 
AR Path="/5FF22603/60587D3D" Ref="#PWR064"  Part="1" 
AR Path="/5FF227D6/60587D3D" Ref="#PWR074"  Part="1" 
AR Path="/5FF22A97/60587D3D" Ref="#PWR084"  Part="1" 
AR Path="/6057E928/60587D3D" Ref="#PWR?"  Part="1" 
AR Path="/6058654D/60587D3D" Ref="#PWR?"  Part="1" 
AR Path="/60587002/60587D3D" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 2450 3400 50  0001 C CNN
F 1 "GND" H 2455 3477 50  0000 C CNN
F 2 "" H 2450 3650 50  0001 C CNN
F 3 "" H 2450 3650 50  0001 C CNN
	1    2450 3650
	1    0    0    -1  
$EndComp
$Comp
L Device:R R31
U 1 1 5E28E1B5
P 2900 3350
AR Path="/5E2885AD/5E28E1B5" Ref="R31"  Part="1" 
AR Path="/5E2D1ED3/5E28E1B5" Ref="R51"  Part="1" 
AR Path="/5E2D2B0D/5E28E1B5" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5E28E1B5" Ref="R91"  Part="1" 
AR Path="/5E2D3748/5E28E1B5" Ref="R71"  Part="1" 
AR Path="/5FF2049F/5E28E1B5" Ref="R30"  Part="1" 
AR Path="/5FF22603/5E28E1B5" Ref="R46"  Part="1" 
AR Path="/5FF227D6/5E28E1B5" Ref="R62"  Part="1" 
AR Path="/5FF22A97/5E28E1B5" Ref="R78"  Part="1" 
AR Path="/6057E928/5E28E1B5" Ref="R?"  Part="1" 
AR Path="/6058654D/5E28E1B5" Ref="R?"  Part="1" 
AR Path="/60587002/5E28E1B5" Ref="R?"  Part="1" 
F 0 "R?" V 2800 3350 50  0000 C CNN
F 1 "1k" V 2900 3350 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2830 3350 50  0001 C CNN
F 3 "~" H 2900 3350 50  0001 C CNN
	1    2900 3350
	0    1    1    0   
$EndComp
$Comp
L Device:R R23
U 1 1 5E28E848
P 2750 3500
AR Path="/5E2885AD/5E28E848" Ref="R23"  Part="1" 
AR Path="/5E2D1ED3/5E28E848" Ref="R43"  Part="1" 
AR Path="/5E2D2B0D/5E28E848" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5E28E848" Ref="R83"  Part="1" 
AR Path="/5E2D3748/5E28E848" Ref="R63"  Part="1" 
AR Path="/5FF2049F/5E28E848" Ref="R26"  Part="1" 
AR Path="/5FF22603/5E28E848" Ref="R42"  Part="1" 
AR Path="/5FF227D6/5E28E848" Ref="R58"  Part="1" 
AR Path="/5FF22A97/5E28E848" Ref="R74"  Part="1" 
AR Path="/6057E928/5E28E848" Ref="R?"  Part="1" 
AR Path="/6058654D/5E28E848" Ref="R?"  Part="1" 
AR Path="/60587002/5E28E848" Ref="R?"  Part="1" 
F 0 "R?" V 2650 3550 50  0000 R CNN
F 1 "10k" V 2750 3500 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2680 3500 50  0001 C CNN
F 3 "~" H 2750 3500 50  0001 C CNN
	1    2750 3500
	-1   0    0    1   
$EndComp
Connection ~ 2750 3350
Wire Wire Line
	2450 3550 2450 3650
Wire Wire Line
	2450 3650 2750 3650
Connection ~ 2450 3650
$Comp
L Device:R R22
U 1 1 5E292C13
P 2600 3050
AR Path="/5E2885AD/5E292C13" Ref="R22"  Part="1" 
AR Path="/5E2D1ED3/5E292C13" Ref="R42"  Part="1" 
AR Path="/5E2D2B0D/5E292C13" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5E292C13" Ref="R82"  Part="1" 
AR Path="/5E2D3748/5E292C13" Ref="R62"  Part="1" 
AR Path="/5FF2049F/5E292C13" Ref="R22"  Part="1" 
AR Path="/5FF22603/5E292C13" Ref="R38"  Part="1" 
AR Path="/5FF227D6/5E292C13" Ref="R54"  Part="1" 
AR Path="/5FF22A97/5E292C13" Ref="R70"  Part="1" 
AR Path="/6057E928/5E292C13" Ref="R?"  Part="1" 
AR Path="/6058654D/5E292C13" Ref="R?"  Part="1" 
AR Path="/60587002/5E292C13" Ref="R?"  Part="1" 
F 0 "R?" V 2500 3100 50  0000 R CNN
F 1 "10R" V 2600 3050 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2530 3050 50  0001 C CNN
F 3 "~" H 2600 3050 50  0001 C CNN
	1    2600 3050
	0    1    1    0   
$EndComp
$Comp
L Device:Q_NMOS_GSD Q1
U 1 1 5E29E0A6
P 2550 2150
AR Path="/5E2885AD/5E29E0A6" Ref="Q1"  Part="1" 
AR Path="/5E2D1ED3/5E29E0A6" Ref="Q5"  Part="1" 
AR Path="/5E2D2B0D/5E29E0A6" Ref="Q?"  Part="1" 
AR Path="/5E2D3093/5E29E0A6" Ref="Q13"  Part="1" 
AR Path="/5E2D3748/5E29E0A6" Ref="Q9"  Part="1" 
AR Path="/5FF2049F/5E29E0A6" Ref="Q3"  Part="1" 
AR Path="/5FF22603/5E29E0A6" Ref="Q7"  Part="1" 
AR Path="/5FF227D6/5E29E0A6" Ref="Q11"  Part="1" 
AR Path="/5FF22A97/5E29E0A6" Ref="Q15"  Part="1" 
AR Path="/6057E928/5E29E0A6" Ref="Q?"  Part="1" 
AR Path="/6058654D/5E29E0A6" Ref="Q?"  Part="1" 
AR Path="/60587002/5E29E0A6" Ref="Q?"  Part="1" 
F 0 "Q?" H 2755 2196 50  0000 L CNN
F 1 "IRLML6344" H 2755 2105 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23_Handsoldering" H 2750 2250 50  0001 C CNN
F 3 "~" H 2550 2150 50  0001 C CNN
	1    2550 2150
	-1   0    0    -1  
$EndComp
Text HLabel 2450 1950 0    50   BiDi ~ 0
OUT0
$Comp
L power:GND #PWR034
U 1 1 60587D40
P 2450 2450
AR Path="/5E2885AD/60587D40" Ref="#PWR034"  Part="1" 
AR Path="/5E2D1ED3/60587D40" Ref="#PWR048"  Part="1" 
AR Path="/5E2D2B0D/60587D40" Ref="#PWR?"  Part="1" 
AR Path="/5E2D3093/60587D40" Ref="#PWR076"  Part="1" 
AR Path="/5E2D3748/60587D40" Ref="#PWR062"  Part="1" 
AR Path="/5FF2049F/60587D40" Ref="#PWR053"  Part="1" 
AR Path="/5FF22603/60587D40" Ref="#PWR063"  Part="1" 
AR Path="/5FF227D6/60587D40" Ref="#PWR073"  Part="1" 
AR Path="/5FF22A97/60587D40" Ref="#PWR083"  Part="1" 
AR Path="/6057E928/60587D40" Ref="#PWR?"  Part="1" 
AR Path="/6058654D/60587D40" Ref="#PWR?"  Part="1" 
AR Path="/60587002/60587D40" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 2450 2200 50  0001 C CNN
F 1 "GND" H 2455 2277 50  0000 C CNN
F 2 "" H 2450 2450 50  0001 C CNN
F 3 "" H 2450 2450 50  0001 C CNN
	1    2450 2450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R21
U 1 1 5E29E0C5
P 2750 2300
AR Path="/5E2885AD/5E29E0C5" Ref="R21"  Part="1" 
AR Path="/5E2D1ED3/5E29E0C5" Ref="R41"  Part="1" 
AR Path="/5E2D2B0D/5E29E0C5" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5E29E0C5" Ref="R81"  Part="1" 
AR Path="/5E2D3748/5E29E0C5" Ref="R61"  Part="1" 
AR Path="/5FF2049F/5E29E0C5" Ref="R25"  Part="1" 
AR Path="/5FF22603/5E29E0C5" Ref="R41"  Part="1" 
AR Path="/5FF227D6/5E29E0C5" Ref="R57"  Part="1" 
AR Path="/5FF22A97/5E29E0C5" Ref="R73"  Part="1" 
AR Path="/6057E928/5E29E0C5" Ref="R?"  Part="1" 
AR Path="/6058654D/5E29E0C5" Ref="R?"  Part="1" 
AR Path="/60587002/5E29E0C5" Ref="R?"  Part="1" 
F 0 "R?" V 2650 2350 50  0000 R CNN
F 1 "10k" V 2750 2300 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2680 2300 50  0001 C CNN
F 3 "~" H 2750 2300 50  0001 C CNN
	1    2750 2300
	-1   0    0    1   
$EndComp
Connection ~ 2750 2150
Wire Wire Line
	2450 2350 2450 2450
Wire Wire Line
	2450 2450 2750 2450
Connection ~ 2450 2450
$Comp
L power:GND #PWR047
U 1 1 5E2B8F6D
P 5050 3250
AR Path="/5E2885AD/5E2B8F6D" Ref="#PWR047"  Part="1" 
AR Path="/5E2D1ED3/5E2B8F6D" Ref="#PWR061"  Part="1" 
AR Path="/5E2D2B0D/5E2B8F6D" Ref="#PWR?"  Part="1" 
AR Path="/5E2D3093/5E2B8F6D" Ref="#PWR089"  Part="1" 
AR Path="/5E2D3748/5E2B8F6D" Ref="#PWR075"  Part="1" 
AR Path="/5FF2049F/5E2B8F6D" Ref="#PWR062"  Part="1" 
AR Path="/5FF22603/5E2B8F6D" Ref="#PWR072"  Part="1" 
AR Path="/5FF227D6/5E2B8F6D" Ref="#PWR082"  Part="1" 
AR Path="/5FF22A97/5E2B8F6D" Ref="#PWR092"  Part="1" 
AR Path="/6057E928/5E2B8F6D" Ref="#PWR?"  Part="1" 
AR Path="/6058654D/5E2B8F6D" Ref="#PWR?"  Part="1" 
AR Path="/60587002/5E2B8F6D" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 5050 3000 50  0001 C CNN
F 1 "GND" H 5055 3077 50  0000 C CNN
F 2 "" H 5050 3250 50  0001 C CNN
F 3 "" H 5050 3250 50  0001 C CNN
	1    5050 3250
	0    -1   -1   0   
$EndComp
Text HLabel 3050 2150 2    50   BiDi ~ 0
IN0
Text HLabel 3050 3350 2    50   BiDi ~ 0
IN1
Text HLabel 3050 4850 2    50   BiDi ~ 0
IN2
Wire Wire Line
	2750 2450 3050 2450
Connection ~ 2750 2450
Wire Wire Line
	2450 1950 2450 1850
Wire Wire Line
	2750 3650 3050 3650
Connection ~ 2750 3650
Wire Wire Line
	2450 3150 2450 3050
Wire Wire Line
	2750 3350 2750 3050
Connection ~ 2750 3050
$Comp
L Device:Q_NMOS_GSD Q?
U 1 1 5FF48F46
P 2550 4850
AR Path="/5E2885AD/5FF48F46" Ref="Q?"  Part="1" 
AR Path="/5E2D1ED3/5FF48F46" Ref="Q?"  Part="1" 
AR Path="/5E2D2B0D/5FF48F46" Ref="Q?"  Part="1" 
AR Path="/5E2D3093/5FF48F46" Ref="Q?"  Part="1" 
AR Path="/5E2D3748/5FF48F46" Ref="Q?"  Part="1" 
AR Path="/5FF2049F/5FF48F46" Ref="Q5"  Part="1" 
AR Path="/5FF22603/5FF48F46" Ref="Q9"  Part="1" 
AR Path="/5FF227D6/5FF48F46" Ref="Q13"  Part="1" 
AR Path="/5FF22A97/5FF48F46" Ref="Q17"  Part="1" 
AR Path="/6057E928/5FF48F46" Ref="Q?"  Part="1" 
AR Path="/6058654D/5FF48F46" Ref="Q?"  Part="1" 
AR Path="/60587002/5FF48F46" Ref="Q?"  Part="1" 
F 0 "Q?" H 2755 4896 50  0000 L CNN
F 1 "IRLML6344" H 2755 4805 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23_Handsoldering" H 2750 4950 50  0001 C CNN
F 3 "~" H 2550 4850 50  0001 C CNN
	1    2550 4850
	-1   0    0    -1  
$EndComp
Text HLabel 2450 4650 0    50   BiDi ~ 0
OUT2
$Comp
L power:GND #PWR?
U 1 1 60587D44
P 2450 5150
AR Path="/5E2885AD/60587D44" Ref="#PWR?"  Part="1" 
AR Path="/5E2D1ED3/60587D44" Ref="#PWR?"  Part="1" 
AR Path="/5E2D2B0D/60587D44" Ref="#PWR?"  Part="1" 
AR Path="/5E2D3093/60587D44" Ref="#PWR?"  Part="1" 
AR Path="/5E2D3748/60587D44" Ref="#PWR?"  Part="1" 
AR Path="/5FF2049F/60587D44" Ref="#PWR055"  Part="1" 
AR Path="/5FF22603/60587D44" Ref="#PWR065"  Part="1" 
AR Path="/5FF227D6/60587D44" Ref="#PWR075"  Part="1" 
AR Path="/5FF22A97/60587D44" Ref="#PWR085"  Part="1" 
AR Path="/6057E928/60587D44" Ref="#PWR?"  Part="1" 
AR Path="/6058654D/60587D44" Ref="#PWR?"  Part="1" 
AR Path="/60587002/60587D44" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 2450 4900 50  0001 C CNN
F 1 "GND" H 2455 4977 50  0000 C CNN
F 2 "" H 2450 5150 50  0001 C CNN
F 3 "" H 2450 5150 50  0001 C CNN
	1    2450 5150
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 60587D45
P 2900 4850
AR Path="/5E2885AD/60587D45" Ref="R?"  Part="1" 
AR Path="/5E2D1ED3/60587D45" Ref="R?"  Part="1" 
AR Path="/5E2D2B0D/60587D45" Ref="R?"  Part="1" 
AR Path="/5E2D3093/60587D45" Ref="R?"  Part="1" 
AR Path="/5E2D3748/60587D45" Ref="R?"  Part="1" 
AR Path="/5FF2049F/60587D45" Ref="R31"  Part="1" 
AR Path="/5FF22603/60587D45" Ref="R47"  Part="1" 
AR Path="/5FF227D6/60587D45" Ref="R63"  Part="1" 
AR Path="/5FF22A97/60587D45" Ref="R79"  Part="1" 
AR Path="/6057E928/60587D45" Ref="R?"  Part="1" 
AR Path="/6058654D/60587D45" Ref="R?"  Part="1" 
AR Path="/60587002/60587D45" Ref="R?"  Part="1" 
F 0 "R?" V 2800 4850 50  0000 C CNN
F 1 "1k" V 2900 4850 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2830 4850 50  0001 C CNN
F 3 "~" H 2900 4850 50  0001 C CNN
	1    2900 4850
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5FF49153
P 2750 5000
AR Path="/5E2885AD/5FF49153" Ref="R?"  Part="1" 
AR Path="/5E2D1ED3/5FF49153" Ref="R?"  Part="1" 
AR Path="/5E2D2B0D/5FF49153" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5FF49153" Ref="R?"  Part="1" 
AR Path="/5E2D3748/5FF49153" Ref="R?"  Part="1" 
AR Path="/5FF2049F/5FF49153" Ref="R27"  Part="1" 
AR Path="/5FF22603/5FF49153" Ref="R43"  Part="1" 
AR Path="/5FF227D6/5FF49153" Ref="R59"  Part="1" 
AR Path="/5FF22A97/5FF49153" Ref="R75"  Part="1" 
AR Path="/6057E928/5FF49153" Ref="R?"  Part="1" 
AR Path="/6058654D/5FF49153" Ref="R?"  Part="1" 
AR Path="/60587002/5FF49153" Ref="R?"  Part="1" 
F 0 "R?" V 2650 5050 50  0000 R CNN
F 1 "10k" V 2750 5000 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2680 5000 50  0001 C CNN
F 3 "~" H 2750 5000 50  0001 C CNN
	1    2750 5000
	-1   0    0    1   
$EndComp
Connection ~ 2750 4850
Wire Wire Line
	2450 5050 2450 5150
Wire Wire Line
	2450 5150 2750 5150
Connection ~ 2450 5150
$Comp
L Device:R R?
U 1 1 5FF4916B
P 2600 4550
AR Path="/5E2885AD/5FF4916B" Ref="R?"  Part="1" 
AR Path="/5E2D1ED3/5FF4916B" Ref="R?"  Part="1" 
AR Path="/5E2D2B0D/5FF4916B" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5FF4916B" Ref="R?"  Part="1" 
AR Path="/5E2D3748/5FF4916B" Ref="R?"  Part="1" 
AR Path="/5FF2049F/5FF4916B" Ref="R23"  Part="1" 
AR Path="/5FF22603/5FF4916B" Ref="R39"  Part="1" 
AR Path="/5FF227D6/5FF4916B" Ref="R55"  Part="1" 
AR Path="/5FF22A97/5FF4916B" Ref="R71"  Part="1" 
AR Path="/6057E928/5FF4916B" Ref="R?"  Part="1" 
AR Path="/6058654D/5FF4916B" Ref="R?"  Part="1" 
AR Path="/60587002/5FF4916B" Ref="R?"  Part="1" 
F 0 "R?" V 2500 4600 50  0000 R CNN
F 1 "10R" V 2600 4550 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2530 4550 50  0001 C CNN
F 3 "~" H 2600 4550 50  0001 C CNN
	1    2600 4550
	0    1    1    0   
$EndComp
Wire Wire Line
	2750 5150 3050 5150
Connection ~ 2750 5150
Wire Wire Line
	2450 4650 2450 4550
Wire Wire Line
	2750 4850 2750 4550
Wire Wire Line
	2750 4550 2750 4300
Connection ~ 2750 4550
Connection ~ 2750 5700
Wire Wire Line
	2750 5700 2750 5450
Wire Wire Line
	2750 6000 2750 5700
Wire Wire Line
	2450 5800 2450 5700
Connection ~ 2750 6300
Wire Wire Line
	2750 6300 3050 6300
$Comp
L Device:R R?
U 1 1 5FF4D691
P 2600 5700
AR Path="/5E2885AD/5FF4D691" Ref="R?"  Part="1" 
AR Path="/5E2D1ED3/5FF4D691" Ref="R?"  Part="1" 
AR Path="/5E2D2B0D/5FF4D691" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5FF4D691" Ref="R?"  Part="1" 
AR Path="/5E2D3748/5FF4D691" Ref="R?"  Part="1" 
AR Path="/5FF2049F/5FF4D691" Ref="R24"  Part="1" 
AR Path="/5FF22603/5FF4D691" Ref="R40"  Part="1" 
AR Path="/5FF227D6/5FF4D691" Ref="R56"  Part="1" 
AR Path="/5FF22A97/5FF4D691" Ref="R72"  Part="1" 
AR Path="/6057E928/5FF4D691" Ref="R?"  Part="1" 
AR Path="/6058654D/5FF4D691" Ref="R?"  Part="1" 
AR Path="/60587002/5FF4D691" Ref="R?"  Part="1" 
F 0 "R?" V 2500 5750 50  0000 R CNN
F 1 "10R" V 2600 5700 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2530 5700 50  0001 C CNN
F 3 "~" H 2600 5700 50  0001 C CNN
	1    2600 5700
	0    1    1    0   
$EndComp
Connection ~ 2450 6300
Wire Wire Line
	2450 6300 2750 6300
Wire Wire Line
	2450 6200 2450 6300
Connection ~ 2750 6000
$Comp
L Device:R R?
U 1 1 5FF4D679
P 2750 6150
AR Path="/5E2885AD/5FF4D679" Ref="R?"  Part="1" 
AR Path="/5E2D1ED3/5FF4D679" Ref="R?"  Part="1" 
AR Path="/5E2D2B0D/5FF4D679" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5FF4D679" Ref="R?"  Part="1" 
AR Path="/5E2D3748/5FF4D679" Ref="R?"  Part="1" 
AR Path="/5FF2049F/5FF4D679" Ref="R28"  Part="1" 
AR Path="/5FF22603/5FF4D679" Ref="R44"  Part="1" 
AR Path="/5FF227D6/5FF4D679" Ref="R60"  Part="1" 
AR Path="/5FF22A97/5FF4D679" Ref="R76"  Part="1" 
AR Path="/6057E928/5FF4D679" Ref="R?"  Part="1" 
AR Path="/6058654D/5FF4D679" Ref="R?"  Part="1" 
AR Path="/60587002/5FF4D679" Ref="R?"  Part="1" 
F 0 "R?" V 2650 6200 50  0000 R CNN
F 1 "10k" V 2750 6150 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2680 6150 50  0001 C CNN
F 3 "~" H 2750 6150 50  0001 C CNN
	1    2750 6150
	-1   0    0    1   
$EndComp
$Comp
L Device:R R?
U 1 1 60587D48
P 2900 6000
AR Path="/5E2885AD/60587D48" Ref="R?"  Part="1" 
AR Path="/5E2D1ED3/60587D48" Ref="R?"  Part="1" 
AR Path="/5E2D2B0D/60587D48" Ref="R?"  Part="1" 
AR Path="/5E2D3093/60587D48" Ref="R?"  Part="1" 
AR Path="/5E2D3748/60587D48" Ref="R?"  Part="1" 
AR Path="/5FF2049F/60587D48" Ref="R32"  Part="1" 
AR Path="/5FF22603/60587D48" Ref="R48"  Part="1" 
AR Path="/5FF227D6/60587D48" Ref="R64"  Part="1" 
AR Path="/5FF22A97/60587D48" Ref="R80"  Part="1" 
AR Path="/6057E928/60587D48" Ref="R?"  Part="1" 
AR Path="/6058654D/60587D48" Ref="R?"  Part="1" 
AR Path="/60587002/60587D48" Ref="R?"  Part="1" 
F 0 "R?" V 2800 6000 50  0000 C CNN
F 1 "1k" V 2900 6000 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2830 6000 50  0001 C CNN
F 3 "~" H 2900 6000 50  0001 C CNN
	1    2900 6000
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60587D47
P 2450 6300
AR Path="/5E2885AD/60587D47" Ref="#PWR?"  Part="1" 
AR Path="/5E2D1ED3/60587D47" Ref="#PWR?"  Part="1" 
AR Path="/5E2D2B0D/60587D47" Ref="#PWR?"  Part="1" 
AR Path="/5E2D3093/60587D47" Ref="#PWR?"  Part="1" 
AR Path="/5E2D3748/60587D47" Ref="#PWR?"  Part="1" 
AR Path="/5FF2049F/60587D47" Ref="#PWR056"  Part="1" 
AR Path="/5FF22603/60587D47" Ref="#PWR066"  Part="1" 
AR Path="/5FF227D6/60587D47" Ref="#PWR076"  Part="1" 
AR Path="/5FF22A97/60587D47" Ref="#PWR086"  Part="1" 
AR Path="/6057E928/60587D47" Ref="#PWR?"  Part="1" 
AR Path="/6058654D/60587D47" Ref="#PWR?"  Part="1" 
AR Path="/60587002/60587D47" Ref="#PWR?"  Part="1" 
F 0 "#PWR?" H 2450 6050 50  0001 C CNN
F 1 "GND" H 2455 6127 50  0000 C CNN
F 2 "" H 2450 6300 50  0001 C CNN
F 3 "" H 2450 6300 50  0001 C CNN
	1    2450 6300
	1    0    0    -1  
$EndComp
Text HLabel 2450 5800 0    50   BiDi ~ 0
OUT3
$Comp
L Device:Q_NMOS_GSD Q?
U 1 1 5FF4D65A
P 2550 6000
AR Path="/5E2885AD/5FF4D65A" Ref="Q?"  Part="1" 
AR Path="/5E2D1ED3/5FF4D65A" Ref="Q?"  Part="1" 
AR Path="/5E2D2B0D/5FF4D65A" Ref="Q?"  Part="1" 
AR Path="/5E2D3093/5FF4D65A" Ref="Q?"  Part="1" 
AR Path="/5E2D3748/5FF4D65A" Ref="Q?"  Part="1" 
AR Path="/5FF2049F/5FF4D65A" Ref="Q6"  Part="1" 
AR Path="/5FF22603/5FF4D65A" Ref="Q10"  Part="1" 
AR Path="/5FF227D6/5FF4D65A" Ref="Q14"  Part="1" 
AR Path="/5FF22A97/5FF4D65A" Ref="Q18"  Part="1" 
AR Path="/6057E928/5FF4D65A" Ref="Q?"  Part="1" 
AR Path="/6058654D/5FF4D65A" Ref="Q?"  Part="1" 
AR Path="/60587002/5FF4D65A" Ref="Q?"  Part="1" 
F 0 "Q?" H 2755 6046 50  0000 L CNN
F 1 "IRLML6344" H 2755 5955 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23_Handsoldering" H 2750 6100 50  0001 C CNN
F 3 "~" H 2550 6000 50  0001 C CNN
	1    2550 6000
	-1   0    0    -1  
$EndComp
Text HLabel 3050 6000 2    50   BiDi ~ 0
IN3
$Comp
L Device:R R29
U 1 1 5E29E0BB
P 2900 2150
AR Path="/5E2885AD/5E29E0BB" Ref="R29"  Part="1" 
AR Path="/5E2D1ED3/5E29E0BB" Ref="R49"  Part="1" 
AR Path="/5E2D2B0D/5E29E0BB" Ref="R?"  Part="1" 
AR Path="/5E2D3093/5E29E0BB" Ref="R89"  Part="1" 
AR Path="/5E2D3748/5E29E0BB" Ref="R69"  Part="1" 
AR Path="/5FF2049F/5E29E0BB" Ref="R29"  Part="1" 
AR Path="/5FF22603/5E29E0BB" Ref="R45"  Part="1" 
AR Path="/5FF227D6/5E29E0BB" Ref="R61"  Part="1" 
AR Path="/5FF22A97/5E29E0BB" Ref="R77"  Part="1" 
AR Path="/6057E928/5E29E0BB" Ref="R?"  Part="1" 
AR Path="/6058654D/5E29E0BB" Ref="R?"  Part="1" 
AR Path="/60587002/5E29E0BB" Ref="R?"  Part="1" 
F 0 "R?" V 2800 2150 50  0000 C CNN
F 1 "1k" V 2900 2150 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2830 2150 50  0001 C CNN
F 3 "~" H 2900 2150 50  0001 C CNN
	1    2900 2150
	0    1    1    0   
$EndComp
Wire Wire Line
	2750 4300 4450 4300
$Comp
L Power_Protection:SRV05-4 U4
U 1 1 5E2887A7
P 4550 3250
AR Path="/5E2885AD/5E2887A7" Ref="U4"  Part="1" 
AR Path="/5E2D1ED3/5E2887A7" Ref="U5"  Part="1" 
AR Path="/5E2D2B0D/5E2887A7" Ref="U?"  Part="1" 
AR Path="/5E2D3093/5E2887A7" Ref="U7"  Part="1" 
AR Path="/5E2D3748/5E2887A7" Ref="U6"  Part="1" 
AR Path="/5FF2049F/5E2887A7" Ref="U6"  Part="1" 
AR Path="/5FF22603/5E2887A7" Ref="U7"  Part="1" 
AR Path="/5FF227D6/5E2887A7" Ref="U8"  Part="1" 
AR Path="/5FF22A97/5E2887A7" Ref="U9"  Part="1" 
AR Path="/6057E928/5E2887A7" Ref="U?"  Part="1" 
AR Path="/6058654D/5E2887A7" Ref="U?"  Part="1" 
AR Path="/60587002/5E2887A7" Ref="U?"  Part="1" 
F 0 "U?" V 4400 3850 50  0000 C CNN
F 1 "SRV05-4" V 4300 3850 50  0000 C CNN
F 2 "Package_TO_SOT_SMD:SOT-23-6_Handsoldering" H 5250 2800 50  0001 C CNN
F 3 "http://www.onsemi.com/pub/Collateral/SRV05-4-D.PDF" H 4550 3250 50  0001 C CNN
	1    4550 3250
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2750 2700 4650 2700
Wire Wire Line
	4650 2700 4650 2750
Wire Wire Line
	2750 2700 2750 3050
Wire Wire Line
	4450 2750 4450 1650
Wire Wire Line
	4450 1650 2750 1650
Wire Wire Line
	4450 3750 4450 4300
Wire Wire Line
	4650 5450 4650 3750
Wire Wire Line
	2750 5450 4650 5450
Text GLabel 4050 3250 0    50   BiDi ~ 0
3V3_ZDIODE
Wire Wire Line
	2750 1850 2750 2150
Wire Wire Line
	2750 1650 2750 1850
Connection ~ 2750 1850
$Comp
L Device:R R?
U 1 1 60587D43
P 2600 1850
AR Path="/5E2885AD/60587D43" Ref="R?"  Part="1" 
AR Path="/5E2D1ED3/60587D43" Ref="R?"  Part="1" 
AR Path="/5E2D2B0D/60587D43" Ref="R?"  Part="1" 
AR Path="/5E2D3093/60587D43" Ref="R?"  Part="1" 
AR Path="/5E2D3748/60587D43" Ref="R?"  Part="1" 
AR Path="/5FF2049F/60587D43" Ref="R21"  Part="1" 
AR Path="/5FF22603/60587D43" Ref="R37"  Part="1" 
AR Path="/5FF227D6/60587D43" Ref="R53"  Part="1" 
AR Path="/5FF22A97/60587D43" Ref="R69"  Part="1" 
AR Path="/6057E928/60587D43" Ref="R?"  Part="1" 
AR Path="/6058654D/60587D43" Ref="R?"  Part="1" 
AR Path="/60587002/60587D43" Ref="R?"  Part="1" 
F 0 "R?" V 2500 1850 50  0000 C CNN
F 1 "10R" V 2600 1850 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2530 1850 50  0001 C CNN
F 3 "~" H 2600 1850 50  0001 C CNN
	1    2600 1850
	0    1    1    0   
$EndComp
$Comp
L Device:Q_NMOS_GSD Q2
U 1 1 5E28AF71
P 2550 3350
AR Path="/5E2885AD/5E28AF71" Ref="Q2"  Part="1" 
AR Path="/5E2D1ED3/5E28AF71" Ref="Q6"  Part="1" 
AR Path="/5E2D2B0D/5E28AF71" Ref="Q?"  Part="1" 
AR Path="/5E2D3093/5E28AF71" Ref="Q14"  Part="1" 
AR Path="/5E2D3748/5E28AF71" Ref="Q10"  Part="1" 
AR Path="/5FF2049F/5E28AF71" Ref="Q4"  Part="1" 
AR Path="/5FF22603/5E28AF71" Ref="Q8"  Part="1" 
AR Path="/5FF227D6/5E28AF71" Ref="Q12"  Part="1" 
AR Path="/5FF22A97/5E28AF71" Ref="Q16"  Part="1" 
AR Path="/6057E928/5E28AF71" Ref="Q?"  Part="1" 
AR Path="/6058654D/5E28AF71" Ref="Q?"  Part="1" 
AR Path="/60587002/5E28AF71" Ref="Q?"  Part="1" 
F 0 "Q?" H 2755 3396 50  0000 L CNN
F 1 "IRLML6344" H 2755 3305 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23_Handsoldering" H 2750 3450 50  0001 C CNN
F 3 "~" H 2550 3350 50  0001 C CNN
	1    2550 3350
	-1   0    0    -1  
$EndComp
$EndSCHEMATC