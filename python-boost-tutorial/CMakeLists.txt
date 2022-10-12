# Define cmake version
cmake_minimum_required(VERSION 3.16)

# Set C++ standard
set(CMAKE_CXX_STANARD 14)

# Define a project name
project (python_boost_demo)

# Go make the static library using the CMake file in /src
add_subdirectory(src)

# Now that we have built our static library, go build the shared Python wrapper library using the Cmake file in /python_wrapper
add_subdirectory(python_wrapper)