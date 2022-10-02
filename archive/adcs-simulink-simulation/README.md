# ADCS-Simulink-Simulation
Simulation of satellite attitude based on reaction wheel model.

Current Model
Animation Block
	Animation of cubeSat within cartesian coordinate system
Attitude Simulation Block
	Angular position and velocity values integrated based on angular acceleration given by
	angAccel = -invInertia * angVel x qsatInertia * angVel - invInertia * angVel x rwheelMomentum - invInertia * rwheelTorque - invInertia * cntrlTorque;
	Magnetorquer torque simply added to reaction wheel torque (in satellite frame of reference)
Control Block
	Simple proportional code with arbitrary k values for reaction wheels

Improvements to be Made
Animation Block
	Camera view following satellite in orbital pattern (may need multiple coordinate axies)
Attitude Simulation Block
	Noise and various torque effects (gravity, solar, magnetic, etc..)
Control Block
	Translate to C code
	Add integral and derivative portion of code
	Calculate real k values
	incorporate torque and reaction wheel velocity limits
