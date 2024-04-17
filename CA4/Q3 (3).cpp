#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <pthread.h>
#define NUM_THREADS 4

struct ThreadData {
    const cv::Mat* frameA;
    const cv::Mat* frameB;
    cv::Mat* result;
    int startRow;
    int endRow;
};

void* calculateAbsoluteDifferenceThread(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);
    const cv::Mat& frameA = *(data->frameA);
    const cv::Mat& frameB = *(data->frameB);
    cv::Mat& result = *(data->result);

    for (int i = data->startRow; i < data->endRow; ++i) {
        for (int j = 0; j < frameA.cols; ++j) {
            result.at<uchar>(i, j) = cv::saturate_cast<uchar>(frameA.at<uchar>(i, j) - frameB.at<uchar>(i, j));
        }
    }

    return nullptr;
}

void calculateAbsoluteDifferenceParallel(const cv::Mat& frameA, const cv::Mat& frameB, cv::Mat& result) {
    int rows = frameA.rows;
    int cols = frameA.cols;
    result = cv::Mat(frameA.size(), frameA.type());

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        int startRow = i * (rows / NUM_THREADS);
        int endRow = (i + 1) * (rows / NUM_THREADS);

        threadData[i] = {&frameA, &frameB, &result, startRow, endRow};

        pthread_create(&threads[i], nullptr, calculateAbsoluteDifferenceThread, &threadData[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }
}

void calculateAbsoluteDifferenceSerial(const cv::Mat& frameA, const cv::Mat& frameB, cv::Mat& result) {
    int rows = frameA.rows;
    int cols = frameA.cols;
    result = cv::Mat(frameA.size(), frameA.type());

    for (int i = 0; i < rows; ++i) {
        for (int j = 0; j < cols; ++j) {
            result.at<uchar>(i, j) = cv::saturate_cast<uchar>(frameA.at<uchar>(i, j) - frameB.at<uchar>(i, j));
        }
    }
}

int main() {
    // Define the size of the images
    int width = 300;
    int height = 200;

    // Create the first sample image
    cv::Mat img1 = cv::imread("/home/joker/Desktop/PP_CA4/Q3/Image1.png", cv::IMREAD_GRAYSCALE);
    cv::Mat img2 = cv::imread("/home/joker/Desktop/PP_CA4/Q3/Image1.png", cv::IMREAD_GRAYSCALE);

    cv::Mat result, result_serial;

    // Ensure images are loaded before timing
    if (img1.empty() || img2.empty()) {
        std::cerr << "Error loading images." << std::endl;
        return 1;
    }

    // Warm-up to reduce timing variability
    calculateAbsoluteDifferenceSerial(img1, img2, result_serial);

    // Parallel computation
    auto start_parallel = std::chrono::high_resolution_clock::now();
    calculateAbsoluteDifferenceParallel(img1, img2, result);
    auto end_parallel = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_parallel = end_parallel - start_parallel;
    double parallel_time = elapsed_parallel.count();

    // Serial computation
    auto start_serial = std::chrono::high_resolution_clock::now();
    calculateAbsoluteDifferenceSerial(img1, img2, result_serial);
    auto end_serial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_serial = end_serial - start_serial;
    double serial_time = elapsed_serial.count();

    double speedup = serial_time / parallel_time;

    cv::Mat diff;
    cv::absdiff(result, result_serial, diff);

    int nz = cv::countNonZero(diff);
    if (nz == 0) {
        std::cout << "Results of absolute difference computation are exactly the same." << std::endl;
    } else {
        std::cout << "Results of absolute difference computation are different." << std::endl;
    }

    std::cout << "Time taken for parallel computation: " << parallel_time << " seconds" << std::endl;
    std::cout << "Time taken for serial computation: " << serial_time << " seconds" << std::endl;
    std::cout << "Speedup: " << speedup << std::endl;

    return 0;
}
