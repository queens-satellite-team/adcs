# csdc-6 - All development for the 2023 CSDC-6 competition. 
This software is designed to run on ADCS hardware as described in the [csdc-4100-adcs-design](https://queensuca.sharepoint.com/:f:/r/teams/GROUP-QSET/Shared%20Documents/Satellite-General/csdc/4000-systems-design/csdc-4100-adcs-design?csf=1&web=1&e=ExKmLO) documents.

## adcs-simulation
Holds the 2022-2023 version of the satellite physics simulation. There is one MATLAB Simulink version and one Python version, it will be decided later which will move forward.

## adcs-control-code
Holds the high-level, hardware agnostic control code. This contains all control algorithms for the ADCS for each mode: pointing mode, fault modes, etc.

## adcs-stm32-driver
Holds the low-level STM32 driver designed for the _______ chip used for the CSDC 6 ADCS design.
