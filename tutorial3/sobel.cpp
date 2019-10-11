#include "sobel.hpp"
#include <iostream>
#include <cmath>
#include <opencv2/highgui.hpp>

#include "grayscale.hpp"

using namespace cv;
using namespace std;

struct sobelThreadData {
    pthread_t &thread;
    pthread_t *threads;
    cv::VideoCapture &input;
    int numThreads;
};

const int vKernel[3][3] = {{-1, 0, 1},
                           {-2, 0, 2},
                           {-1, 0, 1}};

const int hKernel[3][3] = {{-1, -2, -1},
                           { 0,  0,  0},
                           { 1,  2,  1}};


static pthread_t threads[2];

inline void
sobel(int matStartingIdx, int matEndingIdx, int nCols, Mat &input, Mat &output) {
    for(int i = matStartingIdx; i < matEndingIdx - 1; i++) {
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

// Threaded shared globals
static pthread_mutex_t inputStreamAvailable = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t readySobel, completedSobel;
static int thisMatStartingIdx, matSliceIdx, otherMatEndingIdx;
static int nCols;
static Mat input, edgesMaster, edgesThread;

void *sobel_threaded(void *threadArgs) {
    struct sobelThreadData *threadData = (struct sobelThreadData *)threadArgs;

    while (true) {  // TODO: While not ^C
        if (pthread_mutex_trylock(&inputStreamAvailable)) {
            // Critical Section
            Mat frame;
            VideoCapture cap = threadData->input;
            cap.read(frame);
            if (frame.empty())  // End of video
                break;
            toGrayscale(frame, input);
            CV_Assert(input.type() == CV_8UC1);
            int nRows = input.rows;
            nCols = input.cols;
            edgesMaster.create(nRows - 2, nCols - 2, CV_8UC1);
            edgesThread.create(nRows - 2, nCols - 2, CV_8UC1);
            thisMatStartingIdx = 1;
            matSliceIdx = (int)nRows / 2;
            otherMatEndingIdx = nRows - 1;
            pthread_barrier_wait(&readySobel);
            sobel(thisMatStartingIdx, matSliceIdx, nCols, input, edgesMaster);
            pthread_barrier_wait(&completedSobel);

            for (int i = matSliceIdx; i < otherMatEndingIdx - 1; i++) {
                uchar *threadRow = edgesThread.ptr(i);
                uchar *masterRow = edgesMaster.ptr(i);
                for (int j = 1; j < nCols - 1; j++) 
                    masterRow[j] = threadRow[j];
            }

            imshow("Sobel Video", edgesMaster);
            pthread_mutex_unlock(&inputStreamAvailable);
            // End Critical Section
            waitKey(1);
        } else {
            pthread_barrier_wait(&readySobel);
            sobel(matSliceIdx, otherMatEndingIdx, nCols, input, edgesThread);
            pthread_barrier_wait(&completedSobel);
        }
    }

    for (int i = 0; i < threadData->numThreads; i++) {
        if (!pthread_equal(threads[i], threadData->thread))
            pthread_cancel(threads[i]);
    }
    pthread_exit(NULL);
}

int sobelVideo(VideoCapture &cap, int numThreads) {
    for (int i = 0; i < numThreads; i++) {
        struct sobelThreadData threadData = {.thread = threads[i],
                                             .threads = threads,
                                             .input = cap};
        if (int rc = pthread_create(&threads[i], NULL, sobel_threaded,
                (void *)&threadData)) {
            cout << "Could not create thread" << rc << endl;
            return 0;
        }
    }
    return 1;
}

int sobelInit(int numThreads) {
    pthread_barrier_init(&readySobel, NULL, numThreads);
    pthread_barrier_init(&completedSobel, NULL, numThreads);
    return 1;
}
