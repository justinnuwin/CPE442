#include "grayscale.hpp"
#include <cmath>

using namespace cv;
using namespace std;

const int vKernel[3][3] = {{-1, 0, 1},
                           {-2, 0, 2},
                           {-1, 0, 1}};

const int hKernel[3][3] = {{-1, -2, -1},
                           { 0,  0,  0},
                           { 1,  2,  1}};

void sobel(Mat &input, Mat &output) {
    CV_Assert(input.type() == CV_8UC1);

    int nRows = input.rows;
    int nCols = input.cols;
    output.create(nRows - 2, nCols - 2, CV_8UC1);

    for(int i = 1; i < nRows - 1; i++) {
        uchar *inputRowM1_p = input.ptr(i - 1);
        uchar *inputRow0_p = input.ptr(i);
        uchar *inputRowP1_p = input.ptr(i + 1);
        uchar *outputRow_p = output.ptr(i - 1);
        for (int j = 1; j < nCols - 1; j++) {   // Compiler will optimize mult const 2
            int vGradient = inputRowM1_p[j - 1] * vKernel[0][0] +
                            // inputRowM1_p[j    ] * vKernel[0][1] +
                            inputRowM1_p[j + 1] * vKernel[0][2] +
                            inputRow0_p[j - 1] * vKernel[1][0] +
                            // inputRow0_p[j    ] * vKernel[1][1] +
                            inputRow0_p[j + 1] * vKernel[1][2] +
                            inputRowP1_p[j - 1] * vKernel[2][0] +
                            // inputRowP1_p[j    ] * vKernel[2][1] +
                            inputRowP1_p[j + 1] * vKernel[2][2];
            int hGradient = inputRowM1_p[j - 1] * hKernel[0][0] +
                            inputRowM1_p[j    ] * hKernel[0][1] +
                            inputRowM1_p[j + 1] * hKernel[0][2] +
                            // inputRow0_p[j - 1] * hKernel[1][0] +
                            // inputRow0_p[j    ] * hKernel[1][1] +
                            // inputRow0_p[j + 1] * hKernel[1][2] +
                            inputRowP1_p[j - 1] * hKernel[2][0] +
                            inputRowP1_p[j    ] * hKernel[2][1] +
                            inputRowP1_p[j + 1] * hKernel[2][2];
            // Approx. true sobel magnitude sqrt(g_x^2 + g_y^2)
            // int sum = round(sqrt(pow(vGradient, 2) + pow(hGradient, 2)));
            int sum = abs(vGradient) + abs(hGradient);
            outputRow_p[j - 1] = (uchar)(sum > 255 ? 255 : sum);
        }
    }
}
