#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>
#include <opencv2/highgui.hpp>

#include "grayscale.hpp"
#include "sobel.hpp"

using namespace std;
using namespace cv;

#define CAMERAINDEX 0

int main(int argc, char *argv[]) {

    VideoCapture cap(0);
    if (!cap.isOpened())
        return -1;
    Mat test;
    cap.read(test);

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
