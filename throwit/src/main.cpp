#include <stdio.h>
#include <string.h>
#include <sl/Camera.hpp>
#include <opencv2/opencv.hpp>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>

using namespace std;
using namespace sl;

cv::Mat slMat2cvMat(Mat& input);
string type2str(int type);
float compare(uchar L, uchar a, uchar b, float d, uchar L2, uchar a2, uchar b2, float d2);
int findCompMap(int* compMap, int comp);

int mapSize = 200000;

float wLight = 10;
float wColor = 250;
float wDepth = 400;
float thresh = 50;
float redThresh = 145;
float unit = 10;

// camera calibration constants
float fx = 347.542;
float fy = 347.542;
float cx = 325.756;
float cy = 184.519;


const char *fast_cmd = "!";
const char *take_cmd = "o";
const char *grasp_cmd = "g";
const char *lift_cmd = "O";
const char *release_cmd = "r";
const char *return_cmd = "*";

void printInfo() {
    printf("Unit:\t%.2f [,/.]\nLight:\t%.2f [A/Z]\nColor:\t%.2f [S/X]\nDepth:\t%.2f [D/C]\nThresh:\t%.2f [F/V]\nRed:\t%.2f [G/B]\n\n", unit, wLight, wColor, wDepth, thresh, redThresh);
}

int main(int argc, char **argv) {
    
    // calculate angle limits
//    for (int i = 0; i < 6; i++) {
//        angleMin[i] = step2rad * (stepMin[i] - stepZero[i]);
//        angleMax[i] = step2rad * (stepMax[i] - stepZero[i]);
//        printf("%d %f %f\n", i, angleMin[i], angleMax[i]);
//    }

    int serial = open("/dev/ttyUSB0", O_WRONLY);
    if (serial < 0) printf("Not connected to arduino\n");

/*    float angles[] = {0, 0, 0, 0, 0, 0};
    int steps[6];
    memcpy(steps, stepZero, 6 * sizeof(int)); */
    char cmd[20];
    
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
    cv::Mat img = slMat2cvMat(zed_img);

    cv::Mat comp, lab, seg, isRed;
    
    // values pixel for comparison
    uchar L, a, b, L2, a2, b2;
    float d, d2, pr, pc;
    
    int compMap[mapSize];
    int lMap[mapSize];
    int aMap[mapSize];
    int bMap[mapSize];
    int sMap[mapSize];

    time_t last_time_throw = 0;
    
    float x = 0, y = 0, z = 0;
    
    printInfo();
    
    if (write(serial, fast_cmd, strlen(fast_cmd)) < 0) printf("error writing to arduino\n");
    char key = ' ';
    while (key != 'q') {
        if (zed.grab(runtime) == SUCCESS) {
            // retrieve image and depth maps
            zed.retrieveImage(zed_img, VIEW_LEFT);
            //zed.retrieveMeasure(zed_dep, MEASURE_DEPTH);

            //cv::threshold(dep, dep, 5000, 0, cv::THRESH_TRUNC);
            //double min, max;
            //cv::minMaxLoc(dep, &min, &max);
            //cv::imshow("dep", dep / max);

            cv::imshow("img", img);

            // convert to L*a*b for better color comparison
            //cv::cvtColor(img, lab, CV_BGR2Lab);
            //cv::imshow("lab", lab);

            // === SEGMENTATION ===
            
            // init component matrix
            int rows = img.rows;
            int cols = img.cols;
            uchar* pix = img.ptr<uchar>(0);
            int chans = img.channels();
            int count_blue = 0;
            for (int r = 0; r < rows; r++)
            {
              for (int c = 0; c < cols; c++)
              {
                int red = pix[(r * cols + c) * chans + 2];
                int green = pix[(r * cols + c) * chans + 1];
                int blue = pix[(r * cols + c) * chans];
                //printf("%d %d %d | \n", red, green, blue);
                if ((blue > red * 2 ) && (blue > green * 2) && (blue > 40)) 
                {
                  count_blue++;
                  red /= 3;
                  green /= 3;
                  pix[(r * cols + c) * chans] = red;
                  green = pix[(r * cols + c) * chans] = green;
                }
              }
            }
            if (count_blue > rows * cols / 30) 
            {
               printf("see blue\n");
               time_t tm2;
               time(&tm2);
               if (tm2 - last_time_throw > 5)
               {
                 printf("take\n");
                 if (write(serial, take_cmd, strlen(take_cmd)) < 0) printf("error writing to arduino\n");
                 sleep(3);
                 if (write(serial, grasp_cmd, strlen(grasp_cmd)) < 0) printf("error writing to arduino\n");
                 sleep(3);
                 if (write(serial, lift_cmd, strlen(lift_cmd)) < 0) printf("error writing to arduino\n");
                 sleep(3);
                 if (write(serial, release_cmd, strlen(lift_cmd)) < 0) printf("error writing to arduino\n");
                 sleep(1);
                 if (write(serial, return_cmd, strlen(return_cmd)) < 0) printf("error writing to arduino\n");
                 time(&last_time_throw); 
               }              
            }
            else printf("b: %d\n", count_blue);

            cv::imshow("blue", img);
         }
         key = cv::waitKey(1);
    }
    close(serial);
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

