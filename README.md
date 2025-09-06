# at_sonde_ros_driver

[![ROS Industrial CI](https://github.com/ma-shangao/at_sonde_ros_driver/actions/workflows/ros_ind_ci_action.yml/badge.svg?branch=main)](https://github.com/ma-shangao/at_sonde_ros_driver/actions/workflows/ros_ind_ci_action.yml)

[![Jenkins Build](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fbuild.ros2.org%2Fjob%2FRbin_uN64__at_sonde_ros_driver__ubuntu_noble_amd64__binary%2F&logo=ros&label=rolling%20bin)](https://build.ros2.org/job/Rbin_uN64__at_sonde_ros_driver__ubuntu_noble_amd64__binary/)
[![Jazzy Bin](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fbuild.ros2.org%2Fjob%2FJbin_uN64__at_sonde_ros_driver__ubuntu_noble_amd64__binary%2F&logo=ros&label=jazzy%20bin)](https://build.ros2.org/job/Jbin_uN64__at_sonde_ros_driver__ubuntu_noble_amd64__binary/)
[![Humble Bin](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fbuild.ros2.org%2Fjob%2FHbin_uJ64__at_sonde_ros_driver__ubuntu_jammy_amd64__binary%2F&logo=ros&label=humble%20bin)](https://build.ros2.org/job/Hbin_uJ64__at_sonde_ros_driver__ubuntu_jammy_amd64__binary/)

[![Rolling Devel](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fbuild.ros2.org%2Fjob%2FRdev__at_sonde_ros_driver__ubuntu_noble_amd64%2F&logo=ros&label=rolling%20devel)](https://build.ros2.org/job/Rdev__at_sonde_ros_driver__ubuntu_noble_amd64/)
[![Jazzy Devel](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fbuild.ros2.org%2Fjob%2FJdev__at_sonde_ros_driver__ubuntu_noble_amd64%2F&logo=ros&label=jazzy%20devel)](https://build.ros2.org/job/Jdev__at_sonde_ros_driver__ubuntu_noble_amd64/)
[![Humble Devel](https://img.shields.io/jenkins/build?jobUrl=https%3A%2F%2Fbuild.ros2.org%2Fjob%2FHdev__at_sonde_ros_driver__ubuntu_jammy_amd64%2F&logo=ros&label=humble%20devel)](https://build.ros2.org/job/Hdev__at_sonde_ros_driver__ubuntu_jammy_amd64/)

![ROS Package Index Rolling](https://img.shields.io/ros/v/rolling/at_sonde_ros_driver)
![ROS Package Index Jazzy](https://img.shields.io/ros/v/jazzy/at_sonde_ros_driver)
![ROS Package Index Humble](https://img.shields.io/ros/v/humble/at_sonde_ros_driver)

## Overview
This package provides a ROS 2 driver to stream the monitored parameters of an In-Situ Aqua TROLL Multiparameter Sonde, through modbus serial communication.

This package has been tested with the AT600 Sonde.

## Installation

Make sure you have [ROS 2](https://docs.ros.org/en/jazzy/Installation.html) installed on your system.

### Binary

To install the binary package, use the following command:
```bash
sudo apt update
sudo apt install ros-$ROS_DISTRO-at-sonde-ros-driver
```

### From Source
To build the package from source, follow these steps:
1. Create a workspace directory:
```bash
mkdir -p ~/ros2_ws/src
cd ~/ros2_ws/src
```
2. Clone the repository:
```bash
git clone https://github.com/ma-shangao/at_sonde_ros_driver.git
```
3. Prepare the required dependencies with rosdep:
```bash
cd ~/ros2_ws
rosdep install --from-paths src --ignore-src -r -y
```
4. Build the package:
```bash
colcon build
```

## ROS 2 Node: `at_sonde_ros_driver_node`

### Published Topics
The node publishes the following topics:
| Topic Name                     | Message Type           | Description                             |
|--------------------------------|------------------------|-----------------------------------------|
| see ROS param `pub_topic_names`| `std_msgs/msg/Float32` | Publisher of selected sonde parameters  |
| ...|

### ROS Parameters
The configurable ROS parameters for the node:
| Parameter Name                | Type                       | Default Value    | Description                                                             |
|-------------------------------|----------------------------|------------------|-------------------------------------------------------------------------|
| `baud`                        | int                        | 19200            | Baud rate for serial communication                                      | 
| `modbus_debug_flag`           | bool                       | false            | Enable debug messages for modbus communication                          |
| `modbus_timeout_microseconds` | int                        | 700000           | Timeout for modbus communication in microseconds                        |
| `modbus_timeout_seconds`      | int                        | 0                | Timeout for modbus communication in seconds                             |
| `pub_topic_names`             | list[string]               | ["temperature", "battery_remaining"] | Names of the topics to publish the streamed data    |
| `retry_limit`                 | int                        | 5                | Number of retries for modbus communication                              |
| `sensor_scan_flag`            | bool                       | false            | Scan the sensor on the first use after reconfiguration                  |
| `serial_port`                 | string                     | "/dev/ttyUSB0"   | Serial port for modbus communication                                    |
| `sonde_add`                   | int                        | 1                | Sonde address for modbus communication                                  |
| `streaming_param_reg_adds`    | list[int]                  | [5450, 5674]     | Register addresses of the parameters to be streamed                     |

The `pub_topic_names` and `streaming_param_reg_adds` parameters must be set in pairs. The first element of `pub_topic_names` corresponds to the first element of `streaming_param_reg_adds`, and so on.

### Usage
To run the node, use the following command:
```bash
ros2 run at_sonde_ros_driver at_sonde_ros_driver_node
```

The ROS parameters can also be configured using a YAML file.
```bash
ros2 run at_sonde_ros_driver at_sonde_ros_driver_node --ros-args --params-file <path_to_yaml_file>
```

## Known Issues
Some ROS 2 parameters can currently only be set at the start of the node. 

