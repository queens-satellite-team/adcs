!/bin/bash

# Clean and create the build directory
rm -rvf build
mkdir build
cd build

# Install the Conan dependencies (i.e. Boost)
conan install .. --build=missing

# Build the library
conan build ..

# Copy the .pyd file to the python directory for the demo
cd ..
cp build/*.pyd src/