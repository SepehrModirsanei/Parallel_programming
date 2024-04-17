#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <pthread.h>

#define NUM_THREADS 4

struct ThreadData {
    const cv::Mat* image1;
    const cv::Mat* image2;
    cv::Mat* result;
    double alpha;
    int startRow;
    int endRow;
};

void* addImagesParallelWithSaturationThread(void* arg) {
    ThreadData* data = static_cast<ThreadData*>(arg);

    for (int i = data->startRow; i < data->endRow; ++i) {
        for (int j = 0; j < data->image1->cols; ++j) {
            if (j < data->image2->cols && i<data->image2->rows) {
                for (int k = 0; k < data->image1->channels(); ++k) {
                    uchar value1 = data->image1->at<cv::Vec3b>(i, j)[k];
                    uchar value2 = data->image2->at<cv::Vec3b>(i, j)[k];

                    // Apply saturation
                    int result_value = cv::saturate_cast<int>((1 - data->alpha) * value1 + data->alpha * value2);
                    data->result->at<cv::Vec3b>(i, j)[k] = static_cast<uchar>(result_value);
                }
            } else {
                // If j is beyond the columns of image2, copy pixel from image1 to result
                data->result->at<cv::Vec3b>(i, j) = data->image1->at<cv::Vec3b>(i, j);
            }
        }
    }

    return nullptr;
}

cv::Mat addImagesParallelWithSaturation(const cv::Mat& image1, const cv::Mat& image2, double alpha) {
    cv::Mat result = cv::Mat(image1.rows, image1.cols, image1.type());

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        int startRow = i * (image1.rows / NUM_THREADS);
        int endRow = (i + 1) * (image1.rows / NUM_THREADS);

        threadData[i] = {&image1, &image2, &result, alpha, startRow, endRow};

        pthread_create(&threads[i], nullptr, addImagesParallelWithSaturationThread, &threadData[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return result;
}

cv::Mat addImagesParallelWithSaturation2(const cv::Mat& image1, const cv::Mat& image2, double alpha) {
    cv::Mat result = cv::Mat(image1.rows, image1.cols, image1.type());

    pthread_t threads[NUM_THREADS];
    ThreadData threadData[NUM_THREADS];

    for (int i = 0; i < NUM_THREADS; ++i) {
        int startRow = i * (image1.rows / NUM_THREADS);
        int endRow = (i + 1) * (image1.rows / NUM_THREADS);

        threadData[i] = {&image1, &image2, &result, alpha, startRow, endRow};

        pthread_create(&threads[i], nullptr, addImagesParallelWithSaturationThread, &threadData[i]);
    }

    for (int i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], nullptr);
    }

    return result;
}

void addImagesSerial(const cv::Mat& image1, const cv::Mat& image2, double alpha, cv::Mat& result) {
    result = cv::Mat(image1.size(), image1.type());

    for (int i = 0; i < image1.rows; ++i) {
        for (int j = 0; j < image1.cols; ++j) {
            for (int k = 0; k < image1.channels(); ++k) {
                uchar value1 = image1.at<cv::Vec3b>(i, j)[k];
                uchar value2 = image2.at<cv::Vec3b>(i, j)[k];

                // Apply saturation
                int result_value = cv::saturate_cast<int>((1 - alpha) * value1 + alpha * value2);
                result.at<cv::Vec3b>(i, j)[k] = static_cast<uchar>(result_value);
            }
        }
    }
}

int main() {
    // Load two sample images
    cv::Mat image1 = cv::imread("/home/joker/Desktop/PP_CA4/Q4/Image1.png", cv::IMREAD_UNCHANGED);
    cv::Mat image2 = cv::imread("/home/joker/Desktop/PP_CA4/Q4/Image2.png", cv::IMREAD_UNCHANGED);
    cv::Mat result_parallel, result_serial;
    int height = image1.rows;
    int width = image1.cols;
    cv::Mat blackImage = cv::Mat::zeros(height, width, CV_8UC3);  // Assuming 3 channels (e.g., BGR)


    double alpha = 0.5; // Change alpha as needed

    // Parallel computation
    auto start_parallel = std::chrono::high_resolution_clock::now();
    result_parallel = addImagesParallelWithSaturation(image1, image2, alpha);
    // result_parallel = addImagesParallelWithSaturation(image2, image1, alpha);

    auto end_parallel = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_parallel = end_parallel - start_parallel;
    double parallel_time = elapsed_parallel.count();

    // Serial computation
    auto start_serial = std::chrono::high_resolution_clock::now();
    addImagesSerial(image1, image2, alpha, result_serial);
    auto end_serial = std::chrono::high_resolution_clock::now();
    std::chrono::duration<double> elapsed_serial = end_serial - start_serial;
    double serial_time = elapsed_serial.count();

    double speedup = serial_time / parallel_time;

    std::cout << "Time taken for parallel computation: " << parallel_time << " seconds" << std::endl;
    std::cout << "Time taken for serial computation: " << serial_time << " seconds" << std::endl;
    std::cout << "Speedup: " << speedup << std::endl;

    // Display the result images
    cv::imshow("Result Parallel", result_parallel);
    cv::waitKey(0);

    // cv::imshow("Result Serial", result_serial);
    // cv::waitKey(0);

    return 0;
}
