cmake_minimum_required(VERSION 3.8)
project(simulator)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

find_package(yaml-cpp REQUIRED)
# uncomment the following section in order to fill in
# further dependencies manually.
# find_package(<dependency> REQUIRED)

add_executable(simulator
    src/main.cpp
    src/Simulator.cpp
    src/SensorActuatorFactory.cpp
    src/ConfigurationSingleton.cpp
    src/UI.cpp
    interface/src/Actuator.cpp
    interface/src/ADCS_device.cpp
    interface/src/ADCS_timer.cpp
    interface/src/Reaction_wheel.cpp
    interface/src/Sensor.cpp
    ../../adcs-control-code/src/PointingModeController.cpp
  )
#ament_target_dependencies(simulator rclcpp std_msgs yaml-cpp)
target_link_libraries(${PROJECT_NAME} ${YAML_CPP_LIBRARIES})
include_directories(
    "${CMAKE_SOURCE_DIR}/inc",
    "${CMAKE_SOURCE_DIR}/interface/inc",
    "${CMAKE_SOURCE_DIR}/../../adcs-control-code/inc",
    "${CMAKE_SOURCE_DIR}/../../adcs-control-code/interface/inc",
    "/mnt/g/Program Files/eigen-3.4.0")
target_compile_features(simulator PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
set(EXECUTABLE_OUTPUT_PATH "/build")
