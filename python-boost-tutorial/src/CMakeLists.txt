# Create a static library
add_library(project_lib demo.cpp)

# Include relevant header directories
target_include_directories(project_lib PRIVATE "../inc")