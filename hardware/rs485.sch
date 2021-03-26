EESchema Schematic File Version 4
EELAYER 30 0
EELAYER END
$Descr A4 11693 8268
encoding utf-8
Sheet 1 1
Title "RS485 Bricklet"
Date "2021-03-26"
Rev "1.1"
Comp "Tinkerforge GmbH"
Comment1 "Licensed under CERN OHL v.1.1"
Comment2 "Copyright (©) 2021, B.Nordmeyer <bastian@tinkerforge.com>"
Comment3 ""
Comment4 ""
$EndDescr
Text Notes 550  7700 0    40   ~ 0
Copyright Tinkerforge GmbH 2016.\nThis documentation describes Open Hardware and is licensed under the\nCERN OHL v. 1.1.\nYou may redistribute and modify this documentation under the terms of the\nCERN OHL v.1.1. (http://ohwr.org/cernohl). This documentation is distributed\nWITHOUT ANY EXPRESS OR IMPLIED WARRANTY, INCLUDING OF\nMERCHANTABILITY, SATISFACTORY QUALITY AND FITNESS FOR A\nPARTICULAR PURPOSE. Please see the CERN OHL v.1.1 for applicable\nconditions\n
$Comp
L tinkerforge:DRILL U2
U 1 1 4C6050A5
P 10650 6150
F 0 "U2" H 10700 6200 60  0001 C CNN
F 1 "DRILL" H 10650 6150 60  0000 C CNN
F 2 "DRILL_NP" H 10650 6150 60  0001 C CNN
F 3 "" H 10650 6150 60  0001 C CNN
	1    10650 6150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL U3
U 1 1 4C6050A2
P 10650 6350
F 0 "U3" H 10700 6400 60  0001 C CNN
F 1 "DRILL" H 10650 6350 60  0000 C CNN
F 2 "DRILL_NP" H 10650 6350 60  0001 C CNN
F 3 "" H 10650 6350 60  0001 C CNN
	1    10650 6350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL U5
U 1 1 4C60509F
P 11000 6350
F 0 "U5" H 11050 6400 60  0001 C CNN
F 1 "DRILL" H 11000 6350 60  0000 C CNN
F 2 "DRILL_NP" H 11000 6350 60  0001 C CNN
F 3 "" H 11000 6350 60  0001 C CNN
	1    11000 6350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:DRILL U4
U 1 1 4C605099
P 11000 6150
F 0 "U4" H 11050 6200 60  0001 C CNN
F 1 "DRILL" H 11000 6150 60  0000 C CNN
F 2 "DRILL_NP" H 11000 6150 60  0001 C CNN
F 3 "" H 11000 6150 60  0001 C CNN
	1    11000 6150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR01
U 1 1 4C5FCFB4
P 2550 1150
F 0 "#PWR01" H 2550 1250 30  0001 C CNN
F 1 "VCC" H 2550 1250 30  0000 C CNN
F 2 "" H 2550 1150 60  0001 C CNN
F 3 "" H 2550 1150 60  0001 C CNN
	1    2550 1150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR02
U 1 1 4C5FCF4F
P 1000 2550
F 0 "#PWR02" H 1000 2550 30  0001 C CNN
F 1 "GND" H 1000 2480 30  0001 C CNN
F 2 "" H 1000 2550 60  0001 C CNN
F 3 "" H 1000 2550 60  0001 C CNN
	1    1000 2550
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:CON-SENSOR2 P1
U 1 1 4C5FCF27
P 1000 2000
F 0 "P1" H 1150 2400 60  0000 C CNN
F 1 "CON-SENSOR2" V 1150 2000 60  0000 C CNN
F 2 "CON-SENSOR2" H 1000 2000 60  0001 C CNN
F 3 "" H 1000 2000 60  0001 C CNN
	1    1000 2000
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR03
U 1 1 54F75907
P 10650 4350
F 0 "#PWR03" H 10650 4350 30  0001 C CNN
F 1 "GND" H 10650 4280 30  0001 C CNN
F 2 "" H 10650 4350 60  0001 C CNN
F 3 "" H 10650 4350 60  0001 C CNN
	1    10650 4350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:INDUCTOR FB1
U 1 1 54F7633D
P 1950 1150
F 0 "FB1" H 1950 1300 60  0000 C CNN
F 1 "FILTER" H 1950 1050 60  0000 C CNN
F 2 "kicad-libraries:R0603F" H 1950 1150 60  0001 C CNN
F 3 "" H 1950 1150 60  0000 C CNN
	1    1950 1150
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C1
U 1 1 54F76B96
P 1600 1400
F 0 "C1" V 1700 1550 50  0000 L CNN
F 1 "1uF" V 1450 1400 50  0000 L CNN
F 2 "C0603F" H 1600 1400 60  0001 C CNN
F 3 "" H 1600 1400 60  0001 C CNN
	1    1600 1400
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:C C2
U 1 1 54F77AA5
P 2350 1400
F 0 "C2" V 2450 1550 50  0000 L CNN
F 1 "1uF" V 2200 1400 50  0000 L CNN
F 2 "C0603F" H 2350 1400 60  0001 C CNN
F 3 "" H 2350 1400 60  0001 C CNN
	1    2350 1400
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:GND #PWR04
U 1 1 54F77AEA
P 2350 1850
F 0 "#PWR04" H 2350 1850 30  0001 C CNN
F 1 "GND" H 2350 1780 30  0001 C CNN
F 2 "" H 2350 1850 60  0001 C CNN
F 3 "" H 2350 1850 60  0001 C CNN
	1    2350 1850
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 2 1 5820E01A
P 4350 4550
F 0 "U1" H 4200 5500 60  0000 C CNN
F 1 "XMC1404" H 4350 3550 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 4500 5300 60  0001 C CNN
F 3 "" H 4500 5300 60  0000 C CNN
	2    4350 4550
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 1 1 5820E0F1
P 4350 2400
F 0 "U1" H 4200 3350 60  0000 C CNN
F 1 "XMC1404" H 4350 1400 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 4500 3150 60  0001 C CNN
F 3 "" H 4500 3150 60  0000 C CNN
	1    4350 2400
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 4 1 5820E19E
P 6150 6050
F 0 "U1" H 6000 6700 60  0000 C CNN
F 1 "XMC1404" H 6150 5050 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 6300 6800 60  0001 C CNN
F 3 "" H 6300 6800 60  0000 C CNN
	4    6150 6050
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 3 1 5820E1ED
P 6150 4750
F 0 "U1" H 6050 5250 60  0000 C CNN
F 1 "XMC1404" H 6150 4250 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 6300 5500 60  0001 C CNN
F 3 "" H 6300 5500 60  0000 C CNN
	3    6150 4750
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:XMC1XXX48 U1
U 5 1 5820E256
P 4400 6250
F 0 "U1" H 4250 6700 60  0000 C CNN
F 1 "XMC1404" H 4400 5650 60  0000 C CNN
F 2 "kicad-libraries:QFN48-EP2" H 4550 7000 60  0001 C CNN
F 3 "" H 4550 7000 60  0000 C CNN
	5    4400 6250
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C3
U 1 1 5820F9DC
P 3550 2000
F 0 "C3" V 3600 1750 50  0000 L CNN
F 1 "100nF" V 3400 1900 50  0000 L CNN
F 2 "C0603F" H 3550 2000 60  0001 C CNN
F 3 "" H 3550 2000 60  0001 C CNN
	1    3550 2000
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C5
U 1 1 5820FDE6
P 3900 2000
F 0 "C5" V 3950 1750 50  0000 L CNN
F 1 "100nF" V 3750 1900 50  0000 L CNN
F 2 "C0603F" H 3900 2000 60  0001 C CNN
F 3 "" H 3900 2000 60  0001 C CNN
	1    3900 2000
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C6
U 1 1 5821039E
P 3900 2950
F 0 "C6" V 3950 2700 50  0000 L CNN
F 1 "220nF" V 3750 2850 50  0000 L CNN
F 2 "C0603F" H 3900 2950 60  0001 C CNN
F 3 "" H 3900 2950 60  0001 C CNN
	1    3900 2950
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C4
U 1 1 582104B4
P 3550 2950
F 0 "C4" V 3600 2700 50  0000 L CNN
F 1 "100nF" V 3400 2850 50  0000 L CNN
F 2 "C0603F" H 3550 2950 60  0001 C CNN
F 3 "" H 3550 2950 60  0001 C CNN
	1    3550 2950
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR05
U 1 1 5821096B
P 3550 1600
F 0 "#PWR05" H 3550 1700 30  0001 C CNN
F 1 "VCC" H 3550 1700 30  0000 C CNN
F 2 "" H 3550 1600 60  0001 C CNN
F 3 "" H 3550 1600 60  0001 C CNN
	1    3550 1600
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR06
U 1 1 58210A4E
P 3550 2550
F 0 "#PWR06" H 3550 2650 30  0001 C CNN
F 1 "VCC" H 3550 2650 30  0000 C CNN
F 2 "" H 3550 2550 60  0001 C CNN
F 3 "" H 3550 2550 60  0001 C CNN
	1    3550 2550
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR07
U 1 1 58210B67
P 3550 2300
F 0 "#PWR07" H 3550 2300 30  0001 C CNN
F 1 "GND" H 3550 2230 30  0001 C CNN
F 2 "" H 3550 2300 60  0001 C CNN
F 3 "" H 3550 2300 60  0001 C CNN
	1    3550 2300
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR08
U 1 1 58210C80
P 3550 3250
F 0 "#PWR08" H 3550 3250 30  0001 C CNN
F 1 "GND" H 3550 3180 30  0001 C CNN
F 2 "" H 3550 3250 60  0001 C CNN
F 3 "" H 3550 3250 60  0001 C CNN
	1    3550 3250
	1    0    0    -1  
$EndComp
Text GLabel 2150 2000 2    60   Output ~ 0
S-CS
Text GLabel 2150 2100 2    60   Output ~ 0
S-CLK
Text GLabel 2150 2200 2    60   Output ~ 0
S-MOSI
Text GLabel 2150 2300 2    60   Input ~ 0
S-MISO
$Comp
L tinkerforge:GND #PWR09
U 1 1 582328E4
P 1500 2750
F 0 "#PWR09" H 1500 2750 30  0001 C CNN
F 1 "GND" H 1500 2680 30  0001 C CNN
F 2 "" H 1500 2750 60  0001 C CNN
F 3 "" H 1500 2750 60  0001 C CNN
	1    1500 2750
	1    0    0    -1  
$EndComp
Text GLabel 5450 4550 0    60   Input ~ 0
S-CS
Text GLabel 5450 4750 0    60   Input ~ 0
S-CLK
Text GLabel 5450 4650 0    60   Input ~ 0
S-MOSI
Text GLabel 5450 5050 0    60   Output ~ 0
S-MISO
$Comp
L tinkerforge:LED D1
U 1 1 582331F0
P 2850 5800
F 0 "D1" H 2850 5900 50  0000 C CNN
F 1 "yellow" H 2850 5700 50  0000 C CNN
F 2 "D0603E" H 2850 5800 50  0001 C CNN
F 3 "" H 2850 5800 50  0000 C CNN
	1    2850 5800
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:LED D2
U 1 1 58233423
P 2850 6100
F 0 "D2" H 2850 6200 50  0000 C CNN
F 1 "red" H 2850 6000 50  0000 C CNN
F 2 "D0603E" H 2850 6100 50  0001 C CNN
F 3 "" H 2850 6100 50  0000 C CNN
	1    2850 6100
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:LED D3
U 1 1 5823347E
P 2850 6400
F 0 "D3" H 2850 6500 50  0000 C CNN
F 1 "blue" H 2850 6300 50  0000 C CNN
F 2 "D0603E" H 2850 6400 50  0001 C CNN
F 3 "" H 2850 6400 50  0000 C CNN
	1    2850 6400
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:CONN_01X02 P2
U 1 1 58233528
P 3550 6600
F 0 "P2" H 3550 6750 50  0000 C CNN
F 1 "BOOT" V 3650 6600 50  0000 C CNN
F 2 "kicad-libraries:SolderJumper" H 3550 6500 50  0001 C CNN
F 3 "" H 3550 6500 50  0000 C CNN
	1    3550 6600
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:R_PACK4 RP1
U 1 1 582335FC
P 3400 6300
F 0 "RP1" H 3400 6750 50  0000 C CNN
F 1 "1k" H 3400 6250 50  0000 C CNN
F 2 "kicad-libraries:0603X4" H 3400 6300 50  0001 C CNN
F 3 "" H 3400 6300 50  0000 C CNN
	1    3400 6300
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:WE-SL L1
U 1 1 58234A4C
P 9100 3250
F 0 "L1" H 8900 3460 40  0000 C CNN
F 1 "WE-SL" H 9100 3150 40  0000 C CNN
F 2 "kicad-libraries:WE-SL" H 9100 3250 60  0001 C CNN
F 3 "" H 9100 3250 60  0000 C CNN
	1    9100 3250
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:WE-SL L1
U 2 1 58234B45
P 9100 3750
F 0 "L1" H 8900 3960 40  0000 C CNN
F 1 "WE-SL" H 9100 3650 40  0000 C CNN
F 2 "kicad-libraries:WE-SL" H 9100 3750 60  0001 C CNN
F 3 "" H 9100 3750 60  0000 C CNN
	2    9100 3750
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:WE-SL L1
U 3 1 58234BEA
P 9100 1250
F 0 "L1" H 8900 1460 40  0000 C CNN
F 1 "WE-SL" H 9100 1150 40  0000 C CNN
F 2 "kicad-libraries:WE-SL" H 9100 1250 60  0001 C CNN
F 3 "" H 9100 1250 60  0000 C CNN
	3    9100 1250
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:WE-SL L1
U 4 1 58234C33
P 9100 1750
F 0 "L1" H 8900 1960 40  0000 C CNN
F 1 "WE-SL" H 9100 1650 40  0000 C CNN
F 2 "kicad-libraries:WE-SL" H 9100 1750 60  0001 C CNN
F 3 "" H 9100 1750 60  0000 C CNN
	4    9100 1750
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R2
U 1 1 582352E8
P 9500 2900
F 0 "R2" V 9580 2900 50  0000 C CNN
F 1 "120" V 9500 2900 50  0000 C CNN
F 2 "R0805E" H 9500 2900 60  0001 C CNN
F 3 "" H 9500 2900 60  0000 C CNN
	1    9500 2900
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:R R1
U 1 1 582354F1
P 9500 2100
F 0 "R1" V 9580 2100 50  0000 C CNN
F 1 "120" V 9500 2100 50  0000 C CNN
F 2 "R0805E" H 9500 2100 60  0001 C CNN
F 3 "" H 9500 2100 60  0000 C CNN
	1    9500 2100
	-1   0    0    1   
$EndComp
$Comp
L tinkerforge:VCC #PWR010
U 1 1 5824794E
P 2500 5700
F 0 "#PWR010" H 2500 5800 30  0001 C CNN
F 1 "VCC" H 2500 5800 30  0000 C CNN
F 2 "" H 2500 5700 60  0001 C CNN
F 3 "" H 2500 5700 60  0001 C CNN
	1    2500 5700
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:SP3491 U6
U 1 1 5824971E
P 7350 2750
F 0 "U6" H 7150 3300 60  0000 C CNN
F 1 "SP3491" H 7350 2200 60  0000 C CNN
F 2 "kicad-libraries:SOIC-14_3.9x8.7mm_Pitch1.27mm" H 8550 2650 60  0001 C CNN
F 3 "" H 8550 2650 60  0000 C CNN
	1    7350 2750
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:C C7
U 1 1 58249AB1
P 6700 3100
F 0 "C7" V 6550 3050 50  0000 L CNN
F 1 "100nF" V 6850 3000 50  0000 L CNN
F 2 "C0603E" H 6700 3100 60  0001 C CNN
F 3 "" H 6700 3100 60  0001 C CNN
	1    6700 3100
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:R R3
U 1 1 58249E96
P 8150 950
F 0 "R3" V 8230 950 50  0000 C CNN
F 1 "2.7k" V 8150 950 50  0000 C CNN
F 2 "R0603F" H 8150 950 60  0001 C CNN
F 3 "" H 8150 950 60  0000 C CNN
	1    8150 950 
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R4
U 1 1 5824A0C5
P 8150 1500
F 0 "R4" V 8230 1500 50  0000 C CNN
F 1 "2.2k" V 8150 1500 50  0000 C CNN
F 2 "R0603F" H 8150 1500 60  0001 C CNN
F 3 "" H 8150 1500 60  0000 C CNN
	1    8150 1500
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R5
U 1 1 5824A182
P 8150 2050
F 0 "R5" V 8230 2050 50  0000 C CNN
F 1 "2.7k" V 8150 2050 50  0000 C CNN
F 2 "R0603F" H 8150 2050 60  0001 C CNN
F 3 "" H 8150 2050 60  0000 C CNN
	1    8150 2050
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR011
U 1 1 5824AA12
P 6500 2850
F 0 "#PWR011" H 6500 2950 30  0001 C CNN
F 1 "VCC" H 6500 2950 30  0000 C CNN
F 2 "" H 6500 2850 60  0001 C CNN
F 3 "" H 6500 2850 60  0001 C CNN
	1    6500 2850
	0    -1   -1   0   
$EndComp
$Comp
L tinkerforge:GND #PWR012
U 1 1 5824B097
P 6950 3350
F 0 "#PWR012" H 6950 3350 30  0001 C CNN
F 1 "GND" H 6950 3280 30  0001 C CNN
F 2 "" H 6950 3350 60  0001 C CNN
F 3 "" H 6950 3350 60  0001 C CNN
	1    6950 3350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR013
U 1 1 5824B482
P 8150 2350
F 0 "#PWR013" H 8150 2350 30  0001 C CNN
F 1 "GND" H 8150 2280 30  0001 C CNN
F 2 "" H 8150 2350 60  0001 C CNN
F 3 "" H 8150 2350 60  0001 C CNN
	1    8150 2350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR014
U 1 1 5824B66D
P 8150 650
F 0 "#PWR014" H 8150 750 30  0001 C CNN
F 1 "VCC" H 8150 750 30  0000 C CNN
F 2 "" H 8150 650 60  0001 C CNN
F 3 "" H 8150 650 60  0001 C CNN
	1    8150 650 
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R6
U 1 1 5824C90F
P 8150 2950
F 0 "R6" V 8230 2950 50  0000 C CNN
F 1 "2.7k" V 8150 2950 50  0000 C CNN
F 2 "R0603F" H 8150 2950 60  0001 C CNN
F 3 "" H 8150 2950 60  0000 C CNN
	1    8150 2950
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R7
U 1 1 5824C915
P 8150 3500
F 0 "R7" V 8230 3500 50  0000 C CNN
F 1 "2.2k" V 8150 3500 50  0000 C CNN
F 2 "R0603F" H 8150 3500 60  0001 C CNN
F 3 "" H 8150 3500 60  0000 C CNN
	1    8150 3500
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:R R8
U 1 1 5824C91B
P 8150 4050
F 0 "R8" V 8230 4050 50  0000 C CNN
F 1 "2.7k" V 8150 4050 50  0000 C CNN
F 2 "R0603F" H 8150 4050 60  0001 C CNN
F 3 "" H 8150 4050 60  0000 C CNN
	1    8150 4050
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:GND #PWR015
U 1 1 5824C921
P 8150 4350
F 0 "#PWR015" H 8150 4350 30  0001 C CNN
F 1 "GND" H 8150 4280 30  0001 C CNN
F 2 "" H 8150 4350 60  0001 C CNN
F 3 "" H 8150 4350 60  0001 C CNN
	1    8150 4350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:VCC #PWR016
U 1 1 5824C927
P 8150 2650
F 0 "#PWR016" H 8150 2750 30  0001 C CNN
F 1 "VCC" H 8150 2750 30  0000 C CNN
F 2 "" H 8150 2650 60  0001 C CNN
F 3 "" H 8150 2650 60  0001 C CNN
	1    8150 2650
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:CONN_5 P3
U 1 1 54F74CD7
P 11050 2550
F 0 "P3" V 11000 2550 50  0000 C CNN
F 1 "CONN_5" V 11100 2550 50  0000 C CNN
F 2 "AKL_5_5" H 11050 2550 60  0001 C CNN
F 3 "" H 11050 2550 60  0000 C CNN
	1    11050 2550
	1    0    0    1   
$EndComp
NoConn ~ 7700 3000
NoConn ~ 7700 3100
NoConn ~ 7700 3200
$Comp
L tinkerforge:TVS D5
U 1 1 5824DDEB
P 8400 4050
F 0 "D5" H 8350 4150 40  0000 C CNN
F 1 "SMAJ24CA" H 8400 3950 40  0000 C CNN
F 2 "DO-214AC" H 8400 4050 60  0001 C CNN
F 3 "" H 8400 4050 60  0000 C CNN
	1    8400 4050
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:TVS D7
U 1 1 5824E27C
P 8650 4050
F 0 "D7" H 8600 4150 40  0000 C CNN
F 1 "SMAJ24CA" H 8650 3950 40  0000 C CNN
F 2 "DO-214AC" H 8650 4050 60  0001 C CNN
F 3 "" H 8650 4050 60  0000 C CNN
	1    8650 4050
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:TVS D4
U 1 1 5824E8CD
P 8400 2050
F 0 "D4" H 8350 2150 40  0000 C CNN
F 1 "SMAJ24CA" H 8400 1950 40  0000 C CNN
F 2 "DO-214AC" H 8400 2050 60  0001 C CNN
F 3 "" H 8400 2050 60  0000 C CNN
	1    8400 2050
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:TVS D6
U 1 1 5824E8D3
P 8650 2050
F 0 "D6" H 8600 2150 40  0000 C CNN
F 1 "SMAJ24CA" H 8650 1950 40  0000 C CNN
F 2 "DO-214AC" H 8650 2050 60  0001 C CNN
F 3 "" H 8650 2050 60  0000 C CNN
	1    8650 2050
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:R_PACK4 RP2
U 1 1 5825121A
P 6500 2700
F 0 "RP2" H 6500 3150 50  0000 C CNN
F 1 "10k" H 6500 2650 50  0000 C CNN
F 2 "kicad-libraries:0603X4" H 6500 2700 50  0001 C CNN
F 3 "" H 6500 2700 50  0000 C CNN
	1    6500 2700
	1    0    0    -1  
$EndComp
Text GLabel 6100 2350 0    60   Output ~ 0
485-R
Text GLabel 6100 2450 0    60   Input ~ 0
485-~REB
Text GLabel 6100 2550 0    60   Input ~ 0
485-DE
Text GLabel 6100 2650 0    60   Input ~ 0
485-D
$Comp
L tinkerforge:PTC PTC1
U 1 1 582538FF
P 10650 4050
F 0 "PTC1" V 10750 4100 50  0000 C CNN
F 1 "PTC 0.45A" V 10550 4050 50  0000 C CNN
F 2 "C1206" H 10650 4050 60  0001 C CNN
F 3 "" H 10650 4050 60  0000 C CNN
	1    10650 4050
	1    0    0    -1  
$EndComp
Text Notes 10350 1200 0    60   ~ 0
RX
Text Notes 10350 1700 0    60   ~ 0
~RX
Text Notes 10350 3200 0    60   ~ 0
TX
Text Notes 10350 3700 0    60   ~ 0
~TX
Text GLabel 9850 3200 1    60   Output ~ 0
TX
Text GLabel 9850 3700 1    60   Output ~ 0
~TX
Text GLabel 9850 1700 1    60   Output ~ 0
~RX
Text GLabel 9850 1200 1    60   Output ~ 0
RX
$Comp
L tinkerforge:SW_DIP_4 SW1
U 1 1 5825A285
P 9900 2500
F 0 "SW1" H 9850 2800 60  0000 C CNN
F 1 "SW_DIP_4" H 9900 2200 60  0000 C CNN
F 2 "SW_DIP_4_THT" H 10050 2800 60  0001 C CNN
F 3 "" H 10050 2800 60  0000 C CNN
	1    9900 2500
	-1   0    0    1   
$EndComp
Wire Wire Line
	1000 2550 1000 2450
Wire Wire Line
	2350 1800 1600 1800
Wire Wire Line
	1450 1150 1600 1150
Wire Wire Line
	1450 1900 1450 1150
Wire Wire Line
	2350 1600 2350 1800
Wire Wire Line
	1600 1150 1600 1200
Wire Wire Line
	2350 1200 2350 1150
Connection ~ 2350 1150
Wire Wire Line
	1600 1800 1600 1600
Connection ~ 1600 1800
Wire Wire Line
	4000 2250 3900 2250
Wire Wire Line
	3550 2200 3550 2250
Wire Wire Line
	4000 1650 3550 1650
Wire Wire Line
	3550 1600 3550 1650
Wire Wire Line
	3900 2200 3900 2250
Connection ~ 3900 2250
Wire Wire Line
	3550 1750 3900 1750
Wire Wire Line
	3900 1750 3900 1800
Connection ~ 3550 1750
Connection ~ 3900 1750
Wire Wire Line
	3900 2350 4000 2350
Wire Wire Line
	3550 3200 3900 3200
Wire Wire Line
	3900 3200 3900 3150
Wire Wire Line
	3550 2700 3900 2700
Wire Wire Line
	3900 2700 3900 2750
Wire Wire Line
	3550 3150 3550 3200
Connection ~ 3900 3200
Wire Wire Line
	4000 2600 3550 2600
Wire Wire Line
	3550 2550 3550 2600
Connection ~ 3550 2700
Connection ~ 3900 2700
Connection ~ 3550 3200
Connection ~ 3550 2250
Connection ~ 3550 2600
Connection ~ 3550 1650
Connection ~ 2350 1800
Wire Wire Line
	1500 2750 1500 2700
Wire Wire Line
	1350 2300 1500 2300
Wire Wire Line
	7000 3200 6950 3200
Wire Wire Line
	6950 3100 6950 3200
Wire Wire Line
	6900 3100 6950 3100
Connection ~ 6950 3100
Wire Wire Line
	6500 3100 6500 2850
Wire Wire Line
	6500 2850 7000 2850
Connection ~ 6500 2850
Connection ~ 6950 3200
Wire Wire Line
	8150 700  8150 650 
Wire Wire Line
	8150 1250 8150 1200
Wire Wire Line
	8150 1800 8150 1750
Wire Wire Line
	8150 2350 8150 2300
Wire Wire Line
	8150 2700 8150 2650
Wire Wire Line
	8150 3250 8150 3200
Wire Wire Line
	8150 3800 8150 3750
Wire Wire Line
	8150 4350 8150 4300
Wire Wire Line
	7700 2300 7750 2300
Wire Wire Line
	7750 2300 7750 1250
Wire Wire Line
	7750 1250 8150 1250
Wire Wire Line
	7700 2400 7850 2400
Wire Wire Line
	7850 2400 7850 1750
Wire Wire Line
	7850 1750 8150 1750
Wire Wire Line
	7700 2700 7850 2700
Wire Wire Line
	7850 2700 7850 3750
Wire Wire Line
	7850 3750 8150 3750
Wire Wire Line
	7700 2800 7950 2800
Wire Wire Line
	7950 2800 7950 3250
Wire Wire Line
	7950 3250 8150 3250
Connection ~ 8150 3750
Connection ~ 8150 3250
Connection ~ 8150 1750
Connection ~ 8150 1250
Wire Wire Line
	8400 2350 8400 2300
Wire Wire Line
	8650 2350 8650 2300
Wire Wire Line
	8400 4350 8400 4300
Wire Wire Line
	8650 4350 8650 4300
Wire Wire Line
	8400 3750 8400 3800
Wire Wire Line
	8650 3250 8650 3800
Wire Wire Line
	8400 1750 8400 1800
Wire Wire Line
	8650 1250 8650 1800
Connection ~ 8400 3750
Connection ~ 8650 3250
Connection ~ 8400 1750
Connection ~ 8650 1250
Wire Wire Line
	7000 2350 6700 2350
Wire Wire Line
	7000 2500 6900 2500
Wire Wire Line
	6900 2500 6900 2450
Wire Wire Line
	6900 2450 6700 2450
Wire Wire Line
	7000 2600 6850 2600
Wire Wire Line
	6850 2600 6850 2550
Wire Wire Line
	6850 2550 6700 2550
Wire Wire Line
	7000 2750 6800 2750
Wire Wire Line
	6800 2750 6800 2650
Wire Wire Line
	6800 2650 6700 2650
Wire Wire Line
	6100 2350 6300 2350
Wire Wire Line
	6300 2450 6100 2450
Wire Wire Line
	6100 2550 6300 2550
Wire Wire Line
	6300 2650 6100 2650
Wire Wire Line
	10650 2750 10650 3500
Wire Wire Line
	10650 4350 10650 4300
Wire Wire Line
	10450 3250 10450 2350
Wire Wire Line
	10450 2350 10650 2350
Wire Wire Line
	9450 3750 9850 3750
Wire Wire Line
	10500 3750 10500 2450
Wire Wire Line
	10500 2450 10650 2450
Wire Wire Line
	9450 1250 9850 1250
Wire Wire Line
	10550 1250 10550 2550
Wire Wire Line
	10550 2550 10650 2550
Wire Wire Line
	9450 1750 9500 1750
Wire Wire Line
	10600 1750 10600 2650
Wire Wire Line
	10600 2650 10650 2650
Wire Wire Line
	9450 3250 9500 3250
Wire Wire Line
	9850 1250 9850 1200
Connection ~ 9850 1250
Wire Wire Line
	9850 1750 9850 1700
Connection ~ 9850 1750
Wire Wire Line
	9850 3250 9850 3200
Connection ~ 9850 3250
Wire Wire Line
	9850 3750 9850 3700
Connection ~ 9850 3750
Wire Wire Line
	10100 2550 10100 2600
Wire Wire Line
	10100 2600 10150 2600
Wire Wire Line
	10150 2600 10150 3750
Connection ~ 10150 3750
Connection ~ 10100 2600
Wire Wire Line
	10100 2450 10100 2400
Wire Wire Line
	10100 2400 10150 2400
Wire Wire Line
	10150 2400 10150 1250
Connection ~ 10150 1250
Connection ~ 10100 2400
Wire Wire Line
	9700 2450 9600 2450
Wire Wire Line
	9600 2450 9600 3250
Connection ~ 9600 3250
Wire Wire Line
	9700 2550 9650 2550
Wire Wire Line
	9650 2550 9650 1750
Connection ~ 9650 1750
Wire Wire Line
	9700 2350 9500 2350
Wire Wire Line
	9500 1850 9500 1750
Connection ~ 9500 1750
Wire Wire Line
	9700 2650 9500 2650
Wire Wire Line
	9500 3150 9500 3250
Connection ~ 9500 3250
$Comp
L tinkerforge:Earth #PWR017
U 1 1 5826CA1E
P 10700 3500
F 0 "#PWR017" H 10700 3250 50  0001 C CNN
F 1 "Earth" H 10700 3350 50  0001 C CNN
F 2 "" H 10700 3500 50  0000 C CNN
F 3 "" H 10700 3500 50  0000 C CNN
	1    10700 3500
	0    -1   -1   0   
$EndComp
Wire Wire Line
	10650 3500 10700 3500
Connection ~ 10650 3500
$Comp
L tinkerforge:Earth #PWR018
U 1 1 5826CBE8
P 8650 4350
F 0 "#PWR018" H 8650 4100 50  0001 C CNN
F 1 "Earth" H 8650 4200 50  0001 C CNN
F 2 "" H 8650 4350 50  0000 C CNN
F 3 "" H 8650 4350 50  0000 C CNN
	1    8650 4350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:Earth #PWR019
U 1 1 5826CF0C
P 8400 4350
F 0 "#PWR019" H 8400 4100 50  0001 C CNN
F 1 "Earth" H 8400 4200 50  0001 C CNN
F 2 "" H 8400 4350 50  0000 C CNN
F 3 "" H 8400 4350 50  0000 C CNN
	1    8400 4350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:Earth #PWR020
U 1 1 5826CF92
P 8400 2350
F 0 "#PWR020" H 8400 2100 50  0001 C CNN
F 1 "Earth" H 8400 2200 50  0001 C CNN
F 2 "" H 8400 2350 50  0000 C CNN
F 3 "" H 8400 2350 50  0000 C CNN
	1    8400 2350
	1    0    0    -1  
$EndComp
$Comp
L tinkerforge:Earth #PWR021
U 1 1 5826D10E
P 8650 2350
F 0 "#PWR021" H 8650 2100 50  0001 C CNN
F 1 "Earth" H 8650 2200 50  0001 C CNN
F 2 "" H 8650 2350 50  0000 C CNN
F 3 "" H 8650 2350 50  0000 C CNN
	1    8650 2350
	1    0    0    -1  
$EndComp
Text Notes 5350 4000 0    39   ~ 0
SPI Slave\nP1.1 : USIC0_CH1-DX2E : SEL\nP1.2 : USIC0_CH1-DX0B : MOSI\nP1.3 : USIC0_CH1-DX1A : CLK\nP1.6 : USIC0_CH1-DOUT0 : MISO
Text Notes 5300 7250 0    39   ~ 0
ASC\nP2.12 : USIC1_CH1.DOUT0\nP2.13 : USIC1_CH1.DX0D
Text GLabel 5650 6850 0    60   Input ~ 0
485-R
Text GLabel 5650 6650 0    60   Output ~ 0
485-~REB
Text GLabel 5650 6550 0    60   Output ~ 0
485-DE
Text GLabel 5650 6750 0    60   Output ~ 0
485-D
$Comp
L tinkerforge:C C8
U 1 1 58274693
P 2550 4550
F 0 "C8" V 2400 4500 50  0000 L CNN
F 1 "10pF" V 2700 4500 50  0000 L CNN
F 2 "C0603F" H 2550 4550 60  0001 C CNN
F 3 "" H 2550 4550 60  0001 C CNN
	1    2550 4550
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:C C9
U 1 1 582748D4
P 2550 5150
F 0 "C9" V 2400 5100 50  0000 L CNN
F 1 "10pF" V 2700 5100 50  0000 L CNN
F 2 "C0603F" H 2550 5150 60  0001 C CNN
F 3 "" H 2550 5150 60  0001 C CNN
	1    2550 5150
	0    1    1    0   
$EndComp
$Comp
L tinkerforge:GND #PWR022
U 1 1 58275257
P 2350 5300
F 0 "#PWR022" H 2350 5300 30  0001 C CNN
F 1 "GND" H 2350 5230 30  0001 C CNN
F 2 "" H 2350 5300 60  0001 C CNN
F 3 "" H 2350 5300 60  0001 C CNN
	1    2350 5300
	1    0    0    -1  
$EndComp
Wire Wire Line
	3300 5150 3300 4900
Wire Wire Line
	3300 4800 4000 4800
Wire Wire Line
	3300 4550 3300 4800
$Comp
L tinkerforge:R R9
U 1 1 58275F18
P 3550 4900
F 0 "R9" V 3630 4900 50  0000 C CNN
F 1 "0" V 3550 4900 50  0000 C CNN
F 2 "R0402E" H 3550 4900 60  0001 C CNN
F 3 "" H 3550 4900 60  0000 C CNN
	1    3550 4900
	0    1    1    0   
$EndComp
Wire Wire Line
	4000 4900 3800 4900
$Comp
L tinkerforge:CRYSTAL_3225 X1
U 1 1 58276FBA
P 3100 4850
F 0 "X1" V 2850 4650 60  0000 C CNN
F 1 "16MHz" V 3550 4850 60  0000 C CNN
F 2 "kicad-libraries:CRYSTAL_3225" H 3100 4850 60  0001 C CNN
F 3 "" H 3100 4850 60  0000 C CNN
	1    3100 4850
	0    -1   -1   0   
$EndComp
Wire Wire Line
	2750 4550 3100 4550
Wire Wire Line
	2750 5150 3100 5150
Connection ~ 3100 5150
Connection ~ 3100 4550
Wire Wire Line
	2350 5300 2350 5150
$Comp
L tinkerforge:GND #PWR023
U 1 1 5827815F
P 2350 4700
F 0 "#PWR023" H 2350 4700 30  0001 C CNN
F 1 "GND" H 2350 4630 30  0001 C CNN
F 2 "" H 2350 4700 60  0001 C CNN
F 3 "" H 2350 4700 60  0001 C CNN
	1    2350 4700
	1    0    0    -1  
$EndComp
Wire Wire Line
	2350 4700 2350 4550
$Comp
L tinkerforge:GND #PWR024
U 1 1 58278535
P 3400 5100
F 0 "#PWR024" H 3400 5100 30  0001 C CNN
F 1 "GND" H 3400 5030 30  0001 C CNN
F 2 "" H 3400 5100 60  0001 C CNN
F 3 "" H 3400 5100 60  0001 C CNN
	1    3400 5100
	1    0    0    -1  
$EndComp
Wire Wire Line
	3400 5100 3400 5000
Wire Wire Line
	5650 6750 5800 6750
Wire Wire Line
	5800 6850 5650 6850
Wire Wire Line
	5450 4550 5700 4550
Wire Wire Line
	5450 4650 5600 4650
Wire Wire Line
	5450 4750 5500 4750
Wire Wire Line
	5450 5050 5800 5050
Wire Wire Line
	5650 6550 5800 6550
Wire Wire Line
	5800 6650 5650 6650
Wire Wire Line
	3050 6400 3050 6250
Wire Wire Line
	3050 6250 3200 6250
Wire Wire Line
	3900 6350 4050 6350
Wire Wire Line
	3600 6250 3800 6250
Wire Wire Line
	3800 6250 3800 6450
Wire Wire Line
	3800 6450 4050 6450
Wire Wire Line
	2500 5700 2500 5800
Wire Wire Line
	2500 5800 2650 5800
Wire Wire Line
	2500 6100 2650 6100
Connection ~ 2500 5800
Wire Wire Line
	2500 6400 2650 6400
Connection ~ 2500 6100
Wire Wire Line
	4000 5300 3900 5300
Wire Wire Line
	3900 5300 3900 6050
Wire Wire Line
	3200 6150 3150 6150
Wire Wire Line
	3150 6150 3150 6100
Wire Wire Line
	3150 6100 3050 6100
Wire Wire Line
	3600 6150 3900 6150
Wire Wire Line
	3900 6150 3900 6350
$Comp
L tinkerforge:GND #PWR025
U 1 1 5828358D
P 3850 6650
F 0 "#PWR025" H 3850 6650 30  0001 C CNN
F 1 "GND" H 3850 6580 30  0001 C CNN
F 2 "" H 3850 6650 60  0001 C CNN
F 3 "" H 3850 6650 60  0001 C CNN
	1    3850 6650
	0    -1   -1   0   
$EndComp
Wire Wire Line
	3750 6650 3850 6650
Wire Wire Line
	4050 6550 3750 6550
Wire Wire Line
	3200 6050 3200 6000
Wire Wire Line
	3200 6000 3100 6000
Wire Wire Line
	3100 6000 3100 5800
Wire Wire Line
	3100 5800 3050 5800
Wire Wire Line
	3900 6050 3600 6050
NoConn ~ 4050 6650
NoConn ~ 5800 6450
NoConn ~ 5800 6350
NoConn ~ 5800 6250
NoConn ~ 5800 6150
NoConn ~ 5800 6050
NoConn ~ 5800 5950
NoConn ~ 5800 5850
NoConn ~ 5800 5750
NoConn ~ 5800 5650
NoConn ~ 5800 5550
NoConn ~ 5800 4950
NoConn ~ 5800 4850
NoConn ~ 5800 4450
NoConn ~ 4000 3800
NoConn ~ 4000 3900
NoConn ~ 4000 4000
NoConn ~ 4000 4100
NoConn ~ 4000 4200
NoConn ~ 4000 4300
NoConn ~ 4000 4400
NoConn ~ 4000 4500
NoConn ~ 4000 4600
NoConn ~ 4000 4700
NoConn ~ 4000 5000
NoConn ~ 4000 5100
NoConn ~ 4000 5200
NoConn ~ 4050 5950
NoConn ~ 3600 5950
NoConn ~ 3200 5950
Text Notes 10900 4350 1    39   ~ 0
OZCJ0016FF2E
Text Notes 2750 4200 0    39   ~ 0
TSX-3225 16.0000MF18X-AC0\nno C - 1,00017\n3pF - 1,00008\n4.7pF - 1,00005\n9pF - 1,00001\n10pF - 1,00000MHz
Wire Wire Line
	1350 1900 1450 1900
NoConn ~ 1350 1700
$Comp
L tinkerforge:R_PACK4 RP3
U 1 1 587734FA
P 5350 5350
F 0 "RP3" H 5350 5800 50  0000 C CNN
F 1 "10k" H 5350 5300 50  0000 C CNN
F 2 "kicad-libraries:0603X4" H 5350 5350 50  0001 C CNN
F 3 "" H 5350 5350 50  0000 C CNN
	1    5350 5350
	0    1    1    0   
$EndComp
NoConn ~ 5400 5150
$Comp
L tinkerforge:GND #PWR026
U 1 1 58773E1A
P 5550 5650
F 0 "#PWR026" H 5550 5650 30  0001 C CNN
F 1 "GND" H 5550 5580 30  0001 C CNN
F 2 "" H 5550 5650 60  0001 C CNN
F 3 "" H 5550 5650 60  0001 C CNN
	1    5550 5650
	1    0    0    -1  
$EndComp
Wire Wire Line
	5400 5550 5500 5550
Wire Wire Line
	5550 5550 5550 5650
Connection ~ 5550 5550
Connection ~ 5500 5550
Connection ~ 5600 5550
Wire Wire Line
	5700 5150 5700 4550
Connection ~ 5700 4550
Wire Wire Line
	5600 5150 5600 4650
Connection ~ 5600 4650
Wire Wire Line
	5500 5150 5500 4750
Connection ~ 5500 4750
Wire Wire Line
	2350 1150 2550 1150
Wire Wire Line
	1600 1800 1350 1800
Wire Wire Line
	3900 2250 3550 2250
Wire Wire Line
	3900 2250 3900 2350
Wire Wire Line
	3550 1750 3550 1800
Wire Wire Line
	3900 1750 4000 1750
Wire Wire Line
	3900 3200 4000 3200
Wire Wire Line
	3550 2700 3550 2750
Wire Wire Line
	3900 2700 4000 2700
Wire Wire Line
	3550 3200 3550 3250
Wire Wire Line
	3550 2250 3550 2300
Wire Wire Line
	3550 2600 3550 2700
Wire Wire Line
	3550 1650 3550 1750
Wire Wire Line
	2350 1800 2350 1850
Wire Wire Line
	6950 3100 7000 3100
Wire Wire Line
	6950 3200 6950 3350
Wire Wire Line
	8150 3750 8400 3750
Wire Wire Line
	8150 3250 8650 3250
Wire Wire Line
	8150 1750 8400 1750
Wire Wire Line
	8150 1250 8650 1250
Wire Wire Line
	8400 3750 8750 3750
Wire Wire Line
	8650 3250 8750 3250
Wire Wire Line
	8400 1750 8750 1750
Wire Wire Line
	8650 1250 8750 1250
Wire Wire Line
	9850 1250 10150 1250
Wire Wire Line
	9850 1750 10600 1750
Wire Wire Line
	9850 3250 10450 3250
Wire Wire Line
	9850 3750 10150 3750
Wire Wire Line
	10150 3750 10500 3750
Wire Wire Line
	10100 2600 10100 2650
Wire Wire Line
	10150 1250 10550 1250
Wire Wire Line
	10100 2400 10100 2350
Wire Wire Line
	9600 3250 9850 3250
Wire Wire Line
	9650 1750 9850 1750
Wire Wire Line
	9500 1750 9650 1750
Wire Wire Line
	9500 3250 9600 3250
Wire Wire Line
	10650 3500 10650 3800
Wire Wire Line
	3100 5150 3300 5150
Wire Wire Line
	3100 4550 3300 4550
Wire Wire Line
	2500 5800 2500 6100
Wire Wire Line
	2500 6100 2500 6400
Wire Wire Line
	5550 5550 5600 5550
Wire Wire Line
	5500 5550 5550 5550
Wire Wire Line
	5600 5550 5700 5550
Wire Wire Line
	5700 4550 5800 4550
Wire Wire Line
	5600 4650 5800 4650
Wire Wire Line
	5500 4750 5800 4750
$Comp
L tinkerforge:R_PACK4 RP101
U 1 1 6060195C
P 1750 2350
F 0 "RP101" H 1750 2915 50  0000 C CNN
F 1 "82" H 1750 2824 50  0000 C CNN
F 2 "kicad-libraries:4X0402" H 1750 2350 50  0001 C CNN
F 3 "" H 1750 2350 50  0000 C CNN
	1    1750 2350
	-1   0    0    -1  
$EndComp
$Comp
L tinkerforge:C C101
U 1 1 60603D2C
P 1500 2500
F 0 "C101" V 1550 2250 50  0000 L CNN
F 1 "220pF" V 1350 2400 50  0000 L CNN
F 2 "kicad-libraries:C0402F" H 1500 2500 60  0001 C CNN
F 3 "" H 1500 2500 60  0001 C CNN
	1    1500 2500
	1    0    0    -1  
$EndComp
Wire Wire Line
	1550 2300 1500 2300
Connection ~ 1500 2300
Wire Wire Line
	1350 2200 1550 2200
Wire Wire Line
	1350 2100 1550 2100
Wire Wire Line
	1350 2000 1550 2000
Wire Wire Line
	1950 2000 2150 2000
Wire Wire Line
	1950 2100 2150 2100
Wire Wire Line
	1950 2200 2150 2200
Wire Wire Line
	1950 2300 2150 2300
Wire Wire Line
	1650 1150 1600 1150
Connection ~ 1600 1150
Wire Wire Line
	2250 1150 2350 1150
$EndSCHEMATC
