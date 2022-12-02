#!/usr/bin/env python3

import sys
import os
import re
import pandas as pd
import matplotlib.pyplot as plt
import webbrowser

def plot_results(csv_name, outpath):
    data = pd.read_csv(csv_name)
    time = data['Time']

    timestep = data['Timestep']*1000

    plt.figure(1)
    plt.title("Timestep vs. Time")
    plt.xlabel("Time [s]")
    plt.ylabel("Timestep [ms]")
    plt.plot(time, timestep)
    plt.savefig(outpath + '/Timestep_vs_Time.png')

    theta_x = data['Satellite theta x']
    theta_y = data['Satellite theta y']
    theta_z = data['Satellite theta z']

    plt.figure(2)
    plt.title("Satellite Position vs. Time")
    plt.xlabel("Time [s]")
    plt.ylabel("Satellite Position [rad]")
    plt.plot(time, theta_x, label="x")
    plt.plot(time, theta_y, label="y")
    plt.plot(time, theta_z, label="z")
    plt.legend()
    plt.savefig(outpath + '/Satellite_Position_vs_Time.png')
    webbrowser.open(outpath + '/Satellite_Position_vs_Time.png')

    

    omega_x = data['Satellite Omega x']
    omega_y = data['Satellite Omega y']
    omega_z = data['Satellite Omega z']

    plt.figure(3)
    plt.title("Satellite Velocity vs. Time")
    plt.xlabel("Time [s]")
    plt.ylabel("Satellite Velocity [rad/s]")
    plt.plot(time, omega_x, label="x")
    plt.plot(time, omega_y, label="y")
    plt.plot(time, omega_z, label="z")
    plt.legend()
    plt.savefig(outpath + '/Satellite_Velocity_vs_Time.png')

    alpha_x = data['Satellite alpha x']
    alpha_y = data['Satellite alpha y']
    alpha_z = data['Satellite alpha z']

    plt.figure(4)
    plt.title("Satellite Acceleration vs. Time")
    plt.xlabel("Time [s]")
    plt.ylabel("Satellite Acceleration [rad/s^2]")
    plt.plot(time, alpha_x, label="x")
    plt.plot(time, alpha_y, label="y")
    plt.plot(time, alpha_z, label="z")
    plt.legend()
    plt.savefig(outpath + '/Satellite_Acceleration_vs_Time.png')

    accel_x = data['Accelerometer x']
    accel_y = data['Accelerometer y']
    accel_z = data['Accelerometer z']

    plt.figure(5)
    plt.title("Accelerometer Reading vs. Time")
    plt.xlabel("Time [s]")
    plt.ylabel("Accelerometer [m/s^2]")
    plt.plot(time, accel_x, label="x")
    plt.plot(time, accel_y, label="y")
    plt.plot(time, accel_z, label="z")
    plt.legend()
    plt.savefig(outpath + '/Accelerometer_Reading_vs_Time.png')

    wheel_count = len([col for col in data.columns if "Reaction wheel" in col]) // 2
    for i in range(wheel_count):
        plt.figure(6+i)
        rw_omega = data['Reaction wheel %d Omega' % i]
        rw_alpha = data['Reaction wheel %d alpha' % i]

        plt.title("Reaction wheel %d vs. Time" % (i+1))
        plt.xlabel("Time [s]")
        plt.plot(time, rw_omega, label="omega")
        plt.plot(time, rw_alpha, label="alpha")
        plt.legend()
        plt.savefig(outpath + '/Reaction wheel %d alpha.png' %(i+1))


def main():
    filepath = sys.argv[1]
    if (not os.path.exists('plots')):
        os.mkdir('plots')
    filename = re.search('output/(.*).csv', filepath)
    outpath = 'plots/' + filename.group(1)
    if (not os.path.exists(outpath)):
        os.mkdir(outpath)

    plot_results(filepath, outpath)

if __name__ == "__main__":
    main()