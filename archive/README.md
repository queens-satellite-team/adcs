# ADCS - Archive of all repos as of 2022-10-01
*NOTE:* please refer to the general `queens-satellite-team documentation` repo for datasheets and part specifications

## adcs-simulink-simulation
Holds the most recent version of the Simulink (2019) simulation of satellite attitude based on reaction wheel control.
- Future development could include the implementation of magnetorquer simulation, simulating various low-amplitude ambient torques (ie. gravity imbalances, solar sail effect on the side of the sat, etc..), automatic translation of control code to C, upgrading control code to include integral and derivative coefficients, incoporating reaction wheel and magnetorquer limitations (wheel saturation or simulating changes in Earth's field with orbit).
- This version is missing work from the 2021-2022 school year.
- *NOTE:* requires the `Simulink 3D Animation Toolbox`

## attitude-control-simulator
Holds the original attitude simulation model, done purely using MATLAB and discontinued - kept on the repo to reference in the development of the `adcs-simulink-simulation`

## balloon-dev
Holds the varied iterations on the development of Arduino code used to save BNO055, MPU6050, and GPS attitude data to an SD card for the CAN-SBX project.
- The most recent update `master-no-rtos.ino` has been copied to the `CAN-SBX` repo for future development.
- This directory and it's subtrees should either be deleted or updated to be kept simply for reference after the completion of the CAN_SBX project.

## gimbal
Holds STL files for the multiple basic iterations done to create gimbal test beds for a physical 'single U' prototype.
- Future development should focus on a stable low-friction gimbal housing a prototype ADCS unit with as little exterior torque (ie. from gravity or electrical connections) as possible.
- Research has previously been done considering air-bearings.

## reaction-wheel-cntrol
Holds some basic control code for an Arduino microcontroller on a single-axis, single U test bed.

## sin-sensors
Holds some code used to calibrate sun-sensors on a one-axis gimbal