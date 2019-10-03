#include <iostream>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "grayscale.hpp"
#include "sobel.hpp"

using namespace cv;

#define CAMERAINDEX 0

int main(int argc, char *argv[]) {
    VideoCapture cap;
    if (argc == 1) {
        if (!cap.open(0)) {
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

    while (true) {
        Mat frame, grayscale, edges;
        cap.read(frame);
        
        if (frame.empty())
            break;

        toGrayscale(frame, grayscale);
        sobel(grayscale, edges);

        // imshow("Video", frame);
        // imshow("Video Grayscale", grayscale);
        imshow("Video Edges", edges);
        waitKey(1);
    }
}
