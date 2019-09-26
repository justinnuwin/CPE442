#include "grayscale.hpp"

using namespace cv;

#define CCIR601_R_COEF 0.299
#define CCIR601_G_COEF 0.587 
#define CCIR601_B_COEF 0.114

inline uchar computeGamma(const uchar* bgrPixel) {
    double b = (double)bgrPixel[0];
    double g = (double)bgrPixel[1];
    double r = (double)bgrPixel[2];
    return (uchar)(CCIR601_B_COEF * b + CCIR601_G_COEF * g + CCIR601_R_COEF * r);
}


// Based off of: https://docs.opencv.org/2.4/doc/tutorials/core/how_to_scan_images/how_to_scan_images.html#the-efficient-way
void toGrayscale(Mat &input, Mat &output) {
    /* Converts to grayscale following CCIR 601 */

    // accept only char type matrices
    CV_Assert(input.type() == CV_8UC3);

    int channels = input.channels();
    int nRows = input.rows;
    int nCols = input.cols;
    output.create(nRows, nCols, CV_8UC1);   // TODO: input.size() would work better check if it is row, width, channel or not

    if (input.isContinuous() && output.isContinuous()) {
        nCols *= nRows;
        nRows = 1;
    }

    uchar *inputRow_p, *outputRow_p;
    for(int i = 0; i < nRows; i++) {
        inputRow_p = input.ptr<uchar>(i);
        outputRow_p = output.ptr<uchar>(i);
        for (int j = 0; j < nCols; j++) {
            outputRow_p[j] = computeGamma(&(inputRow_p[j * channels]));
        }
    }
}
