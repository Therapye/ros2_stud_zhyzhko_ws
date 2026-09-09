import rclpy
from rclpy.node import Node
from std_msgs.msg import String


class StudentSubscriber(Node):

    def __init__(self):
        super().__init__('stud_zhyzhko_subscriber')
        self.subscription = self.create_subscription(
            String,
            '/stud_zhyzhko/message',
            self.listener_callback,
            10
        )
        self.get_logger().info('stud_zhyzhko_subscriber node has been started.')

    def listener_callback(self, msg):
        self.get_logger().info(f'Received: "{msg.data}"')


def main(args=None):
    rclpy.init(args=args)
    node = StudentSubscriber()
    try:
        rclpy.spin(node)
    except KeyboardInterrupt:
        pass
    finally:
        node.destroy_node()
        rclpy.shutdown()


if __name__ == '__main__':
    main()