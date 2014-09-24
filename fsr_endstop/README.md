FSR_endstop
=======

This is a small controller board for FSR bed leveling sensors for delta 3D printers such as Kossel. More detailed documentation at http://www.unreasonmachines.com/msp430-fsr-endstop-controller-for-3d-printers/.

This specific revision of the PCB hasn't been tested yet (PCBs have been ordered already) - but an earlier version with identical schematics but different layout is running just fine in my Kossel right now. Still, use at your own risk.

## Bill of materials

Name     | Part #                                | Qty |
---------|---------------------------------------|-----|
C1       | 100n 0603 capacitor                   | 1   | 
J1       | 3-pin JST PH right angle connector    | 1   | 
J2,J3,J4 | 2-pin JST PH right angle connector    | 3   |
J5       | 6-pin 1.27mm right angle connector    | 1   |
R1,R2,R3 | 10K 0603 resistor                     | 3   |
R4       | 1.5K 0603 resistor                    | 1   |
R5       | 47K 0603 resistor                     | 1   |
TOUCH    | 0603 2mA LED                          | 1   |
U1       | MSP430G2553 MCU, 20-pin SSOP          | 1   |

Tested with Interlink 30-81794 FSRs, but any similar sized FSR should work, possibly with minor changes to the filter coefficients in code.