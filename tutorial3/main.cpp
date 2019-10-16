#include <iostream>
#include <unistd.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "sobel.hpp"

using namespace cv;

#define CAMERAINDEX 0
#define NUMTHREADS  2

int main(int argc, char *argv[]) {
    VideoCapture cap;
    if (argc == 1) {
        if (!cap.open(CAMERAINDEX)) {
            return -1;
        }
    } else if (argc == 2) {
        if (!cap.open(argv[1])) {
            std::cout << "Could not open " << argv[1] << std::endl;
            return -1;
        }
    } else {
        std::cout << "usage: sobel [videofile]" << std::endl;
        return -1;
    }

    if (!cap.isOpened())
        return -1;

    sobelInit(NUMTHREADS, "Sobel Output");
    sobelVideo(cap, NUMTHREADS);
    while(true)
        sleep(1);
}
