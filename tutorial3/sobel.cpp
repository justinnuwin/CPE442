#include "sobel.hpp"
#include <iostream>
#include <cmath>
#include <vector>
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
    vector<pthread_t> *threads;
    cv::VideoCapture *input;
    int numThreads;
    int threadNum;
};

// POSIX Thread Objects
static vector<pthread_t> threads;
static vector<struct sobelThreadData> threadArgs;
static pthread_mutex_t inputStreamAvailable = PTHREAD_MUTEX_INITIALIZER;
static pthread_barrier_t readySobel, completedSobel;
// Threaded shared globals
static int thisMatStartingIdx, matSliceIdx, otherMatEndingIdx;
static int nCols;
static Mat input, edgesMaster, edgesThread;

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
    int threadNum = threadData->threadNum;
    while (true) {  // TODO: While not ^C
        cout << threadNum << "Thread: Trying to lock mutex" << endl;
        if (pthread_mutex_trylock(&inputStreamAvailable)) {
            cout << threadNum << "Controller: Locked mutex" << endl;
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
            edgesMaster.create(nRows - 2, nCols - 2, CV_8UC1);
            edgesThread.create(nRows - 2, nCols - 2, CV_8UC1);
            thisMatStartingIdx = 1;
            matSliceIdx = (int)nRows / 2 - 10;
            otherMatEndingIdx = nRows - 1;
            cout << threadNum << "Controller: Waiting for child" << endl;
            pthread_barrier_wait(&readySobel);
            cout << threadNum << "Controller: Starting sobel" << endl;
            sobel(thisMatStartingIdx, matSliceIdx, nCols, input, edgesMaster);
            cout << threadNum << "Controller: Finished sobel" << endl;
            pthread_barrier_wait(&completedSobel);
            cout << threadNum << "Controller: Child finished" << endl;

            cout << threadNum << "Controller: Stitching photo" << endl;
            for (int i = matSliceIdx; i < otherMatEndingIdx - 1; i++) {
                uchar *threadRow = edgesThread.ptr(i);
                uchar *masterRow = edgesMaster.ptr(i);
                for (int j = 1; j < nCols - 1; j++) 
                    masterRow[j] = threadRow[j];
            }
            cout << threadNum << "Controller: Finished stitching" << endl;

            cout << threadNum << "Controller: Showing image" << endl;
            //imshow("Sobel Video", edgesMaster);
            cout << threadNum << "Controller: Image shown" << endl;
            cout << threadNum << "Controller: Unlocking mutex" << endl;
            pthread_mutex_unlock(&inputStreamAvailable);
            cout << threadNum << "Thread: Mutex unlocked" << endl;
            // End Critical Section
            //waitKey(50);
        } else {
            cout << threadNum << "\tChild: Mutex was locked :(" << endl;
            cout << threadNum << "\tChild: Waiting for controller" << endl;
            pthread_barrier_wait(&readySobel);
            cout << threadNum << "\tChild: Starting sobel" << endl;
            sobel(matSliceIdx, otherMatEndingIdx, nCols, input, edgesThread);
            cout << threadNum << "\tChild: Finished sobel" << endl;
            pthread_barrier_wait(&completedSobel);
            cout << threadNum << "\tChild: Controller finished" << endl;
        }
    }

    for (int i = 0; i < threadData->numThreads; i++) {
        if (!pthread_equal(threads[i], *threadData->thread))
            pthread_cancel(threads[i]);
    }
    pthread_exit(NULL);
}

void sobelVideo(VideoCapture &cap, int numThreads) {
    threads = vector<pthread_t> (numThreads);
    threadArgs = vector<struct sobelThreadData> (numThreads);
    for (int i = 0; i < numThreads; i++) {
        threadArgs[i].thread = &threads[i];
        threadArgs[i].threads = &threads;
        threadArgs[i].input = &cap;
        threadArgs[i].numThreads = numThreads;
        threadArgs[i].threadNum = i;
        if (int rc = pthread_create(&threads[i], NULL, sobel_threaded,
                (void *)&threadArgs[i])) {
            cout << "Could not create thread" << rc << endl;
        }
    }
}

void sobelInit(int numThreads) {
    pthread_barrier_init(&readySobel, NULL, numThreads);
    pthread_barrier_init(&completedSobel, NULL, numThreads);
}

void sobelCleanup() {
    // Do something..
}
