Lilli main program for its Arduino Nano controller.

Lilli is a robot built by Per Salkowitsch.

This program is part of CU-Lilli software.
 https://github.com/Robotics-DAI-FMFI-UK/cu-lIllI
 
Homepage of the robot is:
 https://kempelen.dai.fmph.uniba.sk/lilli

The MPU6050 code is taken from Jeff Rowberg's github:
 https://github.com/jrowberg/i2cdevlib
 
Adafruit Servo Controller Arduino library is from:
 https://github.com/adafruit/Adafruit-PWM-Servo-Driver-Library

This program shows a very naive approach to balancing,
operates only in two locations - turning shoulders
and hips lifting forward using low-cost GY-87 IMU sensor
that computes yaw-pitch-roll angles based on 6DOF.

Press 'i' to verify that the IMU sensor has no drift,
then press 'b' to enter balancing mode - after first 'b'
balance the robot by hand so that it loads the neutral
sensor value, then hit 'b' again to start self-balancing.



