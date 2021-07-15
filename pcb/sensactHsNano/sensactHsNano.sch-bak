EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 5
Title ""
Date ""
Rev ""
Comp ""
Comment1 ""
Comment2 ""
Comment3 ""
Comment4 ""
$EndDescr
$Sheet
S 4400 700  900  150 
U 5F9C3C99
F0 "audioamplifier" 50
F1 "audioamplifier.sch" 50
$EndSheet
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 5FA1B92C
P 4200 6150
F 0 "J3" H 4280 6142 50  0000 L CNN
F 1 "CAN+24V" H 4280 6051 50  0000 L CNN
F 2 "Connector_Phoenix_MC:PhoenixContact_MC_1,5_4-G-3.5_1x04_P3.50mm_Horizontal" H 4200 6150 50  0001 C CNN
F 3 "~" H 4200 6150 50  0001 C CNN
	1    4200 6150
	1    0    0    1   
$EndComp
$Comp
L sensactHsNano-rescue:SensactBusConnector-sensact J2
U 1 1 5FA1CB9E
P 4750 5850
F 0 "J2" H 4808 6615 50  0000 C CNN
F 1 "SensactBus" H 4808 6524 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x05_P2.54mm_Horizontal" H 4550 5850 50  0001 C CNN
F 3 "" H 4550 5850 50  0001 C CNN
	1    4750 5850
	1    0    0    -1  
$EndComp
$Comp
L RF_Module:ESP32-WROOM-32U U?
U 1 1 5FA345D1
P 2350 2400
AR Path="/5FA2399C/5FA345D1" Ref="U?"  Part="1" 
AR Path="/5FA345D1" Ref="U3"  Part="1" 
F 0 "U3" H 2250 3850 50  0000 R CNN
F 1 "ESP32" H 2250 3750 50  0000 R CNN
F 2 "smopla:ESP32-WROOM-32_HANDSOLDER" H 2350 900 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf" H 2050 2450 50  0001 C CNN
	1    2350 2400
	1    0    0    -1  
$EndComp
Text GLabel 2950 2300 2    50   Output ~ 0
ETH_POWER
Text GLabel 2950 2500 2    50   Output ~ 0
ETH_TX0
Text GLabel 2950 2600 2    50   Output ~ 0
ETH_TXEN
Text GLabel 2950 2700 2    50   Output ~ 0
ETH_TX1
Text GLabel 2950 2900 2    50   Input ~ 0
ETH_RX0
Text GLabel 2950 3000 2    50   Input ~ 0
ETH_RX1
Text GLabel 2950 3100 2    50   Input ~ 0
ETH_CRS
Text GLabel 2950 1600 2    50   BiDi ~ 0
SDA1
Text GLabel 2950 2200 2    50   BiDi ~ 0
SCL1
NoConn ~ 1750 2900
NoConn ~ 1750 2800
NoConn ~ 1750 2700
NoConn ~ 1750 2600
NoConn ~ 1750 2500
NoConn ~ 1750 2400
Text GLabel 2950 3500 2    50   Input ~ 0
CAN_RX
Text GLabel 2950 3200 2    50   Output ~ 0
CAN_TX
$Comp
L sensactHsNano-rescue:DS2482-sensact U?
U 1 1 5FA345F1
P 2050 5950
AR Path="/5FA2399C/5FA345F1" Ref="U?"  Part="1" 
AR Path="/5FA345F1" Ref="U4"  Part="1" 
F 0 "U4" H 2050 6350 60  0000 C CNN
F 1 "DS2482" H 2050 6250 60  0000 C CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_handsolder" H 2050 6241 60  0001 C CNN
F 3 "" H 2050 5950 60  0000 C CNN
	1    2050 5950
	1    0    0    -1  
$EndComp
Text GLabel 1450 6100 0    50   BiDi ~ 0
SCL2
Text GLabel 2650 6100 2    50   BiDi ~ 0
SDA2
Text GLabel 3050 4500 2    50   BiDi ~ 0
CANH
Text GLabel 3050 5200 2    50   BiDi ~ 0
CANL
Text GLabel 4000 6050 0    50   BiDi ~ 0
CANL
Text GLabel 4000 5950 0    50   BiDi ~ 0
CANH
$Comp
L Device:R R?
U 1 1 5FA49518
P 3500 4450
AR Path="/5F9C3C99/5FA49518" Ref="R?"  Part="1" 
AR Path="/5FA49518" Ref="R1"  Part="1" 
F 0 "R1" H 3570 4496 50  0000 L CNN
F 1 "4k7" H 3570 4405 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3430 4450 50  0001 C CNN
F 3 "~" H 3500 4450 50  0001 C CNN
	1    3500 4450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5FA4951E
P 3800 4450
AR Path="/5F9C3C99/5FA4951E" Ref="R?"  Part="1" 
AR Path="/5FA4951E" Ref="R6"  Part="1" 
F 0 "R6" H 3870 4496 50  0000 L CNN
F 1 "4k7" H 3870 4405 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3730 4450 50  0001 C CNN
F 3 "~" H 3800 4450 50  0001 C CNN
	1    3800 4450
	1    0    0    -1  
$EndComp
Wire Wire Line
	3800 4300 3500 4300
$Comp
L power:+3V3 #PWR?
U 1 1 5FA49527
P 3500 4300
AR Path="/5F9C3C99/5FA49527" Ref="#PWR?"  Part="1" 
AR Path="/5FA49527" Ref="#PWR0103"  Part="1" 
F 0 "#PWR0103" H 3500 4150 50  0001 C CNN
F 1 "+3V3" H 3515 4473 50  0000 C CNN
F 2 "" H 3500 4300 50  0001 C CNN
F 3 "" H 3500 4300 50  0001 C CNN
	1    3500 4300
	1    0    0    -1  
$EndComp
Connection ~ 3500 4300
Text GLabel 3500 4600 3    50   BiDi ~ 0
SDA1
Text GLabel 3800 4600 3    50   BiDi ~ 0
SCL1
$Comp
L Device:R R?
U 1 1 5FA4C417
P 2750 4650
AR Path="/5F9C3C99/5FA4C417" Ref="R?"  Part="1" 
AR Path="/5FA4C417" Ref="R7"  Part="1" 
F 0 "R7" H 2800 4600 50  0000 L CNN
F 1 "120R" V 2750 4650 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.30x1.75mm_HandSolder" V 2680 4650 50  0001 C CNN
F 3 "~" H 2750 4650 50  0001 C CNN
	1    2750 4650
	-1   0    0    1   
$EndComp
$Comp
L Switch:SW_SPDT SW1
U 1 1 5FA4E5BD
P 2750 5000
F 0 "SW1" H 2900 5200 50  0000 C CNN
F 1 "TERM" H 2850 4950 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPDT_PCM12" H 2750 5000 50  0001 C CNN
F 3 "~" H 2750 5000 50  0001 C CNN
	1    2750 5000
	0    1    1    0   
$EndComp
$Comp
L Device:C C?
U 1 1 5FA5128B
P 600 5950
AR Path="/5F9C3C99/5FA5128B" Ref="C?"  Part="1" 
AR Path="/5FA5128B" Ref="C19"  Part="1" 
F 0 "C19" H 715 5996 50  0000 L CNN
F 1 "1u" H 715 5905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 638 5800 50  0001 C CNN
F 3 "~" H 600 5950 50  0001 C CNN
	1    600  5950
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5FA51295
P 900 5950
AR Path="/5F9C3C99/5FA51295" Ref="C?"  Part="1" 
AR Path="/5FA51295" Ref="C20"  Part="1" 
F 0 "C20" H 1015 5996 50  0000 L CNN
F 1 "u1" H 1015 5905 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 938 5800 50  0001 C CNN
F 3 "~" H 900 5950 50  0001 C CNN
	1    900  5950
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5FA5129F
P 600 6100
AR Path="/5F9C3C99/5FA5129F" Ref="#PWR?"  Part="1" 
AR Path="/5FA5129F" Ref="#PWR0106"  Part="1" 
F 0 "#PWR0106" H 600 5850 50  0001 C CNN
F 1 "GND" H 800 6000 50  0000 R CNN
F 2 "" H 600 6100 50  0001 C CNN
F 3 "" H 600 6100 50  0001 C CNN
	1    600  6100
	1    0    0    -1  
$EndComp
Wire Wire Line
	600  5800 900  5800
Connection ~ 900  5800
$Comp
L power:+3V3 #PWR?
U 1 1 5FA512AB
P 600 5800
AR Path="/5F9C3C99/5FA512AB" Ref="#PWR?"  Part="1" 
AR Path="/5FA512AB" Ref="#PWR0107"  Part="1" 
F 0 "#PWR0107" H 600 5650 50  0001 C CNN
F 1 "+3V3" H 615 5973 50  0000 C CNN
F 2 "" H 600 5800 50  0001 C CNN
F 3 "" H 600 5800 50  0001 C CNN
	1    600  5800
	1    0    0    -1  
$EndComp
Connection ~ 600  5800
Wire Wire Line
	900  5800 1450 5800
Wire Wire Line
	900  6100 600  6100
Connection ~ 600  6100
Wire Wire Line
	1450 6000 1150 6000
Wire Wire Line
	1150 6000 1150 6100
Wire Wire Line
	1150 6100 900  6100
Connection ~ 900  6100
NoConn ~ 2650 6000
$Comp
L power:GND #PWR?
U 1 1 5FA53BEC
P 2650 5850
AR Path="/5F9C3C99/5FA53BEC" Ref="#PWR?"  Part="1" 
AR Path="/5FA53BEC" Ref="#PWR0108"  Part="1" 
F 0 "#PWR0108" H 2650 5600 50  0001 C CNN
F 1 "GND" V 2655 5722 50  0000 R CNN
F 2 "" H 2650 5850 50  0001 C CNN
F 3 "" H 2650 5850 50  0001 C CNN
	1    2650 5850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2650 5800 2650 5850
Connection ~ 2650 5850
Wire Wire Line
	2650 5850 2650 5900
Text GLabel 1450 5900 0    50   BiDi ~ 0
OneWire
$Comp
L power:GND #PWR?
U 1 1 5FA54B93
P 5050 6300
AR Path="/5F9C3C99/5FA54B93" Ref="#PWR?"  Part="1" 
AR Path="/5FA54B93" Ref="#PWR0109"  Part="1" 
F 0 "#PWR0109" H 5050 6050 50  0001 C CNN
F 1 "GND" V 5055 6172 50  0000 R CNN
F 2 "" H 5050 6300 50  0001 C CNN
F 3 "" H 5050 6300 50  0001 C CNN
	1    5050 6300
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 5FA5E90C
P 2350 1000
AR Path="/5F9C3C99/5FA5E90C" Ref="#PWR?"  Part="1" 
AR Path="/5FA5E90C" Ref="#PWR0111"  Part="1" 
F 0 "#PWR0111" H 2350 850 50  0001 C CNN
F 1 "+3V3" H 2500 1050 50  0000 C CNN
F 2 "" H 2350 1000 50  0001 C CNN
F 3 "" H 2350 1000 50  0001 C CNN
	1    2350 1000
	1    0    0    -1  
$EndComp
Text GLabel 2950 1800 2    50   BiDi ~ 0
SCLK
Text GLabel 3250 1900 2    50   BiDi ~ 0
LRCLK
Text GLabel 3800 2600 2    50   BiDi ~ 0
SDIN
$Comp
L power:GND #PWR?
U 1 1 5FA6ED8E
P 2350 3800
AR Path="/5F9C3C99/5FA6ED8E" Ref="#PWR?"  Part="1" 
AR Path="/5FA6ED8E" Ref="#PWR0112"  Part="1" 
F 0 "#PWR0112" H 2350 3550 50  0001 C CNN
F 1 "GND" H 2550 3800 50  0000 R CNN
F 2 "" H 2350 3800 50  0001 C CNN
F 3 "" H 2350 3800 50  0001 C CNN
	1    2350 3800
	1    0    0    -1  
$EndComp
$Sheet
S 4400 1400 900  150 
U 5FB2AECE
F0 "psu" 50
F1 "psu.sch" 50
$EndSheet
$Comp
L power:GND #PWR?
U 1 1 5FA4BDFF
P 1450 5200
AR Path="/5F9C3C99/5FA4BDFF" Ref="#PWR?"  Part="1" 
AR Path="/5FA4BDFF" Ref="#PWR0105"  Part="1" 
F 0 "#PWR0105" H 1450 4950 50  0001 C CNN
F 1 "GND" H 1600 5200 50  0000 R CNN
F 2 "" H 1450 5200 50  0001 C CNN
F 3 "" H 1450 5200 50  0001 C CNN
	1    1450 5200
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5FA4ADE2
P 1050 5300
AR Path="/5F9C3C99/5FA4ADE2" Ref="#PWR?"  Part="1" 
AR Path="/5FA4ADE2" Ref="#PWR0104"  Part="1" 
F 0 "#PWR0104" H 1050 5050 50  0001 C CNN
F 1 "GND" V 1055 5172 50  0000 R CNN
F 2 "" H 1050 5300 50  0001 C CNN
F 3 "" H 1050 5300 50  0001 C CNN
	1    1050 5300
	0    1    1    0   
$EndComp
$Comp
L Device:R R?
U 1 1 5FA4983B
P 1050 5150
AR Path="/5F9C3C99/5FA4983B" Ref="R?"  Part="1" 
AR Path="/5FA4983B" Ref="R3"  Part="1" 
F 0 "R3" H 1100 5100 50  0000 L CNN
F 1 "4k7" V 1050 5150 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 980 5150 50  0001 C CNN
F 3 "~" H 1050 5150 50  0001 C CNN
	1    1050 5150
	1    0    0    -1  
$EndComp
NoConn ~ 1050 4900
Wire Wire Line
	900  4600 600  4600
Wire Wire Line
	900  4300 1450 4300
Wire Wire Line
	1450 4500 1450 4300
$Comp
L power:+3V3 #PWR?
U 1 1 5FA43AE5
P 900 4300
AR Path="/5F9C3C99/5FA43AE5" Ref="#PWR?"  Part="1" 
AR Path="/5FA43AE5" Ref="#PWR0102"  Part="1" 
F 0 "#PWR0102" H 900 4150 50  0001 C CNN
F 1 "+3V3" H 915 4473 50  0000 C CNN
F 2 "" H 900 4300 50  0001 C CNN
F 3 "" H 900 4300 50  0001 C CNN
	1    900  4300
	1    0    0    -1  
$EndComp
Connection ~ 900  4300
$Comp
L power:GND #PWR?
U 1 1 5FA43ADC
P 600 4600
AR Path="/5F9C3C99/5FA43ADC" Ref="#PWR?"  Part="1" 
AR Path="/5FA43ADC" Ref="#PWR0101"  Part="1" 
F 0 "#PWR0101" H 600 4350 50  0001 C CNN
F 1 "GND" V 605 4472 50  0000 R CNN
F 2 "" H 600 4600 50  0001 C CNN
F 3 "" H 600 4600 50  0001 C CNN
	1    600  4600
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5FA43AD6
P 900 4450
AR Path="/5F9C3C99/5FA43AD6" Ref="C?"  Part="1" 
AR Path="/5FA43AD6" Ref="C22"  Part="1" 
F 0 "C22" H 1015 4496 50  0000 L CNN
F 1 "u1" H 1015 4405 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 938 4300 50  0001 C CNN
F 3 "~" H 900 4450 50  0001 C CNN
	1    900  4450
	1    0    0    -1  
$EndComp
Text GLabel 1050 4700 0    50   Input ~ 0
CAN_TX
Text GLabel 1050 4800 0    50   Output ~ 0
CAN_RX
$Comp
L Interface_CAN_LIN:SN65HVD230 U?
U 1 1 5FA345E9
P 1450 4800
AR Path="/5FA2399C/5FA345E9" Ref="U?"  Part="1" 
AR Path="/5FA345E9" Ref="U5"  Part="1" 
F 0 "U5" H 1500 5150 50  0000 L CNN
F 1 "SN65HVD230" H 1500 5050 50  0000 L CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_handsolder" H 1450 4300 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn65hvd230.pdf" H 1350 5200 50  0001 C CNN
	1    1450 4800
	1    0    0    -1  
$EndComp
$Sheet
S 4400 1050 900  150 
U 60E30B7A
F0 "eth" 50
F1 "eth.sch" 50
$EndSheet
$Comp
L Device:C C?
U 1 1 60E7476D
P 1100 2200
AR Path="/5F9C3C99/60E7476D" Ref="C?"  Part="1" 
AR Path="/60E7476D" Ref="C32"  Part="1" 
F 0 "C32" H 1215 2246 50  0000 L CNN
F 1 "u1" H 1215 2155 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1138 2050 50  0001 C CNN
F 3 "~" H 1100 2200 50  0001 C CNN
	1    1100 2200
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 60E7514C
P 1450 2200
AR Path="/5F9C3C99/60E7514C" Ref="C?"  Part="1" 
AR Path="/60E7514C" Ref="C33"  Part="1" 
F 0 "C33" H 1565 2246 50  0000 L CNN
F 1 "22u" H 1565 2155 50  0000 L CNN
F 2 "Capacitor_SMD:C_1206_3216Metric_Pad1.33x1.80mm_HandSolder" H 1488 2050 50  0001 C CNN
F 3 "~" H 1450 2200 50  0001 C CNN
	1    1450 2200
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 60E75379
P 1100 2050
AR Path="/5F9C3C99/60E75379" Ref="#PWR?"  Part="1" 
AR Path="/60E75379" Ref="#PWR0113"  Part="1" 
F 0 "#PWR0113" H 1100 1900 50  0001 C CNN
F 1 "+3V3" H 1250 2100 50  0000 C CNN
F 2 "" H 1100 2050 50  0001 C CNN
F 3 "" H 1100 2050 50  0001 C CNN
	1    1100 2050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60E757F5
P 1100 2350
AR Path="/5F9C3C99/60E757F5" Ref="#PWR?"  Part="1" 
AR Path="/60E757F5" Ref="#PWR0114"  Part="1" 
F 0 "#PWR0114" H 1100 2100 50  0001 C CNN
F 1 "GND" V 1105 2222 50  0000 R CNN
F 2 "" H 1100 2350 50  0001 C CNN
F 3 "" H 1100 2350 50  0001 C CNN
	1    1100 2350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1450 2050 1100 2050
Connection ~ 1100 2050
Wire Wire Line
	1450 2350 1100 2350
Connection ~ 1100 2350
$Comp
L power:+24V #PWR0115
U 1 1 60F22851
P 3700 6150
F 0 "#PWR0115" H 3700 6000 50  0001 C CNN
F 1 "+24V" V 3715 6278 50  0000 L CNN
F 2 "" H 3700 6150 50  0001 C CNN
F 3 "" H 3700 6150 50  0001 C CNN
	1    3700 6150
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60F23347
P 4000 6250
AR Path="/5F9C3C99/60F23347" Ref="#PWR?"  Part="1" 
AR Path="/60F23347" Ref="#PWR0116"  Part="1" 
F 0 "#PWR0116" H 4000 6000 50  0001 C CNN
F 1 "GND" V 4005 6122 50  0000 R CNN
F 2 "" H 4000 6250 50  0001 C CNN
F 3 "" H 4000 6250 50  0001 C CNN
	1    4000 6250
	0    1    1    0   
$EndComp
Text GLabel 3050 1200 2    50   Input ~ 0
ETH_CLK
$Comp
L Device:R R?
U 1 1 60E59309
P 3000 900
AR Path="/5F9C3C99/60E59309" Ref="R?"  Part="1" 
AR Path="/60E59309" Ref="R25"  Part="1" 
F 0 "R25" H 3050 850 50  0000 L CNN
F 1 "10k" V 3000 900 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 2930 900 50  0001 C CNN
F 3 "~" H 3000 900 50  0001 C CNN
	1    3000 900 
	1    0    0    -1  
$EndComp
Wire Wire Line
	3000 1050 3000 1100
Wire Wire Line
	3000 1200 3050 1200
Wire Wire Line
	2950 1200 3000 1200
Connection ~ 3000 1200
$Comp
L power:+3V3 #PWR?
U 1 1 60E5A575
P 3000 750
AR Path="/5F9C3C99/60E5A575" Ref="#PWR?"  Part="1" 
AR Path="/60E5A575" Ref="#PWR027"  Part="1" 
F 0 "#PWR027" H 3000 600 50  0001 C CNN
F 1 "+3V3" H 3150 800 50  0000 C CNN
F 2 "" H 3000 750 50  0001 C CNN
F 3 "" H 3000 750 50  0001 C CNN
	1    3000 750 
	1    0    0    -1  
$EndComp
Text GLabel 3050 1100 2    50   Input ~ 0
PROG_IO0
Wire Wire Line
	3050 1100 3000 1100
Connection ~ 3000 1100
Wire Wire Line
	3000 1100 3000 1200
Text GLabel 2950 2400 2    50   BiDi ~ 0
ETH_MDIO
Text GLabel 2950 2800 2    50   BiDi ~ 0
ETH_MDC
Text GLabel 900  1200 0    50   Input ~ 0
EN
$Comp
L Device:C C?
U 1 1 60E6687A
P 1350 1350
AR Path="/5F9C3C99/60E6687A" Ref="C?"  Part="1" 
AR Path="/60E6687A" Ref="C40"  Part="1" 
F 0 "C40" H 1465 1396 50  0000 L CNN
F 1 "u1" H 1465 1305 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.18x1.45mm_HandSolder" H 1388 1200 50  0001 C CNN
F 3 "~" H 1350 1350 50  0001 C CNN
	1    1350 1350
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 1200 1750 1200
$Comp
L power:GND #PWR?
U 1 1 60E678E5
P 1350 1600
AR Path="/5F9C3C99/60E678E5" Ref="#PWR?"  Part="1" 
AR Path="/60E678E5" Ref="#PWR029"  Part="1" 
F 0 "#PWR029" H 1350 1350 50  0001 C CNN
F 1 "GND" V 1355 1472 50  0000 R CNN
F 2 "" H 1350 1600 50  0001 C CNN
F 3 "" H 1350 1600 50  0001 C CNN
	1    1350 1600
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 60E67E71
P 1350 1000
AR Path="/5F9C3C99/60E67E71" Ref="R?"  Part="1" 
AR Path="/60E67E71" Ref="R11"  Part="1" 
F 0 "R11" H 1400 1050 50  0000 L CNN
F 1 "1k" V 1350 1000 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 1280 1000 50  0001 C CNN
F 3 "~" H 1350 1000 50  0001 C CNN
	1    1350 1000
	1    0    0    -1  
$EndComp
Connection ~ 1350 1200
$Comp
L power:+3V3 #PWR?
U 1 1 60E682DD
P 1350 850
AR Path="/5F9C3C99/60E682DD" Ref="#PWR?"  Part="1" 
AR Path="/60E682DD" Ref="#PWR028"  Part="1" 
F 0 "#PWR028" H 1350 700 50  0001 C CNN
F 1 "+3V3" H 1365 1023 50  0000 C CNN
F 2 "" H 1350 850 50  0001 C CNN
F 3 "" H 1350 850 50  0001 C CNN
	1    1350 850 
	1    0    0    -1  
$EndComp
Wire Wire Line
	1350 1200 1350 1150
Text GLabel 1000 2900 0    50   Input ~ 0
EN
Text GLabel 1150 2900 2    50   Input ~ 0
ETH_NRST
Wire Wire Line
	1150 2900 1000 2900
Text GLabel 5050 5500 2    50   BiDi ~ 0
SCL1
Text GLabel 5050 5600 2    50   BiDi ~ 0
SDA1
$Comp
L power:+3V3 #PWR?
U 1 1 60ECDEE3
P 5050 5400
AR Path="/5F9C3C99/60ECDEE3" Ref="#PWR?"  Part="1" 
AR Path="/60ECDEE3" Ref="#PWR0123"  Part="1" 
F 0 "#PWR0123" H 5050 5250 50  0001 C CNN
F 1 "+3V3" V 5050 5600 50  0000 C CNN
F 2 "" H 5050 5400 50  0001 C CNN
F 3 "" H 5050 5400 50  0001 C CNN
	1    5050 5400
	0    1    1    0   
$EndComp
$Comp
L power:+24V #PWR0124
U 1 1 60ECF530
P 5050 6000
F 0 "#PWR0124" H 5050 5850 50  0001 C CNN
F 1 "+24V" V 5065 6128 50  0000 L CNN
F 2 "" H 5050 6000 50  0001 C CNN
F 3 "" H 5050 6000 50  0001 C CNN
	1    5050 6000
	0    1    1    0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60ECF76A
P 5050 6200
AR Path="/5F9C3C99/60ECF76A" Ref="#PWR?"  Part="1" 
AR Path="/60ECF76A" Ref="#PWR0125"  Part="1" 
F 0 "#PWR0125" H 5050 5950 50  0001 C CNN
F 1 "GND" V 5055 6072 50  0000 R CNN
F 2 "" H 5050 6200 50  0001 C CNN
F 3 "" H 5050 6200 50  0001 C CNN
	1    5050 6200
	0    -1   -1   0   
$EndComp
$Comp
L power:+24V #PWR0126
U 1 1 60ECEA5D
P 5050 6100
F 0 "#PWR0126" H 5050 5950 50  0001 C CNN
F 1 "+24V" V 5065 6228 50  0000 L CNN
F 2 "" H 5050 6100 50  0001 C CNN
F 3 "" H 5050 6100 50  0001 C CNN
	1    5050 6100
	0    1    1    0   
$EndComp
Text GLabel 2950 3300 2    50   Output ~ 0
AMP_PDN
$Comp
L Switch:SW_Push SW3
U 1 1 60E7B5F4
P 900 1400
F 0 "SW3" V 946 1352 50  0000 R CNN
F 1 "Reset" V 855 1352 50  0000 R CNN
F 2 "sensact:SW_SPST_2x4x3,5" H 900 1600 50  0001 C CNN
F 3 "~" H 900 1600 50  0001 C CNN
	1    900  1400
	0    -1   1    0   
$EndComp
Wire Wire Line
	1350 1200 900  1200
Wire Wire Line
	1350 1600 900  1600
Wire Wire Line
	1350 1500 1350 1600
Connection ~ 1350 1600
$Comp
L Device:LED_Dual_CACA D1
U 1 1 60E7DF81
P 3750 1500
F 0 "D1" V 3796 1290 50  0000 R CNN
F 1 "Rpower_Ginfo" V 3200 1750 50  0000 R CNN
F 2 "liebler_SEMICONDUCTORS:Dialight-5530123F-0" H 3780 1500 50  0001 C CNN
F 3 "~" H 3780 1500 50  0001 C CNN
	1    3750 1500
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60E875CD
P 3850 2100
AR Path="/5F9C3C99/60E875CD" Ref="#PWR?"  Part="1" 
AR Path="/60E875CD" Ref="#PWR036"  Part="1" 
F 0 "#PWR036" H 3850 1850 50  0001 C CNN
F 1 "GND" V 3855 1972 50  0000 R CNN
F 2 "" H 3850 2100 50  0001 C CNN
F 3 "" H 3850 2100 50  0001 C CNN
	1    3850 2100
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 60E8A738
P 650 1650
AR Path="/5F9C3C99/60E8A738" Ref="R?"  Part="1" 
AR Path="/60E8A738" Ref="R13"  Part="1" 
F 0 "R13" H 500 1750 50  0000 L CNN
F 1 "1k" V 650 1650 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 580 1650 50  0001 C CNN
F 3 "~" H 650 1650 50  0001 C CNN
	1    650  1650
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 60E8AC57
P 1750 1100
AR Path="/5F9C3C99/60E8AC57" Ref="#PWR?"  Part="1" 
AR Path="/60E8AC57" Ref="#PWR034"  Part="1" 
F 0 "#PWR034" H 1750 950 50  0001 C CNN
F 1 "+3V3" H 1765 1273 50  0000 C CNN
F 2 "" H 1750 1100 50  0001 C CNN
F 3 "" H 1750 1100 50  0001 C CNN
	1    1750 1100
	1    0    0    -1  
$EndComp
Connection ~ 650  1800
Wire Wire Line
	650  1800 1450 1800
$Comp
L power:+3V3 #PWR?
U 1 1 60E90BBB
P 650 1500
AR Path="/5F9C3C99/60E90BBB" Ref="#PWR?"  Part="1" 
AR Path="/60E90BBB" Ref="#PWR032"  Part="1" 
F 0 "#PWR032" H 650 1350 50  0001 C CNN
F 1 "+3V3" V 650 1700 50  0000 C CNN
F 2 "" H 650 1500 50  0001 C CNN
F 3 "" H 650 1500 50  0001 C CNN
	1    650  1500
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60E9103B
P 650 2200
AR Path="/5F9C3C99/60E9103B" Ref="#PWR?"  Part="1" 
AR Path="/60E9103B" Ref="#PWR033"  Part="1" 
F 0 "#PWR033" H 650 1950 50  0001 C CNN
F 1 "GND" V 655 2072 50  0000 R CNN
F 2 "" H 650 2200 50  0001 C CNN
F 3 "" H 650 2200 50  0001 C CNN
	1    650  2200
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_02x03_Odd_Even J1
U 1 1 60E91FC7
P 1700 3350
F 0 "J1" H 1750 3667 50  0000 C CNN
F 1 "PROG" H 1750 3576 50  0000 C CNN
F 2 "liebler_CONN:IDC_2x03_P1.27mm_Vertical" H 1700 3350 50  0001 C CNN
F 3 "~" H 1700 3350 50  0001 C CNN
	1    1700 3350
	1    0    0    -1  
$EndComp
Text GLabel 2000 3450 2    50   Input ~ 0
PROG_IO0
$Comp
L power:GND #PWR?
U 1 1 60E92D2C
P 2000 3350
AR Path="/5F9C3C99/60E92D2C" Ref="#PWR?"  Part="1" 
AR Path="/60E92D2C" Ref="#PWR031"  Part="1" 
F 0 "#PWR031" H 2000 3100 50  0001 C CNN
F 1 "GND" V 2005 3222 50  0000 R CNN
F 2 "" H 2000 3350 50  0001 C CNN
F 3 "" H 2000 3350 50  0001 C CNN
	1    2000 3350
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 60E93588
P 2000 3250
AR Path="/5F9C3C99/60E93588" Ref="#PWR?"  Part="1" 
AR Path="/60E93588" Ref="#PWR030"  Part="1" 
F 0 "#PWR030" H 2000 3100 50  0001 C CNN
F 1 "+3V3" V 2000 3450 50  0000 C CNN
F 2 "" H 2000 3250 50  0001 C CNN
F 3 "" H 2000 3250 50  0001 C CNN
	1    2000 3250
	0    1    1    0   
$EndComp
Text GLabel 1150 2800 2    50   Input ~ 0
PROG_EN
Wire Wire Line
	1150 2800 1150 2900
Text GLabel 1500 3250 0    50   BiDi ~ 0
PROG_EN
Text GLabel 1500 3350 0    50   Input ~ 0
PROG_TX
Text GLabel 1500 3450 0    50   Output ~ 0
PROG_RX
Text GLabel 2950 1300 2    50   Output ~ 0
PROG_TX
Text GLabel 2950 1500 2    50   Input ~ 0
PROG_RX
$Comp
L power:+3V3 #PWR?
U 1 1 60E95E6E
P 3850 1200
AR Path="/5F9C3C99/60E95E6E" Ref="#PWR?"  Part="1" 
AR Path="/60E95E6E" Ref="#PWR035"  Part="1" 
F 0 "#PWR035" H 3850 1050 50  0001 C CNN
F 1 "+3V3" H 3865 1373 50  0000 C CNN
F 2 "" H 3850 1200 50  0001 C CNN
F 3 "" H 3850 1200 50  0001 C CNN
	1    3850 1200
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 60E96DC0
P 3850 1950
AR Path="/5F9C3C99/60E96DC0" Ref="R?"  Part="1" 
AR Path="/60E96DC0" Ref="R27"  Part="1" 
F 0 "R27" H 3900 2000 50  0000 L CNN
F 1 "k22" V 3850 1950 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3780 1950 50  0001 C CNN
F 3 "~" H 3850 1950 50  0001 C CNN
	1    3850 1950
	1    0    0    -1  
$EndComp
Wire Wire Line
	3650 1200 3850 1200
$Comp
L Device:R R?
U 1 1 60E9C311
P 3650 1950
AR Path="/5F9C3C99/60E9C311" Ref="R?"  Part="1" 
AR Path="/60E9C311" Ref="R26"  Part="1" 
F 0 "R26" H 3700 2000 50  0000 L CNN
F 1 "k22" V 3650 1950 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 3580 1950 50  0001 C CNN
F 3 "~" H 3650 1950 50  0001 C CNN
	1    3650 1950
	1    0    0    -1  
$EndComp
Text GLabel 2950 1400 2    50   BiDi ~ 0
SDA2
Text GLabel 2950 1700 2    50   BiDi ~ 0
SCL2
$Comp
L Diode:SM712_SOT23 D3
U 1 1 60E9FCBA
P 2300 4850
F 0 "D3" V 2254 4929 50  0000 L CNN
F 1 "NUP2105" V 2345 4929 50  0000 L CNN
F 2 "Package_TO_SOT_SMD:SOT-23_Handsoldering" H 2300 4500 50  0001 C CNN
F 3 "https://www.littelfuse.com/~/media/electronics/datasheets/tvs_diode_arrays/littelfuse_tvs_diode_array_sm712_datasheet.pdf.pdf" H 2150 4850 50  0001 C CNN
	1    2300 4850
	0    1    -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60EA4AEA
P 2150 4850
AR Path="/5F9C3C99/60EA4AEA" Ref="#PWR?"  Part="1" 
AR Path="/60EA4AEA" Ref="#PWR037"  Part="1" 
F 0 "#PWR037" H 2150 4600 50  0001 C CNN
F 1 "GND" V 2155 4722 50  0000 R CNN
F 2 "" H 2150 4850 50  0001 C CNN
F 3 "" H 2150 4850 50  0001 C CNN
	1    2150 4850
	0    1    1    0   
$EndComp
Wire Wire Line
	2300 4500 2050 4500
Wire Wire Line
	2050 4500 2050 4800
Wire Wire Line
	2050 4800 1850 4800
Wire Wire Line
	1850 4900 2050 4900
Wire Wire Line
	2050 4900 2050 5200
Wire Wire Line
	2050 5200 2300 5200
$Comp
L Device:R R?
U 1 1 60EAC8DA
P 4150 4450
AR Path="/5F9C3C99/60EAC8DA" Ref="R?"  Part="1" 
AR Path="/60EAC8DA" Ref="R28"  Part="1" 
F 0 "R28" H 4220 4496 50  0000 L CNN
F 1 "4k7" H 4220 4405 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4080 4450 50  0001 C CNN
F 3 "~" H 4150 4450 50  0001 C CNN
	1    4150 4450
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 60EACD38
P 4450 4450
AR Path="/5F9C3C99/60EACD38" Ref="R?"  Part="1" 
AR Path="/60EACD38" Ref="R29"  Part="1" 
F 0 "R29" H 4520 4496 50  0000 L CNN
F 1 "4k7" H 4520 4405 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.20x1.40mm_HandSolder" V 4380 4450 50  0001 C CNN
F 3 "~" H 4450 4450 50  0001 C CNN
	1    4450 4450
	1    0    0    -1  
$EndComp
Text GLabel 4150 4600 3    50   BiDi ~ 0
SDA2
Text GLabel 4450 4600 3    50   BiDi ~ 0
SCL2
Wire Wire Line
	4450 4300 4150 4300
Connection ~ 3800 4300
Connection ~ 4150 4300
Wire Wire Line
	4150 4300 3800 4300
$Comp
L Switch:SW_Push SW2
U 1 1 60E83176
P 650 2000
F 0 "SW2" V 696 1952 50  0000 R CNN
F 1 "Prog" V 605 1952 50  0000 R CNN
F 2 "Button_Switch_THT:SW_Tactile_SPST_Angled_PTS645Vx58-2LFS" H 650 2200 50  0001 C CNN
F 3 "~" H 650 2200 50  0001 C CNN
	1    650  2000
	0    -1   -1   0   
$EndComp
Text GLabel 3850 5550 0    50   BiDi ~ 0
OneWire
$Comp
L power:GND #PWR?
U 1 1 60E712B8
P 3850 5450
AR Path="/5F9C3C99/60E712B8" Ref="#PWR?"  Part="1" 
AR Path="/60E712B8" Ref="#PWR038"  Part="1" 
F 0 "#PWR038" H 3850 5200 50  0001 C CNN
F 1 "GND" V 3855 5322 50  0000 R CNN
F 2 "" H 3850 5450 50  0001 C CNN
F 3 "" H 3850 5450 50  0001 C CNN
	1    3850 5450
	0    1    1    0   
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J6
U 1 1 60E734CF
P 4050 5550
F 0 "J6" H 4130 5542 50  0000 L CNN
F 1 "1wire" H 4130 5451 50  0000 L CNN
F 2 "Connector_JST:JST_XH_B4B-XH-AM_1x04_P2.50mm_Vertical" H 4050 5550 50  0001 C CNN
F 3 "~" H 4050 5550 50  0001 C CNN
	1    4050 5550
	1    0    0    -1  
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 60E73ED2
P 3850 5650
AR Path="/5F9C3C99/60E73ED2" Ref="#PWR?"  Part="1" 
AR Path="/60E73ED2" Ref="#PWR039"  Part="1" 
F 0 "#PWR039" H 3850 5500 50  0001 C CNN
F 1 "+3V3" V 3850 5900 50  0000 C CNN
F 2 "" H 3850 5650 50  0001 C CNN
F 3 "" H 3850 5650 50  0001 C CNN
	1    3850 5650
	0    -1   -1   0   
$EndComp
$Comp
L power:GND #PWR?
U 1 1 60E74721
P 3850 5750
AR Path="/5F9C3C99/60E74721" Ref="#PWR?"  Part="1" 
AR Path="/60E74721" Ref="#PWR040"  Part="1" 
F 0 "#PWR040" H 3850 5500 50  0001 C CNN
F 1 "GND" V 3855 5622 50  0000 R CNN
F 2 "" H 3850 5750 50  0001 C CNN
F 3 "" H 3850 5750 50  0001 C CNN
	1    3850 5750
	0    1    1    0   
$EndComp
Text GLabel 5050 5800 2    50   Output ~ 0
INT1
Text GLabel 1750 1500 0    50   Input ~ 0
INT0
Text GLabel 2950 3400 2    50   Input ~ 0
INT1
Wire Wire Line
	1450 1400 1450 1800
Wire Wire Line
	1450 1400 1750 1400
Wire Wire Line
	2950 2100 3650 2100
Wire Notes Line
	3400 4050 4700 4050
Wire Notes Line
	4700 4050 4700 4900
Wire Notes Line
	4700 4900 3400 4900
Wire Notes Line
	3400 4900 3400 4050
Text Notes 3400 4050 0    50   ~ 0
I2C Pullup
Wire Wire Line
	2300 5200 2650 5200
Connection ~ 2300 5200
Wire Wire Line
	2650 5200 2650 5250
Wire Wire Line
	2650 5250 3050 5250
Wire Wire Line
	3050 5250 3050 5200
Connection ~ 2650 5200
Wire Wire Line
	2300 4500 2750 4500
Connection ~ 2300 4500
Connection ~ 2750 4500
Wire Wire Line
	2750 4500 3050 4500
NoConn ~ 2850 5200
Wire Notes Line
	500  4050 3350 4050
Wire Notes Line
	3350 5400 500  5400
Wire Notes Line
	500  5400 500  4050
Wire Notes Line
	3350 4050 3350 5400
Text Notes 500  4050 0    50   ~ 0
CAN
Wire Notes Line
	500  6250 500  5500
Text Notes 500  5500 0    50   ~ 0
OneWire
Wire Notes Line
	3350 5500 3350 6250
Wire Notes Line
	3350 6250 500  6250
Wire Notes Line
	500  5500 3350 5500
Wire Notes Line
	500  600  4250 600 
Wire Notes Line
	4250 600  4250 3950
Wire Notes Line
	4250 3950 500  3950
Wire Notes Line
	500  3950 500  600 
Text Notes 500  600  0    50   ~ 0
CPU
Wire Notes Line
	3400 5000 5450 5000
Wire Notes Line
	5450 5000 5450 6500
Wire Notes Line
	5450 6500 3400 6500
Wire Notes Line
	3400 6500 3400 5000
Text Notes 3400 5000 0    50   ~ 0
Connectors
Connection ~ 3850 1200
Text GLabel 5050 5700 2    50   Output ~ 0
INT0
NoConn ~ 5050 5900
$Comp
L Device:D_Schottky D4
U 1 1 60E866C2
P 3850 6150
F 0 "D4" H 3850 5850 50  0000 C CNN
F 1 "2A" H 3850 5950 50  0000 C CNN
F 2 "Diode_SMD:D_SMB_Handsoldering" H 3850 6150 50  0001 C CNN
F 3 "~" H 3850 6150 50  0001 C CNN
	1    3850 6150
	1    0    0    -1  
$EndComp
$Sheet
S 4400 1800 900  150 
U 60E9D32B
F0 "ebus" 50
F1 "ebus.sch" 50
$EndSheet
$Comp
L Connector_Generic:Conn_01x04 J?
U 1 1 60F35FFB
P 6450 5500
AR Path="/5F9C3C99/60F35FFB" Ref="J?"  Part="1" 
AR Path="/60F35FFB" Ref="J5"  Part="1" 
F 0 "J5" H 6368 5075 50  0000 C CNN
F 1 "Audio" H 6368 5166 50  0000 C CNN
F 2 "Connector_Phoenix_MC:PhoenixContact_MC_1,5_4-G-3.5_1x04_P3.50mm_Horizontal" H 6450 5500 50  0001 C CNN
F 3 "~" H 6450 5500 50  0001 C CNN
	1    6450 5500
	1    0    0    1   
$EndComp
Text GLabel 6250 5300 0    50   BiDi ~ 0
OUT_A+
Text GLabel 6250 5400 0    50   BiDi ~ 0
OUT_A-
Text GLabel 6250 5500 0    50   BiDi ~ 0
OUT_B-
Text GLabel 6250 5600 0    50   BiDi ~ 0
OUT_B+
Text GLabel 3250 1800 2    50   BiDi ~ 0
EB_TX
Wire Wire Line
	2950 1900 3250 1900
Wire Wire Line
	3250 1800 3250 1900
Text GLabel 3800 2700 2    50   BiDi ~ 0
EB_RX
Wire Wire Line
	3800 2600 3800 2700
Wire Wire Line
	3800 2600 3450 2600
Wire Wire Line
	3450 2600 3450 2000
Wire Wire Line
	3450 2000 2950 2000
$EndSCHEMATC
