#include "full.h"

#include <opencv2/opencv.hpp>
#include <iostream>
#include <memory>

#define WIDTH 200
#define HEIGHT 200

int main() {
    /*float maskTab[] = {-1, -1, -1,
                       -1,  8, -1,
                       -1, -1, -1};
    Matrix<float> mask(3, 3, maskTab);*/
    /*float maskTab[] = {0, -1, 0,
                       -1, 5, -1,
                       0, -1, 0};
    Matrix<float> mask(3, 3, maskTab);*/

    ushort maskTab[] = {0, 0, 0,
                       0, 1, 0,
                       0, 0, 0,};
    Matrix<ushort> mask(3, 3, maskTab);

    /*short maskTab[] = {01, 04, 06, 04, 01,
                       04, 16, 24, 16, 04,
                       06, 24, 36, 24, 06,
                       04, 16, 24, 16, 04,
                       01, 04, 06, 04, 01,};
    Matrix<unsigned short> mask(5, 5, maskTab);*/

    // Take the first camera as video input
    cv::VideoCapture camera(0, cv::CAP_V4L2);
    // Set input size as close as possible to given width and height
    camera.set(3, WIDTH);
    camera.set(4, HEIGHT);

    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    // Create the windows that will render the images
    cv::namedWindow("Convolution");
    cv::namedWindow("Base image");
    cv::Mat frame, grey;

    std::shared_ptr<HiveEntropyNode> n = std::make_shared<HiveEntropyNode>("192.168.1.38:6969");
    Distributor<unsigned short> dist(n);
    n->sendAskingHardwareSpecification("192.168.1.38:9999");
    n->resolveNodeIdentities();

    while (true) {
        // Get camera image
        camera >> frame;

        // Convert the image to grayscale
        cvtColor(frame, grey, cv::COLOR_BGR2GRAY);

        // Convert the image to unsigned char
        grey.convertTo(grey, CV_8UC1);

        // Convert the image to an HiveEntropy matrix
        Matrix<ushort> heMatrix(grey.rows, grey.cols, grey.data);

        /*Matrix<uchar> castMatrix = Matrix<uchar>(heMatrix);
        cv::Mat output(castMatrix.getRows(), castMatrix.getColumns(), CV_8UC1, castMatrix.getData());*/

        // Send the matrices to be convolved
        std::string uid = dist.distributeMatrixConvolution(heMatrix, mask);
        // Wait for the result
        Matrix<ushort> convolved = dist.get(uid);

        // Convert the convolved matrix back to uchar
        Matrix<uchar> castMatrix = Matrix<uchar>(convolved);
        cv::Mat output(convolved.getRows(), convolved.getColumns(), CV_8UC1, castMatrix.getData());

        // Display the image
        cv::imshow("Base image", grey);
        cv::imshow("Convolution", output);

        if (cv::waitKey(50) >= 0){
            break;
        }
    }

    return 0;
}
