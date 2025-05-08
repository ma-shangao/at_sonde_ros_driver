cmake_minimum_required(VERSION 3.8)
project(at_sonde_ros_driver)

if(CMAKE_COMPILER_IS_GNUCXX OR CMAKE_CXX_COMPILER_ID MATCHES "Clang")
  add_compile_options(-Wall -Wextra -Wpedantic)
endif()

# find dependencies
find_package(ament_cmake REQUIRED)
find_package(rclcpp REQUIRED)
find_package(std_msgs REQUIRED)

find_library(LIBMODBUS
  NAMES libmodbus.so
  PATHS /usr/lib
)
find_path(LIBMODBUS_INCLUDE_DIR
  NAMES modbus.h
  PATHS /usr/include/modbus
)

add_executable(${PROJECT_NAME}_node src/driver.cpp)

ament_target_dependencies(${PROJECT_NAME}_node
  rclcpp
  std_msgs
)

target_link_libraries(${PROJECT_NAME}_node
  ${LIBMODBUS}
)
target_include_directories(${PROJECT_NAME}_node
  PRIVATE ${LIBMODBUS_INCLUDE_DIR}
)

install(TARGETS
  ${PROJECT_NAME}_node
  DESTINATION
  lib/${PROJECT_NAME}
)

if(BUILD_TESTING)
  if(NOT $ENV{ROS_DISTRO} STREQUAL "humble")
    find_package(ament_lint_auto REQUIRED)
    ament_lint_auto_find_test_dependencies()
  endif()
endif()

ament_package()
