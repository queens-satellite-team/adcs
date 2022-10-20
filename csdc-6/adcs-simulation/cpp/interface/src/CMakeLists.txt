# Create a static library
add_library(project_lib interface.cpp)

# Include relevant header directories
target_include_directories(project_lib PRIVATE "../inc")
target_include_directories(project_lib PRIVATE "../../../../adcs-control-code/interface/inc")