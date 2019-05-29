#include <stdio.h>
#include <string.h>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>

using namespace std;
using namespace sl;

cv::Mat slMat2cvMat(Mat& input);
string type2str(int type);
float compare(uchar L, uchar a, uchar b, float d, uchar L2, uchar a2, uchar b2, float d2);
int findCompMap(int* compMap, int comp);

int mapSize = 200000;

float wLight = 100;
float wColor = 250;
float wDepth = 400;
float thresh = 50;
float unit = 10;
    
int main(int argc, char **argv) {
    
    Camera zed;
    
    InitParameters init;
    init.sdk_verbose = false;
    init.camera_resolution = RESOLUTION_VGA;
    init.camera_fps = 30;
    init.depth_mode = DEPTH_MODE_ULTRA;//MEDIUM;
    init.coordinate_units = UNIT_MILLIMETER;
    
    RuntimeParameters runtime;
    runtime.sensing_mode = SENSING_MODE_FILL;
    
    ERROR_CODE err = zed.open(init);
    if (err != SUCCESS) exit(-1);

    sl::Mat zed_img(zed.getResolution(), MAT_TYPE_8U_C4);
    sl::Mat zed_dep(zed.getResolution(), MAT_TYPE_32F_C1);
    cv::Mat img = slMat2cvMat(zed_img);
    cv::Mat dep = slMat2cvMat(zed_dep);

    cv::Mat comp, lab;
    
    // values pixel for comparison
    uchar L, a, b, L2, a2, b2;
    float d, d2, pr, pc;
    
    int compMap[mapSize];
    int lMap[mapSize];
    int aMap[mapSize];
    int bMap[mapSize];
    int sMap[mapSize];
    
    printf("Unit:\t%.2f [,/.]\nLight:\t%.2f [A/Z]\nColor:\t%.2f [S/X]\nDepth:\t%.2f [D/C]\nThresh:\t%.2f [F/V]\n\n", unit, wLight, wColor, wDepth, thresh);
    
    char key = ' ';
    while (key != 'q') {
        if (zed.grab(runtime) == SUCCESS) {
            // retrieve image and depth maps
            zed.retrieveImage(zed_img, VIEW_LEFT);
            zed.retrieveMeasure(zed_dep, MEASURE_DEPTH);

            cv::threshold(dep, dep, 5000, 0, cv::THRESH_TRUNC);
            double min, max;
            cv::minMaxLoc(dep, &min, &max);
            cv::imshow("dep", dep / max);

            cv::imshow("img", img);

            // convert to L*a*b for better color comparison
            cv::cvtColor(img, lab, CV_BGR2Lab);
            //cv::imshow("lab", lab);

            // === SEGMENTATION ===
            
            // init component matrix
            int rows = lab.rows;
            int cols = lab.cols;
            comp = cv::Mat::zeros(rows, cols, CV_32SC1);
            
            int newComp = 1;
            // clear arrays
            fill_n(compMap, mapSize, 0);
            fill_n(lMap, mapSize, 0);
            fill_n(aMap, mapSize, 0);
            fill_n(bMap, mapSize, 0);
            fill_n(sMap, mapSize, 0);
            
            // our matrices are continuous - 1 long row - only 1 pointer needed
            // row pointers
            uchar* lPtr = lab.ptr<uchar>(0);
            int* cPtr = comp.ptr<int>(0);
            float* dPtr = dep.ptr<float>(0);
            // channel counts
            int lChan = lab.channels(), dChan = dep.channels(), cChan = comp.channels();
            // iterate
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    pr = 0;
                    pc = 0;
                    // get data for this pixel
                    L = lPtr[(r * cols + c) * lChan];
                    a = lPtr[(r * cols + c) * lChan + 1];
                    b = lPtr[(r * cols + c) * lChan + 2];
                    d = dPtr[(r * cols + c) * dChan];
                    if (r > 0) { // compare with neighbor in previous row
                        L2 = lPtr[((r-1) * cols + c) * lChan];
                        a2 = lPtr[((r-1) * cols + c) * lChan + 1];
                        b2 = lPtr[((r-1) * cols + c) * lChan + 2];
                        d2 = dPtr[((r-1) * cols + c) * dChan];
                        pr = compare(L, a, b, d, L2, a2, b2, d2);
                    }
                    if (c > 0) { // compare with neighbor in previous column
                        L2 = lPtr[(r * cols + c-1) * lChan];
                        a2 = lPtr[(r * cols + c-1) * lChan + 1];
                        b2 = lPtr[(r * cols + c-1) * lChan + 2];
                        d2 = dPtr[(r * cols + c-1) * dChan];
                        pc = compare(L, a, b, d, L2, a2, b2, d2);
                    }
                    //cPtr[(r * cols + c) * cChan] = (uchar) (pr < pc? pr : pc);
                    
                    int compToSet = 0;
                    if (r > 0) {
                        if (c > 0) {
                            // inside image
                            if (pr < thresh) {
                                if (pc < thresh) {
                                    // both similar
                                    int compR = cPtr[((r-1) * cols + c) * cChan];
                                    int compC = cPtr[(r * cols + c - 1) * cChan];
                                    if (compR == compC) {
                                        // both previous row and colum are in the same component
                                        compToSet = compR;
                                    } else {
                                        // different components, need to mark as same
                                        // check if they are merged already
                                        int mR = findCompMap(compMap, compR);
                                        int mC = findCompMap(compMap, compC);
                                        if (mR < mC) {
                                            compToSet = mR;
                                            compMap[compC] = mR;
                                        } else {
                                            compToSet = mC;
                                            compMap[compR] = mC;
                                        }
                                    }
                                } else {
                                    // only previous row similar
                                    compToSet = cPtr[((r-1) * cols + c) * cChan];
                                }
                            } else {
                                if (pc < thresh) {
                                    // only previous column similar
                                    compToSet = cPtr[(r * cols + c - 1) * cChan];
                                } else {
                                    // neither similar
                                    compToSet = newComp++;
                                }
                            }
                        } else {
                            // first column - check only previous row
                            if (pr < thresh) compToSet = cPtr[((r-1) * cols) * cChan]; // similar - same component
                            else compToSet = newComp++;
                        }
                    } else {
                        if (c > 0) {
                            // first row - check only previous column
                            if (pc < thresh) compToSet = cPtr[(c-1) * cChan]; // similar - same component
                            else compToSet = newComp++; // else - new component
                        } else {
                            // first corner - new component
                            compToSet = newComp++;
                        }
                    }
                    cPtr[(r * cols + c) * cChan] = compToSet;
                    if (newComp >= mapSize) break;
                }
                if (newComp >= mapSize) break;
            }
            
            // iterate over segment image, merging and collecting segment stats
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    // replace using map if needed
                    int comp = cPtr[(r * cols + c) * cChan];
                    comp = findCompMap(compMap, comp);
                    cPtr[(r * cols + c) * cChan] = comp;
                    // set data
                    lMap[comp] += lPtr[(r * cols + c) * lChan];
                    aMap[comp] += lPtr[(r * cols + c) * lChan + 1];
                    bMap[comp] += lPtr[(r * cols + c) * lChan + 2];
                    sMap[comp] += 1;

                }
            }
            
            //cv::imshow("comp", comp);

            // create colored component image
            cv::Mat lab_seg = cv::Mat::zeros(rows, cols, CV_8UC3);
            lPtr = lab_seg.ptr<uchar>(0);
            lChan = lab_seg.channels();
            for (int r = 0; r < rows; r++) {
                for (int c = 0; c < cols; c++) {
                    int comp = cPtr[(r * cols + c) * cChan];
                    lPtr[(r * cols + c) * lChan] = lMap[comp] / sMap[comp];
                    lPtr[(r * cols + c) * lChan + 1] = aMap[comp] / sMap[comp];
                    lPtr[(r * cols + c) * lChan + 2] = bMap[comp] / sMap[comp];
                }
            }
            
            cv::cvtColor(lab_seg, lab_seg, CV_Lab2BGR);
            cv::imshow("seg", lab_seg);
            
            key = cv::waitKey(1);
            if (key != 255) {
                if (key == ',') unit /= 10;
                if (key == '.') unit *= 10;
                if (key == 'a') wLight += unit;
                if (key == 'z') wLight -= unit;
                if (key == 's') wColor += unit;
                if (key == 'x') wColor -= unit;
                if (key == 'd') wDepth += unit;
                if (key == 'c') wDepth -= unit;
                if (key == 'f') thresh += unit;
                if (key == 'v') thresh -= unit;
                printf("Unit:\t%.2f [,/.]\nLight:\t%.2f [A/Z]\nColor:\t%.2f [S/X]\nDepth:\t%.2f [D/C]\nThresh:\t%.2f [F/V]\n\n", unit, wLight, wColor, wDepth, thresh);
                }
            }
        }
    }
}

int findCompMap(int* compMap, int comp) {
    if (compMap[comp] == 0) {
        // not in map yet
        return comp;
    } else {
        // return found
        return compMap[comp];
    }
}

float compare(uchar L, uchar a, uchar b, float d, uchar L2, uchar a2, uchar b2, float d2) {
    // 4D euclidean distance
    return sqrt(wLight*(L-L2)*(L-L2) + wColor*(a-a2)*(a-a2) + wColor*(b-b2)*(b-b2) + wDepth*(d-d2)*(d-d2));
}

/**
* Conversion function between sl::Mat and cv::Mat
**/
cv::Mat slMat2cvMat(Mat& input) {
    // Mapping between MAT_TYPE and CV_TYPE
    int cv_type = -1;
    switch (input.getDataType()) {
        case MAT_TYPE_32F_C1: cv_type = CV_32FC1; break;
        case MAT_TYPE_32F_C2: cv_type = CV_32FC2; break;
        case MAT_TYPE_32F_C3: cv_type = CV_32FC3; break;
        case MAT_TYPE_32F_C4: cv_type = CV_32FC4; break;
        case MAT_TYPE_8U_C1: cv_type = CV_8UC1; break;
        case MAT_TYPE_8U_C2: cv_type = CV_8UC2; break;
        case MAT_TYPE_8U_C3: cv_type = CV_8UC3; break;
        case MAT_TYPE_8U_C4: cv_type = CV_8UC4; break;
        default: break;
    }

    // Since cv::Mat data requires a uchar* pointer, we get the uchar1 pointer from sl::Mat (getPtr<T>())
    // cv::Mat and sl::Mat will share a single memory structure
    return cv::Mat(input.getHeight(), input.getWidth(), cv_type, input.getPtr<sl::uchar1>(MEM_CPU));
}

// openCV mat type debug
string type2str(int type) {
  string r;

  uchar depth = type & CV_MAT_DEPTH_MASK;
  uchar chans = 1 + (type >> CV_CN_SHIFT);

  switch ( depth ) {
    case CV_8U:  r = "8U"; break;
    case CV_8S:  r = "8S"; break;
    case CV_16U: r = "16U"; break;
    case CV_16S: r = "16S"; break;
    case CV_32S: r = "32S"; break;
    case CV_32F: r = "32F"; break;
    case CV_64F: r = "64F"; break;
    default:     r = "User"; break;
  }

  r += "C";
  r += (chans+'0');

  return r;
}

