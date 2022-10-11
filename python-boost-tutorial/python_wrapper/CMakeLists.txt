# Make the produced libraries go into /build instead of /build/Release
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY_RELEASE ${CMAKE_BINARY_DIR})

# Find Boost and auto-generate the cmake files
find_package(Boost MODULE REQUIRED)

# Find Python, which is required by Boost
find_package(Python 3 REQUIRED COMPONENTS Interpreter Development)

# Create the shared library from the python wrapper code
add_library(python_demo MODULE python_wrapper.cpp)

# Include the relevant header files
target_include_directories(python_demo PRIVATE
    "${CMAKE_SOURCE_DIR}/inc"
    ${Python_INCLUDE_DIRS}
    ${Boost_INCLUDE_DIRS}
)

# Link to required libraries
target_link_libraries(python_demo PRIVATE 
    project_lib
    ${Python_LIBRARIES}
    Boost::Boost 
)

# Convert the final .dll to .pyd (needed for Windows)
add_custom_command(TARGET python_demo
    POST_BUILD
    COMMAND ${CMAKE_COMMAND} -E copy $<TARGET_FILE:python_demo> "${CMAKE_BINARY_DIR}/python_demo.pyd"
    COMMENT "Converting dll to pyd" VERBATIM
)