#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h> 

cv::Mat d1 = (cv::Mat_<float>(3,1) << 0, 230, 0);
cv::Mat d2 = (cv::Mat_<float>(3,1) << 0, 110, 0);
cv::Mat d3 = (cv::Mat_<float>(3,1) << 125, 0, 0);
cv::Mat d4 = (cv::Mat_<float>(3,1) << 0, 70, 0);
cv::Mat d5 = (cv::Mat_<float>(3,1) << -30, 85, 80);

cv::Mat rotationMatrix(int axis, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    switch (axis) {
        case 0: return (cv::Mat_<float>(3,3) << 1, 0, 0, 0, c, -s, 0, s, c);
        case 1: return (cv::Mat_<float>(3,3) << c, 0, s, 0, 1, 0, -s, 0, c);
        case 2: return (cv::Mat_<float>(3,3) << c, -s, 0, s, c, 0, 0, 0, 1);
    }
}

cv::Mat calculateArm(float *a) {
    cv::Mat d = rotationMatrix(0, a[5]) * d1; // elbow rotation
    d = rotationMatrix(1, a[4]) * d; // elbow twist
    d += d2; // move to shoulder
    d = rotationMatrix(2, a[3]) * d; // shoulder side
    d = rotationMatrix(0, a[2]) * d; // shoulder front/back
    d += d3; // move to center of chest
    d = rotationMatrix(1, a[1]) * d; // neck rotation
    d += d4; // move to head
    d = rotationMatrix(0, a[0]) * d; // head tilt
    d += d5; // move to eye
    return d;
}

// changes values of angle parameters to move the arm closer to target position
void gradientDescent(float *a, cv::Mat target, float step) {
    cv::Mat current = calculateArm(a);
    float distance = cv::norm(target, current);
    int modifiedAngle = -1;
    int direction = -2;
    for (int i = 2; i < 6; i++) { // dont change neck angles
        for (int j=-1; j<= 1; j+=2) {
            a[i] += j * step;
            float newDistance = cv::norm(target, calculateArm(a));
            if (newDistance < distance) {
                distance = newDistance;
                modifiedAngle = i;
                direction = j;
            }
            a[i] -= j * step;
        }
    }
    if (modifiedAngle != -1 && direction != -2) {
        a[modifiedAngle] += direction * step;
    }
}





