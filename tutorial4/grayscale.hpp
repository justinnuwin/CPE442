#ifndef _GRAYSCALE_H_
#define _GRAYSCALE_H_

#include <opencv2/core.hpp>
void toGrayscale(cv::Mat &input, cv::Mat &output);
void toGrayscale_threaded(int matStartingIdx, int matEndingIdx, cv::Mat &input, cv::Mat &output);

#endif
