### Object detection using ZED Mini image and depth map

Real-time object detection based on colors and depths. Output image contains segments, colored by their average color in the camera image.

# Building

    mkdir build
    cd build
    cmake ..
    make

# Running

    ./main

Program displays current segmentation parameters in standard output. Parameters can be changed using the displayed keys (with graphic windows in focus)
