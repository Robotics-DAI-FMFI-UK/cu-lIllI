Lilli demo for 3D Expo 2018 in Bratislava

Lilli is a robot built by Per Salkowitsch.

This program is part of CU-Lilli software.
 https://github.com/Robotics-DAI-FMFI-UK/cu-lIllI
 
Homepage of the robot is:
 https://kempelen.dai.fmph.uniba.sk/lilli

The MPU6050 code is taken from Jeff Rowberg's github:
 https://github.com/jrowberg/i2cdevlib

This program shows a very naive approach to balancing,
operates only in two locations - turning shoulders
and hips lifting forward using low-cost GY-87 IMU sensor
that computes yaw-pitch-roll angles based on 6DOF.

Press 'i' to verify that the IMU sensor has no drift,
then press 'b' to enter balancing mode - after first 'b'
balance the robot by hand so that it loads the neutral
sensor value, then hit 'b' again to start self-balancing.


Known issues:

The original code for dancing and loading choreography
from the serial port now freezes sometimes since the
interrupts clash with the IMU. It is still kept here,
but we will need another approach to integrate both
IMU and longer serial port communication in one program.
