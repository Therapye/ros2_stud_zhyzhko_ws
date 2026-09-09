#include <micro_ros_arduino.h>
#include <stdio.h>
#include <rcl/rcl.h>
#include <rcl/error_handling.h>
#include <rclc/rclc.h>
#include <rclc/executor.h>
#include <std_msgs/msg/float32.h>
#include "DHT.h"

#define DHTPIN 4
#define DHTTYPE DHT22
DHT dht(DHTPIN, DHTTYPE);

#define RCCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){return false;}}
#define RCSOFTCHECK(fn) { rcl_ret_t temp_rc = fn; if((temp_rc != RCL_RET_OK)){}}

rcl_node_t node;
rclc_support_t support;
rcl_allocator_t allocator;
rclc_executor_t executor;
rcl_timer_t timer;

rcl_publisher_t pub_celsius;
rcl_publisher_t pub_fahrenheit;

std_msgs__msg__Float32 msg_celsius;
std_msgs__msg__Float32 msg_fahrenheit;

const float FAHRENHEIT_MULT_NUM = 9.0;
const float FAHRENHEIT_MULT_DEN = 5.0;
const float FAHRENHEIT_OFFSET = 32.0;
const float DEFAULT_TEMP = 20.0;

enum States 
{
  WAITING_AGENT,
  AGENT_AVAILABLE,
  AGENT_CONNECTED,
  AGENT_DISCONNECTED
} state;


void init_sensor() 
{
  dht.begin();
}


float celsius_to_fahrenheit(float c) 
{
  return (c * FAHRENHEIT_MULT_NUM / FAHRENHEIT_MULT_DEN) + FAHRENHEIT_OFFSET;
}


float read_temperature_celsius() 
{
  float temp = dht.readTemperature();
  static float last_valid_temp = DEFAULT_TEMP;

  if (isnan(temp)) 
  {
    return last_valid_temp; 
  }
  
  last_valid_temp = temp;
  return temp;
}


void publish_data() 
{
  float temp_c = read_temperature_celsius();
  float temp_f = celsius_to_fahrenheit(temp_c);

  msg_celsius.data = temp_c;
  msg_fahrenheit.data = temp_f;

  RCSOFTCHECK(rcl_publish(&pub_celsius, &msg_celsius, NULL));
  RCSOFTCHECK(rcl_publish(&pub_fahrenheit, &msg_fahrenheit, NULL));
}


void timer_callback(rcl_timer_t * timer, int64_t last_call_time) 
{
  RCLC_UNUSED(last_call_time);
  if (timer != NULL) 
  {
    publish_data();
  }
}


bool create_entities() 
{
  allocator = rcl_get_default_allocator();

  RCCHECK(rclc_support_init(&support, 0, NULL, &allocator));

  RCCHECK(rclc_node_init_default(&node, "stud_zhyzhko_esp32", "", &support));

  RCCHECK(rclc_publisher_init_default(
    &pub_celsius,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "/stud_zhyzhko/temperature/celsius"
  ));

  RCCHECK(rclc_publisher_init_default(
    &pub_fahrenheit,
    &node,
    ROSIDL_GET_MSG_TYPE_SUPPORT(std_msgs, msg, Float32),
    "/stud_zhyzhko/temperature/fahrenheit"
  ));

  const unsigned int timer_timeout = 1000;
  RCCHECK(rclc_timer_init_default(
    &timer,
    &support,
    RCL_MS_TO_NS(timer_timeout),
    timer_callback
  ));

  RCCHECK(rclc_executor_init(&executor, &support.context, 1, &allocator));
  RCCHECK(rclc_executor_add_timer(&executor, &timer));

  return true;
}


void destroy_entities() 
{
  rmw_context_t * rmw_context = rcl_context_get_rmw_context(&support.context);
  (void) rmw_uros_set_context_entity_destroy_session_timeout(rmw_context, 0);

  rcl_publisher_fini(&pub_celsius, &node);
  rcl_publisher_fini(&pub_fahrenheit, &node);
  rcl_timer_fini(&timer);
  rclc_executor_fini(&executor);
  rcl_node_fini(&node);
  rclc_support_fini(&support);
}


void setup() 
{
  set_microros_transports();
  init_sensor();
  state = WAITING_AGENT;
}


void loop() 
{
  static unsigned long last_ping = 0;

  switch (state) 
  {
    case WAITING_AGENT:
      if (millis() - last_ping > 500) 
      {
        last_ping = millis();
        state = (rmw_uros_ping_agent(100, 1) == RMW_RET_OK) ? AGENT_AVAILABLE : WAITING_AGENT;
      }
      break;

    case AGENT_AVAILABLE:
      state = (create_entities()) ? AGENT_CONNECTED : WAITING_AGENT;
      if (state == WAITING_AGENT) 
      {
        destroy_entities();
      }
      break;

    case AGENT_CONNECTED:
      if (millis() - last_ping > 1000) 
      {
        last_ping = millis();
        state = (rmw_uros_ping_agent(100, 1) == RMW_RET_OK) ? AGENT_CONNECTED : AGENT_DISCONNECTED;
      }
      if (state == AGENT_CONNECTED) 
      {
        rclc_executor_spin_some(&executor, RCL_MS_TO_NS(100));
      }
      break;

    case AGENT_DISCONNECTED:
      destroy_entities();
      state = WAITING_AGENT;
      break;
  }
}