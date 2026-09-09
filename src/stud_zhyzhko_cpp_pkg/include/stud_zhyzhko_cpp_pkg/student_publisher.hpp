#ifndef STUD_ZHYZHKO_CPP_PKG__STUDENT_PUBLISHER_HPP_
#define STUD_ZHYZHKO_CPP_PKG__STUDENT_PUBLISHER_HPP_

#include <chrono>
#include "rclcpp/rclcpp.hpp"
#include "std_msgs/msg/string.hpp"

class StudentPublisher : public rclcpp::Node
{
public:
  StudentPublisher();

private:
  void timer_callback();

  rclcpp::Publisher<std_msgs::msg::String>::SharedPtr publisher_;
  rclcpp::TimerBase::SharedPtr timer_;
  size_t counter_;
};

#endif