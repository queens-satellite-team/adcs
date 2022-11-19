import pandas as pd
import matplotlib.pyplot as plt


data = pd.read_csv("output/sim_out.csv")
time = data['Time']

theta_x = data['Satellite theta x']
theta_y = data['Satellite theta y']
theta_z = data['Satellite theta z']

plt.figure(1)
plt.title("Satellite Position vs. Time")
plt.xlabel("Time [s]")
plt.ylabel("Satellite Position [rad]")
plt.plot(time, theta_x, label="x")
plt.plot(time, theta_y, label="y")
plt.plot(time, theta_z, label="z")
plt.legend()
#plt.show()
plt.savefig('plots/Satellite_Position_vs_Time.png')

omega_x = data['Satellite Omega x']
omega_y = data['Satellite Omega y']
omega_z = data['Satellite Omega z']

plt.figure(2)
plt.title("Satellite Velocity vs. Time")
plt.xlabel("Time [s]")
plt.ylabel("Satellite Velocity [rad/s]")
plt.plot(time, omega_x, label="x")
plt.plot(time, omega_y, label="y")
plt.plot(time, omega_z, label="z")
plt.legend()
#plt.show()
plt.savefig('plots/Satellite_Velocity_vs_Time.png')

alpha_x = data['Satellite alpha x']
alpha_y = data['Satellite alpha y']
alpha_z = data['Satellite alpha z']

plt.figure(3)
plt.title("Satellite Acceleration vs. Time")
plt.xlabel("Time [s]")
plt.ylabel("Satellite Acceleration [rad/s^2]")
plt.plot(time, alpha_x, label="x")
plt.plot(time, alpha_y, label="y")
plt.plot(time, alpha_z, label="z")
plt.legend()
#plt.show()
plt.savefig('plots/Satellite_Acceleration_vs_Time.png')

accel_x = data['Accelerometer x']
accel_y = data['Accelerometer y']
accel_z = data['Accelerometer z']

plt.figure(4)
plt.title("Accelerometer Reading vs. Time")
plt.xlabel("Time [s]")
plt.ylabel("Accelerometer [m/s^2]")
plt.plot(time, accel_x, label="x")
plt.plot(time, accel_y, label="y")
plt.plot(time, accel_z, label="z")
plt.legend()
#plt.show()
plt.savefig('plots/Accelerometer_Reading_vs_Time.png')

wheel_count = len([col for col in data.columns if "Reaction wheel" in col]) // 2
for i in range(wheel_count):
    plt.figure(5+i)
    rw_omega = data['Reaction wheel %d Omega' % i]
    rw_alpha = data['Reaction wheel %d alpha' % i]

    plt.title("Reaction wheel %d vs. Time" % (i+1))
    plt.xlabel("Time [s]")
    plt.plot(time, rw_omega, label="omega")
    plt.plot(time, rw_alpha, label="alpha")
    plt.legend()
    #plt.show()
    print(i)
    plt.savefig('plots/Reaction wheel %d alpha.png' %(i+1))

