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
    cpp/src/main.cpp
    cpp/src/Simulator.cpp
    cpp/src/SensorActuatorFactory.cpp
    cpp/src/ConfigurationSingleton.cpp
    cpp/src/Accelerometer.cpp
    cpp/src/Actuator.cpp
    cpp/src/Reaction_wheel.cpp
    cpp/src/Sensor.cpp
  )
#ament_target_dependencies(simulator rclcpp std_msgs yaml-cpp)
target_link_libraries(${PROJECT_NAME} ${YAML_CPP_LIBRARIES})
target_include_directories(simulator PUBLIC
    cpp/inc)
target_compile_features(simulator PUBLIC c_std_99 cxx_std_17)  # Require C99 and C++17
