#include <full.h>
#include <opencv2/opencv.hpp>
#include "iostream"
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define WIDTH 200
#define HEIGHT 200

int main() {
    /*float maskTab[] = {-1, -1, -1,
                       -1,  8, -1,
                       -1, -1, -1};*/
    /*float maskTab[] = {0, -1, 0,
                       -1, 5, -1,
                       0, -1, 0};
    Matrix<float> mask(3, 3, maskTab);*/

    /*float maskTab[] = {0, 0, 0,
                       0, 1, 0,
                       0, 0, 0,};
    Matrix<float> mask(3, 3, maskTab);*/

    short maskTab[] = {01, 04, 06, 04, 01,
                       04, 16, 24, 16, 04,
                       06, 24, 36, 24, 06,
                       04, 16, 24, 16, 04,
                       01, 04, 06, 04, 01,};
    Matrix<unsigned short> mask(5, 5, maskTab);

    cv::VideoCapture camera(0, cv::CAP_V4L2);
    camera.set(3, WIDTH);
    camera.set(4, HEIGHT);

    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    cv::namedWindow("Convolution");
    cv::namedWindow("Base image");
    cv::Mat frame, grey;

    HiveEntropyNode n("192.168.1.38:6969");
    Distributor<unsigned short> dist(&n);
    n.sendAskingHardwareSpecification("192.168.1.14:9999");
    n.resolveNodeIdentities();

    while (1) {
        // Get camera image
        camera >> frame;

        // Convert the image to grayscale
        cvtColor(frame, grey, cv::COLOR_BGR2GRAY);

        // Convert the image to unsigned char
        grey.convertTo(grey, CV_8UC1);

        // Convert the image to an HiveEntropy matrix
        Matrix<unsigned short> heMatrix(grey.rows, grey.cols, grey.data);

        // Send the matrices to be convolved
        std::string uid = dist.distributeMatrixConvolution(heMatrix, mask);
        // Wait for the result
        Matrix<unsigned short> convolved = dist.get(uid);

        // Convert the convolved matrix back to uchar
        cv::Mat output(convolved.getRows(), convolved.getColumns(), CV_8UC1, Matrix<uchar>(convolved).getData());

        // Display the image
        cv::imshow("Base image", grey);
        cv::imshow("Convolution", output);

        if (cv::waitKey(10) >= 0){
            break;
        }
    }


    return 0;
}
