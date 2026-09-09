#include "stud_zhyzhko_cpp_pkg/student_publisher.hpp"

using namespace std::chrono_literals;

StudentPublisher::StudentPublisher()
: Node("stud_zhyzhko_publisher"), counter_(1)
{
  publisher_ = this->create_publisher<std_msgs::msg::String>(
    "/stud_zhyzhko/message", 10);

  timer_ = this->create_wall_timer(
    500ms, std::bind(&StudentPublisher::timer_callback, this));

  RCLCPP_INFO(this->get_logger(), "stud_zhyzhko_publisher node has been started.");
}

void StudentPublisher::timer_callback()
{
  auto message = std_msgs::msg::String();
  message.data = "Zhyzhko Message #" + std::to_string(counter_++);
  
  RCLCPP_INFO(this->get_logger(), "Publishing: '%s'", message.data.c_str());
  publisher_->publish(message);
}