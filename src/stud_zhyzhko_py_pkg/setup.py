from setuptools import find_packages, setup

package_name = 'stud_zhyzhko_py_pkg'

setup(
    name=package_name,
    version='0.0.0',
    packages=find_packages(exclude=['test']),
    data_files=[
        ('share/ament_index/resource_index/packages',
            ['resource/' + package_name]),
        ('share/' + package_name, ['package.xml']),
    ],
    install_requires=['setuptools'],
    zip_safe=True,
    maintainer='therapy',
    maintainer_email='therapy@todo.todo',
    description='TODO: Package description',
    license='TODO: License declaration',
    extras_require={
        'test': [
            'pytest',
        ],
    },
    entry_points={
        'console_scripts': [
            'stud_zhyzhko_subscriber = stud_zhyzhko_py_pkg.student_subscriber:main',
            'stud_zhyzhko_temperature_monitor = stud_zhyzhko_py_pkg.stud_zhyzhko_temperature_monitor:main'
        ],
    },
)
