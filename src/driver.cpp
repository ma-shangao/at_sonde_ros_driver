// Copyright 2025 MA Song
//
// Redistribution and use in source and binary forms, with or without
// modification, are permitted provided that the following conditions are met:
//
//    * Redistributions of source code must retain the above copyright
//      notice, this list of conditions and the following disclaimer.
//
//    * Redistributions in binary form must reproduce the above copyright
//      notice, this list of conditions and the following disclaimer in the
//      documentation and/or other materials provided with the distribution.
//
//    * Neither the name of the MA Song nor the names of its
//      contributors may be used to endorse or promote products derived from
//      this software without specific prior written permission.
//
// THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS"
// AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE
// IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE
// ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE
// LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR
// CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF
// SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS; OR BUSINESS
// INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN
// CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE)
// ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE
// POSSIBILITY OF SUCH DAMAGE.


#include <modbus.h>
#include <errno.h>

#include <iostream>
#include <cassert>
#include "rclcpp/rclcpp.hpp"
#include "rclcpp/qos.hpp"
#include "std_msgs/msg/float32.hpp"

class sonde_driver : public rclcpp::Node
{
public:
        /// @brief Constructor
  sonde_driver()
  : Node("sonde_driver")
  {
            /* ROS params */
            // Sensor scan flag
    bool sensor_scan_flag = this->declare_parameter(
                "sensor_scan_flag",
                false
    );
            // Todo: add a parameter event handler to update the parameter
            // Modbus Debug flag
    this->declare_parameter(
                "modbus_debug_flag",
                false
    );
    this->declare_parameter(
                "modbus_timeout_seconds",
                0
    );
    this->declare_parameter(
                "modbus_timeout_microseconds",
                700000
    );
    retry_limit = this->declare_parameter(
                "retry_limit",
                5
    );
            // Sonde hardware address
    int sonde_add = this->declare_parameter(
                "sonde_add",
                1
    );
            // The list of register addresses for the parameters to be streamed
    this->declare_parameter(
                "streaming_param_reg_adds",
                std::vector<int>{
      5450,
      5674
                }
    );
    this->declare_parameter(
                "pub_topic_names",
                std::vector<std::string>{
      "temperature",
      "battery_remaining"
                }
    );
    this->declare_parameter(
                "serial_port",
                std::string("/dev/ttyUSB0")
    );
    this->declare_parameter(
                "baud",
                19200
    );

    assert(
                this->get_parameter("streaming_param_reg_adds").as_integer_array().size() ==
                this->get_parameter("pub_topic_names").as_string_array().size()
    );

            /* Create publishers for the sonde data stream */
    std::vector<std::string>::iterator it;
    std::vector<std::string> pub_topics = this->get_parameter("pub_topic_names").as_string_array();
    for (it = pub_topics.begin(); it != pub_topics.end(); it++) {
      sonde_data_pubs.push_back(
                    this->create_publisher<std_msgs::msg::Float32>(
                        *it,
                        rclcpp::SensorDataQoS()
                    )
      );
    }

    mb = modbus_new_rtu(
                this->get_parameter("serial_port").as_string().c_str(),
                this->get_parameter("baud").as_int(), 'E', 8, 1);
    if (mb == NULL) {
      RCLCPP_ERROR_STREAM(
                    this->get_logger(),
                    "Unable to allocate libmodbus context" <<
                    std::endl
      );
      return;
    }
            /*Set the slave ID*/
    if (modbus_set_slave(mb, sonde_add) == -1) {
      RCLCPP_ERROR_STREAM(
                    this->get_logger(),
                    "Invalid slave ID:" <<
                    sonde_add <<
                    std::endl
      );
      modbus_free(mb);
      return;
    }
            /*Set the Debug mode*/
    if (modbus_set_debug(mb,
      static_cast<int>(this->get_parameter("modbus_debug_flag").as_bool())) == -1)
    {
      RCLCPP_ERROR_STREAM(
                    this->get_logger(),
                    "Failed to set debug mode: " <<
                    modbus_strerror(errno) <<
                    std::endl
      );
      modbus_free(mb);
      return;
    }
            /*Set the timeout interval*/
    if (modbus_set_response_timeout(
                mb,
                this->get_parameter("modbus_timeout_seconds").as_int(),
                this->get_parameter("modbus_timeout_microseconds").as_int()
      ) == -1)
    {
      RCLCPP_ERROR_STREAM(
                    this->get_logger(),
                    "Failed to set response timeout: " <<
                    modbus_strerror(errno) <<
                    std::endl
      );
      modbus_free(mb);
      return;
    }
            /*Connect to the sonde*/
    if (modbus_connect(mb) == -1) {
      RCLCPP_ERROR_STREAM(
                    this->get_logger(),
                    "Connection failed: " <<
                    modbus_strerror(errno) <<
                    std::endl
      );
      modbus_free(mb);
      return;
    }
            /*Wake up the sonde*/
    wakeUp();
            /*Scan the sensors*/
    if (sensor_scan_flag) {
      const int SCAN_SENSORS_REG_ADD = 6947;
      uint16_t scan_sensors;
      while (
        (rc = modbus_read_registers(mb,
                        SCAN_SENSORS_REG_ADD,
                        1,
                        &scan_sensors
        )) == -1 && retries < retry_limit)
      {
        retries++;
      }
      if (rc == -1) {
        RCLCPP_ERROR_STREAM(
                        this->get_logger(),
                        modbus_strerror(errno) <<
                        std::endl
        );
        modbus_close(mb);
        modbus_free(mb);
        return;
      }
    }
            /* Access and pulish the data */
    sonde_data_timer = this->create_wall_timer(
                std::chrono::milliseconds(500),
                std::bind(&sonde_driver::accessPublish, this)
    );
    RCLCPP_INFO_STREAM(
                this->get_logger(),
                "Sonde Data streaming" <<
                std::endl
    );
  }
        /// @brief Destructor
  ~sonde_driver()
  {
            // Close the connection
    modbus_close(mb);
            // Free the modbus context
    modbus_free(mb);
  }

private:
        // The modbus structure
  modbus_t * mb;
        // Retry counter
  int retries = 0;
        // modbus return code
  int rc;

        // Modbus retry limit
  int retry_limit;

        // The list of publishers for the sonde data
  std::vector<rclcpp::Publisher<std_msgs::msg::Float32>::SharedPtr> sonde_data_pubs;

        // Timer for the sonde data stream
  rclcpp::TimerBase::SharedPtr sonde_data_timer;
        /// @brief Wake up the sonde by sending a carriage return
  void wakeUp()
  {
            // Carriage return
    const uint8_t WAKEUP = 0x0D;
    modbus_send_raw_request(mb, &WAKEUP, sizeof(WAKEUP));
            // wait for 2 seconds
    rclcpp::sleep_for(std::chrono::seconds(2));
    RCLCPP_INFO_STREAM(
                this->get_logger(),
                "Waking up the device..." <<
                std::endl
    );
  }
  void accessPublish()
  {
            // Reserved for reading parameters from the registers
    uint16_t reg_tab[7];
    std_msgs::msg::Float32 msg;
    std::vector<int64_t>::iterator it;
    std::vector<int64_t> reg_adds =
      this->get_parameter("streaming_param_reg_adds").as_integer_array();
    for (it = reg_adds.begin(); it != reg_adds.end(); it++) {
      retries = 0;
      while ((rc = modbus_read_registers(
                    mb,
                    static_cast<int>(*it),
                    7,
                    reg_tab
        ) == -1) && retries < retry_limit)
      {
        retries++;
      }
      if (rc == -1) {
        RCLCPP_ERROR_STREAM(
                        this->get_logger(),
                        modbus_strerror(errno) <<
                        std::endl
        );
        modbus_close(mb);
        modbus_free(mb);
        rclcpp::shutdown();
        return;
      }
      int data_quality = reg_tab[2];
                // data quality ID 4 is a wiper warning
      if (data_quality != 0 && data_quality != 4) {
        RCLCPP_DEBUG_STREAM(
                        this->get_logger(),
                        "Data quality ID: " <<
                        data_quality
        );
        return;
      }
      msg.data = modbus_get_float_abcd(reg_tab);
      sonde_data_pubs.at(it - reg_adds.begin())->publish(msg);
    }
  }
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<sonde_driver>());
  rclcpp::shutdown();
  return 0;
}
