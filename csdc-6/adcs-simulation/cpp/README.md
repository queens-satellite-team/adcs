# C++ ADCS Simulation
The ADCS simulation supports the following features:

- Reaction Wheel Controls
    - Any combination of reaction wheel combination is theoretically possible, but for now they must all point towards a common point
- Sensor attitude knowledge
    - Gyroscopes:
        - Currently used as a means to get the satellite position directly
        - Next steps are to model the gyroscope to work the same way as the actual hardware
- Unit testing
    - Several pre-defined tests are available to validate any changes to the model. Useage is described in the "Usage" section
    - 6 analytical examples show "PASS" or "FAIL" as the results are compared to analytically solved examples
    - 3 controller-based examples must be inspected manually to confirm if they are working as expected
- Performance testing
    - Three performance tests are used to validate code efficiency. Any major changes to code should run the performance tests before and after to ensure the changes are not inhibiting to useage

## Requirements
The following tools and software are necessary to build and run the simulation:

|Tool|Description|Download|Necessity|
|-|-|-|-|
|WSL|The Windows Subststem for Linux is a commandline tool that allows linux to be run within windows. This is necessary for building some of the tools used by the simulator.|https://learn.microsoft.com/en-us/windows/wsl/install|Required|
|VSCode|VSCode is a text editor that has functionality very close to an IDE. It is the recommended text edit for the simulator.|https://code.visualstudio.com/download|Recommeded|

Of course if you're using native linux you don't need to do anything special here.

### Linux packages
After installing WSL, make sure to install the following packages as well. You can run the command in a WSL terminal (or in any terminal if you're using native Linux):
|Package|Description|Command|
|-|-|-|
|cmake|cmake is a tool that allows simple compilation of binaries for c++ projects.|`sudo apt install cmake`|
|yaml|YAML is a markup language commonly used for programming configuration files. It is used for our config files, one for the setup and one for the exit conditions.|`sudo apt install libyaml-cpp-dev`|
|eigen|eigen is a C++ package used for fast matrix-based math. It is used for both the simulation and the control code.|`sudo apt install libeigen3-dev`|
|g++ compiler|the g++ compiler is what we use to compile the simulator.|`sudo apt install build-essential`|
|Python3|Python is a scripting language. We use it to plot our results.|`sudo apt install python3-dev`|
|pip|pip is a tool used to install python packages.|`sudo apt install python3-pip`|

### Python libraries
Some Python libraries are required for the plotting functionality:
|Package|Description|Command|
|-|-|-|
|pandas|pandas is a Python library used for data manipulation. It is used to read in the csv file for plotting|`pip3 install pandas`|
|matplotlib|matplotlib is a plotting library for Python.|`pip3 install matplotlib`|

## Compilation
After cloning the repo, Follow these steps to compile the code:
1. Open the "cpp" folder in vscode
2. Open a WSL terminal:  
   ![Click the little arrow beside the + sign on the far right of the terminal and choose WSL to open the terminal.](https://github.com/queens-satellite-team/adcs/blob/assets/assets/cpp_readme_pictures/open_wsl_terminal.png)

3. Navigate to `/adcs-simulator/cpp`
4. Run `./cmake.sh` (*This is a shell script that will clean and build all the cmake files. You can also run `./cmake.sh clean` to cleanup any unwanted cmake files*)
5. Run `make` to build the binary. It will be located in `./bin` and is called `simulator`

## Usage
After building the binary, you can start the simualtor by running
```
./bin/simulator
```
From there, you can run any of the following commands. For more assitance, you can run `help <command>` with any of the commands below:

- `start_sim <config_yaml> <exit_yaml>`  
  Starts a simulation with the provided files as configuration files. It will run until either a) the  provided timeout is reached, or b) the controller has reached the desired pointing state. If the exit yaml  isn't provided it will just run with the initial coniditions until the timout is reached. See the unit  tests for example yaml files.

- `resume_sim <exit_yaml>`  
  Currently not implemented.

- `unit_test`  
    Runs a predefined set of tests to ensure the simulation is working properly. The first 6 are scenarios that have been calculated analytically. The results are compared against the exptected results and a pass/fail is assigned. The last three tests use the controller, in the following three scenarios:
    1. The satellite is given an initial state of rest, and is asked to stay in that state for 600 seconds
    2. The satellite is given an initial velocity, and is asked to return to it's original state.
    3. The satellite is at rest, and is requested to change attidue by around 30 degrees.  

   The output directory and plotting directories are cleared before running the tests, so make sure to save any results you want before running this test.

- `perf_test`  
  Runs a predefined set of tests in order to benchmark the efficiency of the simulator. Three tests are run ten times each and their run time is then averaged and displayed to the user. This should be used to determine how changes to the simulator effect efficiency. The tests run are the same as the controller-based unit tests with some minor differences.  

  The output directory and plotting directories are cleared before running the tests, so make sure to save any results you want before running this test.

- `exit`  
  Quits the application.

- `clean_out`  
  Deletes all output csv files in the `output` folder.

- `clean_plots`  
  Deletes all plot files in the `plots` folder.

## Future Work
The following items are to be implemented in the future:

### Physics
- [ ] Orbital modelling
- [ ] Magnetic field modelling
- [ ] Sun position and intensity modelling
- [ ] Make the sensors more accurate to the actual hardware, and implement missing sensors and actuators
- [ ] Add noise to all sensor measurements and actuator outputs
  - [ ] Filter the noise on sensors using a Kalman sensor

### UI
- [ ] Allow continuation of the previous simulation run whever it exited
- [ ] Implement a GUI
  - [ ] Add a 3D model of the satellite (an .stl file or similar should be provided) that shows the simulation as it progresses

### Other functionality
- [ ] Explicitly dissalow all invalid timestamp constructors
- [ ] Change the reaction wheels to pass their ID instead of their position to the simulator
- [ ] Add functionality to run each unit test individually as a command line option
