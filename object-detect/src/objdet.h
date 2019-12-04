#ifndef __OBJDET_H__
#define __OBJDET_H__

#include <stdio.h>
#include <opencv2/opencv.hpp>
#include <math.h>
#include <fcntl.h>

extern float pi;
extern float step2rad;
extern float rad2step;

extern int angleIdx[];
extern int stepMin[];
extern int stepZero[];
extern int stepMax[];
extern int stepDir[];

extern float angleMax[];
extern float angleMin[];

int clampStep(int s, int i);

float normalizeAngle(float angle);

void anglesToSteps(float *a, int *s);

void gradientDescent(float *a, cv::Mat target, float step);

cv::Mat calculateArm(float *a);


#endif

