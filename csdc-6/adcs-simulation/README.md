# adcs-simulation
Cubesat attitude simulations. These simulations work for any generic satellite design. The current features are described for each.

## MATLAB
The MATLAB simulation uses Simulink to provide a rotational attitude simulation. A PID controller is used to apply torques to the satellite using reaction wheels. This model was primarily used in development, and is no longer a working model.

## C++
The C++ simulation is a console-based program that can plot all rotational satellite properties. The PID controller used for control is located in [csdc-6/adcs-control-code](https://github.com/queens-satellite-team/adcs/tree/staging/csdc-6/adcs-control-code), and was tuned using the MATLAB model. The C++ model is the current working model. The control code will be used on the satellite itself. This code was developped for CSDC-6 but should be ubiquitous accross any satellite design.