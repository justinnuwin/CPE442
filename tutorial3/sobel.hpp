#ifndef _SOBEL_H_
#define _SOBEL_H_

#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

void sobelVideo(cv::VideoCapture &cap, int numThreads);
void sobelInit(int numThreads);
void sobelCleanup();

#endif
