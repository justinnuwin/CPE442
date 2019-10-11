#ifndef _SOBEL_H_
#define _SOBEL_H_

#include <pthread.h>
#include <opencv2/core.hpp>
#include <opencv2/videoio.hpp>

int sobelVideo(cv::VideoCapture &cap, int numThreads);
int sobelInit(int numThreads);

class Sobel {
    public:
        Sobel(cv::VideoCapture cap, int numThreads);
        ~Sobel();
    private:
        pthread_t threads[2];
        pthread_mutex_t inputStreamAvailable = PTHREAD_MUTEX_INITIALIZER;
        pthread_barrier_t readySobel, completedSobel;
        int thisMatStartingIdx, matSliceIdx, otherMatEndingIdx;
        int nCols;
        cv::Mat input, edgesMaster, edgesThread;
};


#endif
