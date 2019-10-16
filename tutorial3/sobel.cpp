#include "sobel.hpp"
#include <iostream>
#include <cmath>
#include <vector>
#include <pthread.h>
#include <semaphore.h>
#include <opencv2/core.hpp>
#include <opencv2/highgui.hpp>

#include "grayscale.hpp"

using namespace cv;
using namespace std;

const int vKernel[3][3] = {{-1, 0, 1},
                           {-2, 0, 2},
                           {-1, 0, 1}};

const int hKernel[3][3] = {{-1, -2, -1},
                           { 0,  0,  0},
                           { 1,  2,  1}};

struct sobelThreadData {
    pthread_t *thread;
    int threadNum;
    cv::VideoCapture *input;
};

// POSIX Thread Objects
static vector<pthread_t> processingThreads;
static pthread_t displayThread;
static vector<struct sobelThreadData> threadArgs;
static pthread_mutex_t inputStreamAvailable = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t readySobel, completedSobel;
static sem_t outputReady;
// Threaded shared globals
static int thisMatStartingIdx, matSliceIdx, otherMatEndingIdx;
static int nCols;
static Mat input, outputController, outputThread;

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

void *sobel_threaded(void *threadArgs) {
    struct sobelThreadData *threadData = (struct sobelThreadData *)threadArgs;
    while (true) {  // TODO: While not ^C
        if (pthread_mutex_trylock(&inputStreamAvailable)) {
            // Critical Section
            Mat frame;
            VideoCapture cap = *threadData->input;
            cap.read(frame);
            if (frame.empty())  // End of video
                break;
            toGrayscale(frame, input);
            CV_Assert(input.type() == CV_8UC1);
            int nRows = input.rows;
            nCols = input.cols;
            outputController.create(nRows - 2, nCols - 2, CV_8UC1);
            outputThread.create(nRows - 2, nCols - 2, CV_8UC1);
            thisMatStartingIdx = 1;
            matSliceIdx = (int)nRows / 2 - 10;
            otherMatEndingIdx = nRows - 1;
            pthread_barrier_wait(&readySobel);
            sobel(thisMatStartingIdx, matSliceIdx, nCols, input, outputController);
            pthread_barrier_wait(&completedSobel);
            for (int i = matSliceIdx; i < otherMatEndingIdx - 1; i++) {
                uchar *threadRow = outputThread.ptr(i);
                uchar *masterRow = outputController.ptr(i);
                for (int j = 1; j < nCols - 1; j++) 
                    masterRow[j] = threadRow[j];
            }
            sem_post(&outputReady);
            pthread_mutex_unlock(&inputStreamAvailable);
            // End Critical Section
        } else {
            pthread_barrier_wait(&readySobel);
            sobel(matSliceIdx, otherMatEndingIdx, nCols, input, outputThread);
            pthread_barrier_wait(&completedSobel);
        }
    }
    pthread_exit(NULL);
}

void sobelVideo(VideoCapture &cap, int numThreads) {
    processingThreads = vector<pthread_t> (numThreads);
    threadArgs = vector<struct sobelThreadData> (numThreads);
    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].thread = &processingThreads[i];
        threadArgs[i].threadNum = i;
        threadArgs[i].input = &cap;
        if (int rc = pthread_create(&processingThreads[i], NULL, sobel_threaded,
                (void *)&threadArgs[i])) {
            cout << "Could not create thread" << rc << endl;
        }
    }
}

void *sobelVideoDisplay(void *displayThreadArgs) {
    char *windowName = (char *)displayThreadArgs;
    int sem;
    while (true) {
        sem_getvalue(&outputReady, &sem);
        sem_wait(&outputReady);
        imshow(windowName, outputController);
        waitKey(1);
    }
}

void sobelInit(int numThreads, const char displayWindowName[]) {
    pthread_barrier_init(&readySobel, NULL, numThreads);
    pthread_barrier_init(&completedSobel, NULL, numThreads);
    if (displayWindowName) {
        sem_init(&outputReady, 0, 0);
        pthread_create(&displayThread, NULL, sobelVideoDisplay,
                (void *)displayWindowName);
    }
}

void sobelCleanup() {
    // Do something..
}
