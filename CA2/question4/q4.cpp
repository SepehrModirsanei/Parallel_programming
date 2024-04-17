#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <emmintrin.h>

void addWeightedParallel(const cv::Mat& frameA, const cv::Mat& frameB, double alpha, cv::Mat& result) {
    CV_Assert(frameA.size() == frameB.size());

    int size = frameA.rows * frameA.cols * frameA.channels();
    result = cv::Mat(frameA.size(), frameA.type());

    for (int i = 0; i < size; i += 16) {
        __m128i a = _mm_loadu_si128(reinterpret_cast<const __m128i*>(frameA.data + i));
        __m128i b = _mm_loadu_si128(reinterpret_cast<const __m128i*>(frameB.data + i));
        __m128i weighted_b = _mm_cvtps_epi32(_mm_mul_ps(_mm_cvtepi32_ps(b), _mm_set1_ps(alpha)));
        __m128i sum = _mm_adds_epu8(a, weighted_b);

        _mm_storeu_si128(reinterpret_cast<__m128i*>(result.data + i), sum);
    }
}

void addWeightedSerial(const cv::Mat& frameA, const cv::Mat& frameB, double alpha, cv::Mat& result) {
    CV_Assert(frameA.size() == frameB.size());

    cv::addWeighted(frameA, 1 - alpha, frameB, alpha, 0, result);
}

int main() {
    // Load two sample images
    cv::Mat img_a = cv::imread("path_to_image_a.jpg");
    cv::Mat img_b = cv::imread("path_to_image_b.jpg");
    cv::Mat result_parallel, result_serial;

    double alpha = 0.5; // Change alpha as needed

    // Parallel computation
    auto start_parallel = std::chrono::high_resolution_clock::now();
    addWeightedParallel(img_a, img_b, alpha, result_parallel);
    auto end_parallel = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_parallel = end_parallel - start_parallel;
    double parallel_time = elapsed_parallel.count();

    // Serial computation
    auto start_serial = std::chrono::high_resolution_clock::now();
    addWeightedSerial(img_a, img_b, alpha, result_serial);
    auto end_serial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_serial = end_serial - start_serial;
    double serial_time = elapsed_serial.count();

    double speedup = serial_time / parallel_time;

    std::cout << "Time taken for parallel computation: " << parallel_time << " seconds" << std::endl;
    std::cout << "Time taken for serial computation: " << serial_time << " seconds" << std::endl;
    std::cout << "Speedup: " << speedup << std::endl;

    return 0;
}
