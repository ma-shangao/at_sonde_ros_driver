# at_sonde_ros_driver

## Overview
This package provides a ROS 2 driver to stream the monitored parameters of an In-Situ Aqua TROLL Multiparameter Sonde, through modbus serial communication.

This package has been tested with the AT600 Sonde.

## Requirements
Prepare the required dependencies with rosdep:
```bash
rosdep install --from-paths src --ignore-src -r -y
```

## ROS 2 Node: `at_sonde_ros_driver_node`

### Published Topics
List the topics the node publishes:
| Topic Name                     | Message Type           | Description                             |
|--------------------------------|------------------------|-----------------------------------------|
| see ROS param `pub_topic_names`| `std_msgs/msg/Float32` | Publisher of selected sonde parameters |
| ...|

### Parameters
Describe the configurable ROS parameters for the node:
| Parameter Name                | Type                       | Default Value    | Description                                                             |
|-------------------------------|----------------------------|------------------|-------------------------------------------------------------------------|
| `modbus_debug_flag`           | bool                       | true             | Enable debug messages for modbus communication                          |
| `modbus_timeout_microseconds` | int                        | 700000           | Timeout for modbus communication in microseconds                        |
| `modbus_timeout_seconds`      | int                        | 0                | Timeout for modbus communication in seconds                             |
| `pub_topic_names`             | `std::vector<std::string>` | `{"temperature","battery_remaining"}` | Names of the topics to publish the streamed data   |
| `retry_limit`                 | int                        | 5                | Number of retries for modbus communication                              |
| `sensor_scan_flag`            | bool                       | false            | Scan the sensor on the first use after reconfiguration                  |
| `sonde_add`                   | int                        | 1                | Sonde address for modbus communication                                  |
| `streaming_param_reg_adds`    | `std::vector<int64_t>`     | `{5450, 5674}`   | Register addresses of the parameters to be streamed                     |

The `pub_topic_names` and `streaming_param_reg_adds` parameters must be set in pairs. The first element of `pub_topic_names` corresponds to the first element of `streaming_param_reg_adds`, and so on. 

### Usage
Provide instructions on how to run the node:
```bash
ros2 run at_sonde_ros_driver at_sonde_ros_driver_node
```

## Known Issues
Some ROS 2 parameters can currently only be set at the start of the node. 

