#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <fcntl.h>
#include "objdet.h"

float pi = 3.141592;
float step2rad = pi / 360;
float rad2step = 360 / pi;

int angleIdx[] = {24, 23, 14, 10, 11, 9};
int stepMin[] = {310, 400, 135, 150, 135, 150};
int stepZero[] = {310, 400, 470, 480, 360, 360}; 
int stepMax[] = {310, 400, 500, 500, 510, 410};
int stepDir[] = {1, 1, -1, -1, -1, 1};

float angleMax[] = {0, 0, 2.88, 2.88, 1.92, 1.83};
float angleMin[] = {0, 0, -0.26, -0.17, -1.31, -0.44};

int clampStep(int s, int i) {
    if (s < stepMin[i]) return stepMin[i];
    if (s > stepMax[i]) return stepMax[i];
    return s;
}

float normalizeAngle(float angle) {
    while (angle > pi) {
        angle -= pi;
    } 
    while (angle < -pi) {
        angle += pi;
    }
    return angle;
}

void anglesToSteps(float *a, int *s) {
    for (int i = 0; i < 6; i++) {
        s[i] = clampStep((int) rad2step * normalizeAngle(a[i]) * stepDir[i] + stepZero[i], i);
    }
}

cv::Mat rotationMatrix(int axis, float angle) {
    float s = sin(angle);
    float c = cos(angle);
    switch (axis) {
        case 0: return (cv::Mat_<float>(3,3) << 1, 0, 0, 0, c, -s, 0, s, c);
        case 1: return (cv::Mat_<float>(3,3) << c, 0, s, 0, 1, 0, -s, 0, c);
        case 2: return (cv::Mat_<float>(3,3) << c, -s, 0, s, c, 0, 0, 0, 1);
    }
}

cv::Mat d1 = (cv::Mat_<float>(3,1) << 0, 230, 0);
cv::Mat d2 = (cv::Mat_<float>(3,1) << 0, 110, 0);
cv::Mat d3 = (cv::Mat_<float>(3,1) << -125, 0, 0);
cv::Mat d4 = (cv::Mat_<float>(3,1) << 0, 70, 0);
cv::Mat d5 = (cv::Mat_<float>(3,1) << 30, 85, -80);

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

float clampAngle(float a, int i) {
    if (a < angleMin[i]) return angleMin[i];
    if (a > angleMax[i]) return angleMax[i];
    return a;
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





