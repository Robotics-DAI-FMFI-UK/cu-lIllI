 Lilli demo for 3D Expo 2018 in Bratislava

 Lilli is a robot built by Per Salkowitsch.
 
 This program is part of CU-Lilli software.
  https://github.com/Robotics-DAI-FMFI-UK/cu-lIllI
  
 Homepage of the robot is:
  https://kempelen.dai.fmph.uniba.sk/lilli

Choreography format
-------------------

starts with @ and then every line of the text file has 3 numbers:

X Y Z

X - time in milliseconds since the dance start (16bit)

Y - servo number 0-31 (see below) or special command (also see below) (8bit)

Z - servo position or argument for special command (16bit)`

`# lines starting with hashtag are comments`

Choreography is terminated by 

0 0 0

Special commands
----------------

* 32-63: setup servo delay (servo num. arg-32) 
* 64 setup the total dance time (to finish in arg seconds from now) 
* 65 "goto" - jump to a specified line (comments are not counted)
* 66 play mp3 
* 67 reset time - the following times will be relative to this moment
* 68 subroutine start - commands will not be executed, only when called by cmd 70 somewhere
* 69 subroutine end   - times between 68 and 69 are relative to the position of sub call
* 70 call subroutine
* 71 beep


Servo numbering 
---------------

(you need to replace the servo numbers if yours is different)

*   LEFT_FOOT                       0
*   LEFT_KNEE                       1
*   LEFT_HIP_LIFTING_FWD            2
*   LEFT_HIP_TURNING                3
*   LEFT_HIP_LIFTING_SIDEWAYS       4
*   LEFT_SHOULDER_LIFTING          10
*   LEFT_ARM_TURNING               11
*   LEFT_SHOULDER_TURNING          14
*   LEFT_ELBOW                      9
*   LEFT_WRIST                     12
*   LEFT_HAND_CLOSING              13
*   TORSO                          15
*   HEAD_TURNING                   23
*   HEAD_LIFTING                   24
*   RIGHT_FOOT                     16
*   RIGHT_KNEE                     17
*   RIGHT_HIP_LIFTING_FWD          19
*   RIGHT_HIP_TURNING              18
*   RIGHT_HIP_LIFTING_SIDEWAYS     20
*   RIGHT_SHOULDER_LIFTING         26
*   RIGHT_ARM_TURNING              22
*   RIGHT_SHOULDER_TURNING         25
*   RIGHT_ELBOW                    21
*   RIGHT_WRIST                    29
*   RIGHT_HAND_CLOSING             28

  
Example choreographies
----------------------

```
@
# simple waving with left elbow + song 3 and time reset test
1 41 7 
2 66 3
10 71 0
1000 9 170
5000 9 250
8000 67 0
1 41 3
1000 9 170
5000 9 250
8000 71 0
0 0 0 

@
# 30 seconds of repeated waving with left elbow
1 41 2 
2 66 3
3 64 30
10 71 0
1000 9 170
5000 9 250
8000 65 4
0 0 0

@
# testing a wave-elbow as a procedure
1 41 2
2 66 3
3 68 1
1000 9 170
5000 9 250
8000 69 1
1000 70 1
2000 70 1
3000 70 1
10 71 0
0 0 0
```

See also demo.txt.
