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
S 950  850  950  750 
U 5F9C3C99
F0 "audioamplifier" 50
F1 "audioamplifier.sch" 50
$EndSheet
$Comp
L Connector_Generic:Conn_01x08 J1
U 1 1 5FA08532
P 9100 4550
F 0 "J1" H 9180 4542 50  0000 L CNN
F 1 "Conn_01x08" H 9180 4451 50  0000 L CNN
F 2 "sensact:PhoenixContact_MC_0,5_8-G-2.5_1x08_P2.50mm_Horizontal" H 9100 4550 50  0001 C CNN
F 3 "~" H 9100 4550 50  0001 C CNN
	1    9100 4550
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x04 J3
U 1 1 5FA1B92C
P 10550 2950
F 0 "J3" H 10630 2942 50  0000 L CNN
F 1 "Conn_01x04" H 10630 2851 50  0000 L CNN
F 2 "sensact:PhoenixContact_MC_0,5_4-G-2.5_1x04_P2.50mm_Horizontal" H 10550 2950 50  0001 C CNN
F 3 "~" H 10550 2950 50  0001 C CNN
	1    10550 2950
	1    0    0    -1  
$EndComp
$Comp
L sensact:SensactBusConnector J2
U 1 1 5FA1CB9E
P 10200 4650
F 0 "J2" H 10258 5415 50  0000 C CNN
F 1 "SensactBusConnector" H 10258 5324 50  0000 C CNN
F 2 "Connector_IDC:IDC-Header_2x05_P2.54mm_Horizontal" H 10000 4650 50  0001 C CNN
F 3 "" H 10000 4650 50  0001 C CNN
	1    10200 4650
	1    0    0    -1  
$EndComp
$Sheet
S 2000 850  1000 750 
U 5FA2399C
F0 "ethswitch" 50
F1 "ethswitch.sch" 50
$EndSheet
$Comp
L RF_Module:ESP32-WROOM-32U U?
U 1 1 5FA345D1
P 1700 3600
AR Path="/5FA2399C/5FA345D1" Ref="U?"  Part="1" 
AR Path="/5FA345D1" Ref="U3"  Part="1" 
F 0 "U3" H 1600 5050 50  0000 R CNN
F 1 "ESP32-WROOM-32U" H 1600 4950 50  0000 R CNN
F 2 "RF_Module:ESP32-WROOM-32U" H 1700 2100 50  0001 C CNN
F 3 "https://www.espressif.com/sites/default/files/documentation/esp32-wroom-32d_esp32-wroom-32u_datasheet_en.pdf" H 1400 3650 50  0001 C CNN
	1    1700 3600
	1    0    0    -1  
$EndComp
Text GLabel 2300 2400 2    50   Input ~ 0
ETH_REF_CLK
Text GLabel 2300 3500 2    50   Output ~ 0
ETH_POWER
Text GLabel 2300 3700 2    50   Output ~ 0
ETH_TXD0
Text GLabel 2300 3800 2    50   Output ~ 0
ETH_TX_EN
Text GLabel 2300 3900 2    50   Output ~ 0
ETH_TXD1
Text GLabel 2300 4100 2    50   Input ~ 0
ETH_RXD0
Text GLabel 2300 4200 2    50   Input ~ 0
ETH_RXD1
Text GLabel 2300 4300 2    50   Input ~ 0
ETH_CRS_DV
Text GLabel 2300 2800 2    50   BiDi ~ 0
SDA1
Text GLabel 2300 3400 2    50   BiDi ~ 0
SCL1
NoConn ~ 1100 4100
NoConn ~ 1100 4000
NoConn ~ 1100 3900
NoConn ~ 1100 3800
NoConn ~ 1100 3700
NoConn ~ 1100 3600
Text GLabel 2300 4700 2    50   Input ~ 0
CAN_RX
Text GLabel 2300 4400 2    50   Output ~ 0
CAN_TX
$Comp
L sensact:DS2482 U?
U 1 1 5FA345F1
P 3750 6050
AR Path="/5FA2399C/5FA345F1" Ref="U?"  Part="1" 
AR Path="/5FA345F1" Ref="U4"  Part="1" 
F 0 "U4" H 3750 6553 60  0000 C CNN
F 1 "DS2482" H 3750 6447 60  0000 C CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_handsolder" H 3750 6341 60  0000 C CNN
F 3 "" H 3750 6050 60  0000 C CNN
	1    3750 6050
	1    0    0    -1  
$EndComp
Text GLabel 3150 6200 0    50   BiDi ~ 0
SCL1
Text GLabel 4350 6200 2    50   BiDi ~ 0
SDA1
Text GLabel 4600 4450 2    50   BiDi ~ 0
CANH
Text GLabel 4600 4550 2    50   BiDi ~ 0
CANL
Text GLabel 8900 4350 0    50   BiDi ~ 0
CANL
Text GLabel 8900 4250 0    50   BiDi ~ 0
CANH
$Comp
L Device:R R?
U 1 1 5FA49518
P 3600 2700
AR Path="/5F9C3C99/5FA49518" Ref="R?"  Part="1" 
AR Path="/5FA49518" Ref="R1"  Part="1" 
F 0 "R1" H 3670 2746 50  0000 L CNN
F 1 "4k7" H 3670 2655 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 3530 2700 50  0001 C CNN
F 3 "~" H 3600 2700 50  0001 C CNN
	1    3600 2700
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5FA4951E
P 3900 2700
AR Path="/5F9C3C99/5FA4951E" Ref="R?"  Part="1" 
AR Path="/5FA4951E" Ref="R6"  Part="1" 
F 0 "R6" H 3970 2746 50  0000 L CNN
F 1 "4k7" H 3970 2655 50  0000 L CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 3830 2700 50  0001 C CNN
F 3 "~" H 3900 2700 50  0001 C CNN
	1    3900 2700
	1    0    0    -1  
$EndComp
Wire Wire Line
	3900 2550 3600 2550
$Comp
L power:+3V3 #PWR?
U 1 1 5FA49527
P 3600 2550
AR Path="/5F9C3C99/5FA49527" Ref="#PWR?"  Part="1" 
AR Path="/5FA49527" Ref="#PWR0103"  Part="1" 
F 0 "#PWR0103" H 3600 2400 50  0001 C CNN
F 1 "+3V3" H 3615 2723 50  0000 C CNN
F 2 "" H 3600 2550 50  0001 C CNN
F 3 "" H 3600 2550 50  0001 C CNN
	1    3600 2550
	1    0    0    -1  
$EndComp
Connection ~ 3600 2550
Text GLabel 3600 2850 3    50   BiDi ~ 0
SDA1
Text GLabel 3900 2850 3    50   BiDi ~ 0
SCL1
$Comp
L Device:R R?
U 1 1 5FA4C417
P 4700 4950
AR Path="/5F9C3C99/5FA4C417" Ref="R?"  Part="1" 
AR Path="/5FA4C417" Ref="R7"  Part="1" 
F 0 "R7" H 4750 4900 50  0000 L CNN
F 1 "120R" V 4700 4950 50  0000 C CNN
F 2 "Resistor_SMD:R_1206_3216Metric_Pad1.42x1.75mm_HandSolder" V 4630 4950 50  0001 C CNN
F 3 "~" H 4700 4950 50  0001 C CNN
	1    4700 4950
	1    0    0    -1  
$EndComp
$Comp
L Switch:SW_SPDT SW1
U 1 1 5FA4E5BD
P 4900 4800
F 0 "SW1" H 5050 5000 50  0000 C CNN
F 1 "TERM" H 5000 4750 50  0000 C CNN
F 2 "Button_Switch_SMD:SW_SPDT_PCM12" H 4900 4800 50  0001 C CNN
F 3 "~" H 4900 4800 50  0001 C CNN
	1    4900 4800
	1    0    0    -1  
$EndComp
Text GLabel 4700 5100 3    50   BiDi ~ 0
CANH
Text GLabel 5100 4900 2    50   BiDi ~ 0
CANL
NoConn ~ 5100 4700
$Comp
L Device:C C?
U 1 1 5FA5128B
P 2300 6050
AR Path="/5F9C3C99/5FA5128B" Ref="C?"  Part="1" 
AR Path="/5FA5128B" Ref="C19"  Part="1" 
F 0 "C19" H 2415 6096 50  0000 L CNN
F 1 "1u" H 2415 6005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2338 5900 50  0001 C CNN
F 3 "~" H 2300 6050 50  0001 C CNN
	1    2300 6050
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5FA51295
P 2600 6050
AR Path="/5F9C3C99/5FA51295" Ref="C?"  Part="1" 
AR Path="/5FA51295" Ref="C20"  Part="1" 
F 0 "C20" H 2715 6096 50  0000 L CNN
F 1 "u1" H 2715 6005 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 2638 5900 50  0001 C CNN
F 3 "~" H 2600 6050 50  0001 C CNN
	1    2600 6050
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5FA5129F
P 2300 6200
AR Path="/5F9C3C99/5FA5129F" Ref="#PWR?"  Part="1" 
AR Path="/5FA5129F" Ref="#PWR0106"  Part="1" 
F 0 "#PWR0106" H 2300 5950 50  0001 C CNN
F 1 "GND" V 2305 6072 50  0000 R CNN
F 2 "" H 2300 6200 50  0001 C CNN
F 3 "" H 2300 6200 50  0001 C CNN
	1    2300 6200
	1    0    0    -1  
$EndComp
Wire Wire Line
	2300 5900 2600 5900
Connection ~ 2600 5900
$Comp
L power:+3V3 #PWR?
U 1 1 5FA512AB
P 2300 5900
AR Path="/5F9C3C99/5FA512AB" Ref="#PWR?"  Part="1" 
AR Path="/5FA512AB" Ref="#PWR0107"  Part="1" 
F 0 "#PWR0107" H 2300 5750 50  0001 C CNN
F 1 "+3V3" H 2315 6073 50  0000 C CNN
F 2 "" H 2300 5900 50  0001 C CNN
F 3 "" H 2300 5900 50  0001 C CNN
	1    2300 5900
	1    0    0    -1  
$EndComp
Connection ~ 2300 5900
Wire Wire Line
	2600 5900 3150 5900
Wire Wire Line
	2600 6200 2300 6200
Connection ~ 2300 6200
Wire Wire Line
	3150 6100 2850 6100
Wire Wire Line
	2850 6100 2850 6200
Wire Wire Line
	2850 6200 2600 6200
Connection ~ 2600 6200
NoConn ~ 4350 6100
$Comp
L power:GND #PWR?
U 1 1 5FA53BEC
P 4350 5950
AR Path="/5F9C3C99/5FA53BEC" Ref="#PWR?"  Part="1" 
AR Path="/5FA53BEC" Ref="#PWR0108"  Part="1" 
F 0 "#PWR0108" H 4350 5700 50  0001 C CNN
F 1 "GND" V 4355 5822 50  0000 R CNN
F 2 "" H 4350 5950 50  0001 C CNN
F 3 "" H 4350 5950 50  0001 C CNN
	1    4350 5950
	0    -1   -1   0   
$EndComp
Wire Wire Line
	4350 5900 4350 5950
Connection ~ 4350 5950
Wire Wire Line
	4350 5950 4350 6000
Text GLabel 3150 6000 0    50   BiDi ~ 0
OneWire
$Comp
L power:GND #PWR?
U 1 1 5FA54B93
P 8900 4950
AR Path="/5F9C3C99/5FA54B93" Ref="#PWR?"  Part="1" 
AR Path="/5FA54B93" Ref="#PWR0109"  Part="1" 
F 0 "#PWR0109" H 8900 4700 50  0001 C CNN
F 1 "GND" V 8905 4822 50  0000 R CNN
F 2 "" H 8900 4950 50  0001 C CNN
F 3 "" H 8900 4950 50  0001 C CNN
	1    8900 4950
	0    1    1    0   
$EndComp
Text GLabel 8900 4850 0    50   BiDi ~ 0
OneWire
$Comp
L power:+3V3 #PWR?
U 1 1 5FA5D523
P 8900 4750
AR Path="/5F9C3C99/5FA5D523" Ref="#PWR?"  Part="1" 
AR Path="/5FA5D523" Ref="#PWR0110"  Part="1" 
F 0 "#PWR0110" H 8900 4600 50  0001 C CNN
F 1 "+3V3" H 8915 4923 50  0000 C CNN
F 2 "" H 8900 4750 50  0001 C CNN
F 3 "" H 8900 4750 50  0001 C CNN
	1    8900 4750
	0    -1   -1   0   
$EndComp
$Comp
L power:+3V3 #PWR?
U 1 1 5FA5E90C
P 1700 2200
AR Path="/5F9C3C99/5FA5E90C" Ref="#PWR?"  Part="1" 
AR Path="/5FA5E90C" Ref="#PWR0111"  Part="1" 
F 0 "#PWR0111" H 1700 2050 50  0001 C CNN
F 1 "+3V3" H 1850 2250 50  0000 C CNN
F 2 "" H 1700 2200 50  0001 C CNN
F 3 "" H 1700 2200 50  0001 C CNN
	1    1700 2200
	1    0    0    -1  
$EndComp
Text GLabel 2300 3000 2    50   BiDi ~ 0
SCLK
Text GLabel 2300 3100 2    50   BiDi ~ 0
LRCLK
Text GLabel 2300 3200 2    50   BiDi ~ 0
SDIN
$Comp
L power:GND #PWR?
U 1 1 5FA6ED8E
P 1700 5000
AR Path="/5F9C3C99/5FA6ED8E" Ref="#PWR?"  Part="1" 
AR Path="/5FA6ED8E" Ref="#PWR0112"  Part="1" 
F 0 "#PWR0112" H 1700 4750 50  0001 C CNN
F 1 "GND" V 1705 4872 50  0000 R CNN
F 2 "" H 1700 5000 50  0001 C CNN
F 3 "" H 1700 5000 50  0001 C CNN
	1    1700 5000
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J8
U 1 1 5FAD4781
P 5800 2850
F 0 "J8" H 5880 2842 50  0000 L CNN
F 1 "Conn_01x08" H 5880 2751 50  0000 L CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_2x04_P1.27mm_Horizontal" H 5800 2850 50  0001 C CNN
F 3 "~" H 5800 2850 50  0001 C CNN
	1    5800 2850
	1    0    0    -1  
$EndComp
$Comp
L Connector_Generic:Conn_01x08 J9
U 1 1 5FAD4C7E
P 5800 3800
F 0 "J9" H 5880 3792 50  0000 L CNN
F 1 "Conn_01x08" H 5880 3701 50  0000 L CNN
F 2 "Connector_PinHeader_1.27mm:PinHeader_2x04_P1.27mm_Horizontal" H 5800 3800 50  0001 C CNN
F 3 "~" H 5800 3800 50  0001 C CNN
	1    5800 3800
	1    0    0    -1  
$EndComp
$Comp
L power:+3.3V #PWR0117
U 1 1 5FB107B3
P 5600 4950
F 0 "#PWR0117" H 5600 4800 50  0001 C CNN
F 1 "+3.3V" V 5615 5078 50  0000 L CNN
F 2 "" H 5600 4950 50  0001 C CNN
F 3 "" H 5600 4950 50  0001 C CNN
	1    5600 4950
	0    1    1    0   
$EndComp
$Sheet
S 3100 850  550  750 
U 5FB2AECE
F0 "psu" 50
F1 "psu.sch" 50
$EndSheet
$Comp
L power:GND #PWR?
U 1 1 5FA4BDFF
P 4200 4850
AR Path="/5F9C3C99/5FA4BDFF" Ref="#PWR?"  Part="1" 
AR Path="/5FA4BDFF" Ref="#PWR0105"  Part="1" 
F 0 "#PWR0105" H 4200 4600 50  0001 C CNN
F 1 "GND" V 4205 4722 50  0000 R CNN
F 2 "" H 4200 4850 50  0001 C CNN
F 3 "" H 4200 4850 50  0001 C CNN
	1    4200 4850
	1    0    0    -1  
$EndComp
$Comp
L power:GND #PWR?
U 1 1 5FA4ADE2
P 3800 4950
AR Path="/5F9C3C99/5FA4ADE2" Ref="#PWR?"  Part="1" 
AR Path="/5FA4ADE2" Ref="#PWR0104"  Part="1" 
F 0 "#PWR0104" H 3800 4700 50  0001 C CNN
F 1 "GND" V 3805 4822 50  0000 R CNN
F 2 "" H 3800 4950 50  0001 C CNN
F 3 "" H 3800 4950 50  0001 C CNN
	1    3800 4950
	1    0    0    -1  
$EndComp
$Comp
L Device:R R?
U 1 1 5FA4983B
P 3800 4800
AR Path="/5F9C3C99/5FA4983B" Ref="R?"  Part="1" 
AR Path="/5FA4983B" Ref="R3"  Part="1" 
F 0 "R3" H 3850 4750 50  0000 L CNN
F 1 "4k7" V 3800 4800 50  0000 C CNN
F 2 "Resistor_SMD:R_0805_2012Metric_Pad1.15x1.40mm_HandSolder" V 3730 4800 50  0001 C CNN
F 3 "~" H 3800 4800 50  0001 C CNN
	1    3800 4800
	1    0    0    -1  
$EndComp
NoConn ~ 3800 4550
Wire Wire Line
	3650 4250 3350 4250
Wire Wire Line
	3650 3950 4200 3950
Wire Wire Line
	4200 4150 4200 3950
$Comp
L power:+3V3 #PWR?
U 1 1 5FA43AE5
P 3350 3950
AR Path="/5F9C3C99/5FA43AE5" Ref="#PWR?"  Part="1" 
AR Path="/5FA43AE5" Ref="#PWR0102"  Part="1" 
F 0 "#PWR0102" H 3350 3800 50  0001 C CNN
F 1 "+3V3" H 3365 4123 50  0000 C CNN
F 2 "" H 3350 3950 50  0001 C CNN
F 3 "" H 3350 3950 50  0001 C CNN
	1    3350 3950
	1    0    0    -1  
$EndComp
Connection ~ 3650 3950
Wire Wire Line
	3350 3950 3650 3950
$Comp
L power:GND #PWR?
U 1 1 5FA43ADC
P 3350 4250
AR Path="/5F9C3C99/5FA43ADC" Ref="#PWR?"  Part="1" 
AR Path="/5FA43ADC" Ref="#PWR0101"  Part="1" 
F 0 "#PWR0101" H 3350 4000 50  0001 C CNN
F 1 "GND" V 3355 4122 50  0000 R CNN
F 2 "" H 3350 4250 50  0001 C CNN
F 3 "" H 3350 4250 50  0001 C CNN
	1    3350 4250
	1    0    0    -1  
$EndComp
$Comp
L Device:C C?
U 1 1 5FA43AD6
P 3650 4100
AR Path="/5F9C3C99/5FA43AD6" Ref="C?"  Part="1" 
AR Path="/5FA43AD6" Ref="C22"  Part="1" 
F 0 "C22" H 3765 4146 50  0000 L CNN
F 1 "u1" H 3765 4055 50  0000 L CNN
F 2 "Capacitor_SMD:C_0805_2012Metric_Pad1.15x1.40mm_HandSolder" H 3688 3950 50  0001 C CNN
F 3 "~" H 3650 4100 50  0001 C CNN
	1    3650 4100
	1    0    0    -1  
$EndComp
Text GLabel 3800 4350 0    50   Input ~ 0
CAN_TX
Text GLabel 3800 4450 0    50   Output ~ 0
CAN_RX
$Comp
L Interface_CAN_LIN:SN65HVD230 U?
U 1 1 5FA345E9
P 4200 4450
AR Path="/5FA2399C/5FA345E9" Ref="U?"  Part="1" 
AR Path="/5FA345E9" Ref="U5"  Part="1" 
F 0 "U5" H 4250 4800 50  0000 L CNN
F 1 "SN65HVD230" H 4250 4700 50  0000 L CNN
F 2 "sensact:SOIC-8_3.9x4.9mm_Pitch1.27mm_handsolder" H 4200 3950 50  0001 C CNN
F 3 "http://www.ti.com/lit/ds/symlink/sn65hvd230.pdf" H 4100 4850 50  0001 C CNN
	1    4200 4450
	1    0    0    -1  
$EndComp
$Sheet
S 3900 850  700  750 
U 5FF21AE2
F0 "top_hmi" 50
F1 "top_hmi.sch" 50
$EndSheet
$EndSCHEMATC