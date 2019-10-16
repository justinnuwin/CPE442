#ifndef _SOBEL_H_
#define _SOBEL_H_

#include <opencv2/videoio.hpp>

void sobelVideo(cv::VideoCapture &cap, int numThreads);
void sobelInit(int numThreads, const char displayWindowName[]);
void sobelCleanup();

#endif
