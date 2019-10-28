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
    int threadNum;
    cv::VideoCapture *input;
};

// POSIX Thread Objects
static vector<pthread_t> processingThreads;
static pthread_t displayThread;
static vector<struct sobelThreadData> threadArgs;
static pthread_mutex_t inputStreamAvailable = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t readyFrame, completedSobel;
static sem_t outputReady;
// Threaded shared globals
static int thisMatStartingIdx, matSliceIdx, otherMatEndingIdx;
static Mat frame, input, outputController, outputThread;

inline void
sobel(int matStartingIdx, int matEndingIdx, Mat &input, Mat &output) {
    int nCols = output.cols;
    int i, j;   // Loop variables
    int vGradient, hGradient, sum;
    for(i = matStartingIdx; i < matEndingIdx - 1; i++) {
#pragma omp simd
        for (j = 1; j < nCols - 1; j++) {   // Compiler will optimize mult const 2
            vGradient = input.ptr(i-1)[j - 1] * vKernel[0][0] +
                            // input.ptr(i-1)[j    ] * vKernel[0][1] +
                            input.ptr(i-1)[j + 1] * vKernel[0][2] +
                            input.ptr(i)[j - 1] * vKernel[1][0] +
                            // input.ptr(i)[j    ] * vKernel[1][1] +
                            input.ptr(i)[j + 1] * vKernel[1][2] +
                            input.ptr(i+1)[j - 1] * vKernel[2][0] +
                            // input.ptr(i+1)[j    ] * vKernel[2][1] +
                            input.ptr(i+1)[j + 1] * vKernel[2][2];
            hGradient = input.ptr(i-1)[j - 1] * hKernel[0][0] +
                            input.ptr(i-1)[j    ] * hKernel[0][1] +
                            input.ptr(i-1)[j + 1] * hKernel[0][2] +
                            // input.ptr(i)[j - 1] * hKernel[1][0] +
                            // input.ptr(i)[j    ] * hKernel[1][1] +
                            // input.ptr(i)[j + 1] * hKernel[1][2] +
                            input.ptr(i+1)[j - 1] * hKernel[2][0] +
                            input.ptr(i+1)[j    ] * hKernel[2][1] +
                            input.ptr(i+1)[j + 1] * hKernel[2][2];
            // Approx. true sobel magnitude sqrt(g_x^2 + g_y^2)
            // int sum = round(sqrt(pow(vGradient, 2) + pow(hGradient, 2)));
            sum = abs(vGradient) + abs(hGradient);
            output.ptr(i-1)[j - 1] = (uchar)(sum > 255 ? 255 : sum);
        }
    }
}

void *sobel_threaded(void *threadArgs) {
    struct sobelThreadData *threadData = (struct sobelThreadData *)threadArgs;
    int threadNum = threadData->threadNum;
    pthread_t thisThread = processingThreads[threadNum];
    while (true) {  // TODO: Break when ^C caught
        if (pthread_mutex_trylock(&inputStreamAvailable)) {
            // Critical Section
            VideoCapture cap = *threadData->input;
            cap.read(frame);
            if (frame.empty())  // End of video
                break;
            CV_Assert(frame.type() == CV_8UC3);
            int nRows = frame.rows;
            int nCols = frame.cols;
            input.create(nRows, nCols, CV_8UC1);
            outputController.create(nRows - 2, nCols - 2, CV_8UC1);
            outputThread.create(nRows - 2, nCols - 2, CV_8UC1);
            thisMatStartingIdx = 1;
            matSliceIdx = (int)nRows / 2;   // TODO: Add support for more than 2 threads
            otherMatEndingIdx = nRows - 1;
            pthread_barrier_wait(&readyFrame);
            toGrayscale_threaded(thisMatStartingIdx, matSliceIdx, frame, input);
            sobel(thisMatStartingIdx, matSliceIdx, input, outputController);
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
            pthread_barrier_wait(&readyFrame);
            toGrayscale_threaded(matSliceIdx, otherMatEndingIdx, frame, input);
            sobel(matSliceIdx, otherMatEndingIdx, input, outputThread);
            pthread_barrier_wait(&completedSobel);
        }
    }
    for (int i = 0; i < (int)processingThreads.size(); i++) {
        if (!pthread_equal(thisThread, processingThreads[i])) {
            cout << "Cancelling thread " << i << endl;
            pthread_cancel(processingThreads[i]);
        }
    }
    cout << "Thread exiting " << threadNum << endl;
    pthread_exit(NULL);
}

void sobelVideo(VideoCapture &cap, int numThreads) {
    processingThreads = vector<pthread_t> (numThreads);
    threadArgs = vector<struct sobelThreadData> (numThreads);
    for (int i = 0; i < numThreads; i++) {
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
    while (true) {
        sem_wait(&outputReady);
        imshow(windowName, outputController);
        waitKey(1);
    }
}

void sobelInit(int numThreads, const char displayWindowName[]) {
    pthread_barrier_init(&readyFrame, NULL, numThreads);
    pthread_barrier_init(&completedSobel, NULL, numThreads);
    if (displayWindowName) {
        sem_init(&outputReady, 0, 0);
        pthread_create(&displayThread, NULL, sobelVideoDisplay,
                (void *)displayWindowName);
    }
}

// This is a blocking function call
void sobelCleanup() {
    for (int i = 0; i < (int)processingThreads.size(); i++) {
        pthread_join(processingThreads[i], NULL);
        cout << "Joined thread " << i << endl;
        break;
    }
    pthread_cancel(displayThread);
}
