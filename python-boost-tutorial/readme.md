> This tutorial was originally created by [Andrew Hayman](https://github.com/hayman314). The original code can be found [here](https://github.com/hayman314/python-boost-demo).

# Description
This code base demonstrates a hello world example of using Boost's C++ Python wrapper using conan and cmake. 

## Requirements
- [Python](https://www.python.org/downloads/), version at least 3.9 (make sure to add to path during install)
- [CMake](https://cmake.org/download/), latest version (again make sure this is added to path)

## How to run
First, install conan
```
pip install -r requirements.txt
```
You will likely need to restart your terminal/IDE so conan is accessible. 

Run the build.sh script that runs the package installation and builds the shared library for Python with 
```
./build.sh
```

Change permissions if necessary with

```
chmod 777 build.sh
```
Once run, the .dll (.pyd) will be in /python directory where you can test the demo Python program with 
```
python python/demo.py
```
which should result in 'hello world'

## Walkthrough
### Program Structure
- src -> cpp source files built into static libraries
- inc -> cpp header files for the src files
- python_wrapper -> cpp source files for converting the src libraries to Python
- python -> demo Python program to show importing & using the resulting library

### conanfile.py
This is where we define our requirements for the project, including Boost. This makes Boost available to cmake through find_package and automatically deals with the paths. 

### CMakeLists.txt
These files define the build for the program. This includes library linking, include directories, etc.

### CMake Build Process
1. [build script / Conan] Install Boost using Conan. The `--build=missing` flag takes care of building Boost with the right flags that allow it to use Boost.Python
2. [build script / Conan] Tell CMake to build
3. [CMake] Create a static library for your code in /src and /inc, which is completely independent of the Python wrapper. This is useful because it can be tested independently (i.e. with gtest)
4. [CMake] Create a shared library with the code from /python_wrapper which pulls in the static library and relevant headers. 
5. [CMake] Convert the '.dll' to '.pyd' which is the format Python expects
6. [build script] Copy the .pyd to the Python directory where you want to import it

### Additional notes
- This does not demonstrate best practices for Conan/CMake, but rather the smallest possible example to get Boost.Python working. 
- Name the shared library the same as the Boost Python module.
- This example can easily be used as a boilerplate to include additional dependencies like GTest for testing C++ code. 
