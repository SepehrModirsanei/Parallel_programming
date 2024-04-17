// g++ -mavx -msse Q4_1.cpp -o q4_1.out -lopencv_core -lopencv_imgcodecs -lopencv_highgui

#include <iostream>
#include <opencv2/opencv.hpp>
#include <chrono>
#include <emmintrin.h>
#include <tmmintrin.h>


using namespace std;
using namespace cv;


cv::Mat addImagesParallelWithSaturation(const cv::Mat& image1, const cv::Mat& image2, double alpha) {

    cv::Mat result = cv::Mat(image1.rows, image1.cols, image1.type());

    #pragma omp parallel for num_threads(32) collapse(2) schedule(static)

     for (int i = 0; i < image1.rows; ++i) {
        for (int j = 0; j < image1.cols; ++j) {
                result.at<cv::Vec3b>(i, j)[0] = image1.at<cv::Vec3b>(i, j)[0];
                result.at<cv::Vec3b>(i, j)[1] = image1.at<cv::Vec3b>(i, j)[1];
                result.at<cv::Vec3b>(i, j)[2] = image1.at<cv::Vec3b>(i, j)[2];
            
        }
    }

    #pragma omp parallel for num_threads(32)  schedule(static)
    for (int i = 0; i < image2.rows; ++i) {
        for (int j = 0; j < image2.cols; ++j) {
            
                uchar value10 = image1.at<cv::Vec3b>(i, j)[0];
                uchar value20 = image2.at<cv::Vec3b>(i, j)[0];

                // Apply saturation
                int result_value0 = cv::saturate_cast<int>((1-alpha)*value10 + alpha * value20);
                result.at<cv::Vec3b>(i, j)[0] = static_cast<uchar>(result_value0);


                uchar value11 = image1.at<cv::Vec3b>(i, j)[1];
                uchar value21 = image2.at<cv::Vec3b>(i, j)[1];

                // Apply saturation
                int result_value1 = cv::saturate_cast<int>((1-alpha)*value11 + alpha * value21);
                result.at<cv::Vec3b>(i, j)[1] = static_cast<uchar>(result_value1);


                uchar value12= image1.at<cv::Vec3b>(i, j)[2];
                uchar value22 = image2.at<cv::Vec3b>(i, j)[2];

                // Apply saturation
                int result_value2 = cv::saturate_cast<int>((1-alpha)*value12 + alpha * value22);
                result.at<cv::Vec3b>(i, j)[2] = static_cast<uchar>(result_value2);
            
        }
    }

    return result;
}


void addImagesSerial(const cv::Mat& image1, const cv::Mat& image2, double alpha, cv::Mat& result) {
    // CV_Assert(image1.size() == image2.size());

    //assume that the smaller image is image2
    result = cv::Mat(image1.size(), image1.type());

    for (int i = 0; i < image1.rows; ++i) {
        for (int j = 0; j < image1.cols; ++j) {
            for (int k = 0; k < image1.channels(); ++k) {
                if(i > image2.rows || j > image2.cols){
                    result.at<cv::Vec3b>(i, j)[k] = cv::saturate_cast<uchar>(image1.at<cv::Vec3b>(i, j)[k]);
                }else {
                    result.at<cv::Vec3b>(i, j)[k] = cv::saturate_cast<uchar>(image1.at<cv::Vec3b>(i, j)[k] + alpha * image2.at<cv::Vec3b>(i, j)[k]);
                }
            }
        }
    }
    // imshow("Result Image", result);
}

int main() {
    // Load two sample images
    cv::Mat image1 = cv::imread("/home/joker/Desktop/CA2_PP/Q4/Image1.png", cv::IMREAD_UNCHANGED);
    cv::Mat image2 = cv::imread("/home/joker/Desktop/CA2_PP/Q4/Image2.png", cv::IMREAD_UNCHANGED);
    cv::Mat result_parallel, result_serial;

    double alpha = 0.5; // Change alpha as needed

    // Parallel computation
    auto start_parallel = std::chrono::high_resolution_clock::now();
    result_parallel = addImagesParallelWithSaturation(image1, image2, alpha);
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

    // imshow("Result Image", result_serial);
    // waitKey(0);
    // destroyAllWindows();

    return 0;
}
