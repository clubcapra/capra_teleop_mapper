#include <rclcpp/rclcpp.hpp>
#include <sensor_msgs/msg/joy.hpp>
#include <std_msgs/msg/bool.hpp>

using std::placeholders::_1;

class TeleopMapper : public rclcpp::Node
{
public:
  TeleopMapper()
  : Node("teleop_mapper"), selected_(false)
  {
    joy1_pub_ = this->create_publisher<sensor_msgs::msg::Joy>(
      "/joy1", rclcpp::QoS(10));

    joy2_pub_ = this->create_publisher<sensor_msgs::msg::Joy>(
      "/joy2", rclcpp::QoS(10));

    joy_sub_ = this->create_subscription<sensor_msgs::msg::Joy>(
      "/joy", rclcpp::QoS(10),
      std::bind(&TeleopMapper::joy_cb, this, _1));

    select_sub_ = this->create_subscription<std_msgs::msg::Bool>(
      "/joy_select", rclcpp::QoS(10),
      std::bind(&TeleopMapper::select_cb, this, _1));

    RCLCPP_INFO(this->get_logger(),
                "Joy demux started (false→/joy1, true→/joy2)");
  }

private:
  void select_cb(const std_msgs::msg::Bool::SharedPtr msg)
  {
    selected_ = msg->data;
  }

  void joy_cb(const sensor_msgs::msg::Joy::SharedPtr msg)
  {
    if (selected_) {
      joy2_pub_->publish(*msg);
    } else {
      joy1_pub_->publish(*msg);
    }
  }

  bool selected_;

  rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr joy1_pub_;
  rclcpp::Publisher<sensor_msgs::msg::Joy>::SharedPtr joy2_pub_;
  rclcpp::Subscription<sensor_msgs::msg::Joy>::SharedPtr joy_sub_;
  rclcpp::Subscription<std_msgs::msg::Bool>::SharedPtr select_sub_;
};

int main(int argc, char ** argv)
{
  rclcpp::init(argc, argv);
  rclcpp::spin(std::make_shared<TeleopMapper>());
  rclcpp::shutdown();
  return 0;
}
