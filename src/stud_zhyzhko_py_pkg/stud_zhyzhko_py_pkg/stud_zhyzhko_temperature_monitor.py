#!/usr/bin/env python3
import rclpy
from rclpy.node import Node
from std_msgs.msg import Float32


class TemperatureMonitor(Node):

    def __init__(self):
        super().__init__('stud_zhyzhko_temperature_monitor')

  
        self.last_celsius = None
        self.last_fahrenheit = None


        self.sub_celsius = self.create_subscription(
            Float32,
            '/stud_zhyzhko/temperature/celsius',
            self.celsius_callback,
            10,
        )

  
        self.sub_fahrenheit = self.create_subscription(
            Float32,
            '/stud_zhyzhko/temperature/fahrenheit',
            self.fahrenheit_callback,
            10,
        )

        self.get_logger().info('Temperature Monitor node successfully started!')

    def celsius_callback(self, msg: Float32):
        self.last_celsius = msg.data
        self.print_temperature()

    def fahrenheit_callback(self, msg: Float32):
        self.last_fahrenheit = msg.data
        self.print_temperature()

    def print_temperature(self):
        c_str = (
            f'{self.last_celsius:.1f} °C'
            if self.last_celsius is not None
            else 'Waiting...'
        )
        f_str = (
            f'{self.last_fahrenheit:.1f} °F'
            if self.last_fahrenheit is not None
            else 'Waiting...'
        )

        print(
            f'\nTemperature:\n  Celsius:    {c_str}\n  Fahrenheit: {f_str}',
            flush=True,
        )


def main(args=None):
    rclpy.init(args=args)
    node = TemperatureMonitor()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()