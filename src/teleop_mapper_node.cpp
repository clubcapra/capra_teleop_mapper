#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <std_msgs/msg/int32.hpp>

using std::placeholders::_1;

class TeleopMapper : public rclcpp::Node
{
public:
  TeleopMapper()
  : Node("teleop_mapper"), active_(1)
  {
    joy_pub_ = this->create_publisher<sensor_msgs::msg::Joy>(
      "/joy", rclcpp::QoS(10));

    joy1_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
      "/joy1", rclcpp::QoS(10),
      std::bind(&TeleopMapper::joy1_cb, this, _1));

    joy2_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
      "/joy2", rclcpp::QoS(10),
      std::bind(&TeleopMapper::joy2_cb, this, _1));

    select_sub_ = this->create_subscription<std_msgs::msg::Int32>(
      "/joy_select", rclcpp::QoS(10),
      std::bind(&TeleopMapper::select_cb, this, _1));

    RCLCPP_INFO(this->get_logger(), "Joy mux started (default: joy1)");
  }

private:
  void select_cb(const std_msgs::msg::Int32::SharedPtr msg)
  {
    if (msg->data == 1 || msg->data == 2) {
      active_ = msg->data;
      RCLCPP_INFO(this->get_logger(), "Switched to /joy%d", active_);
    } else {
      RCLCPP_WARN(this->get_logger(),
                  "Invalid joy_select value: %d (use 1 or 2)",
                  msg->data);
    }
  }

  void joy1_cb(const sensor_msgs::msg::Joy::SharedPtr msg)
  {
    if (active_ == 1) {
      joy_pub_->publish(*msg);
    }
  }

  void joy2_cb(const sensor_msgs::msg::Joy::SharedPtr msg)
  {
    if (active_ == 2) {
      joy_pub_->publish(*msg);
    }
  }

  int active_;

  rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr joy_pub_;
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy1_sub_;
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy2_sub_;
  rclcpp::Subscription<std_msgs::msg::Int32>::SharedPtr select_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TeleopMapper>());
  rclcpp::shutdown();
  return 0;
}
