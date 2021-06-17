#include <full.h>
#include "opencv2/opencv.hpp"
#include "iostream"
#include <fcntl.h>
#include <unistd.h>

using namespace std;

#define BLUE 0
#define GREEN 30
#define RED 150
#define WIDTH 1000
#define HEIGHT 1000
#define H_LIMIT 30
#define L_LIMIT 30
#define NEIBOR_WIDTH 3
#define PORTNAME "/dev/ttyACM0"


/**
 * \fn cv::Mat gaussianFilter(cv::Mat image)
 * \brief Réalise la convolution d'une image par un filtre gaussien
 *
 * \param[in] image Image en niveau de gris à convoluer
 * \return Image après convolution
 */
cv::Mat gaussianFilter(cv::Mat image){

    int rows = image.rows;
    int cols = image.cols;

    int sum;

    int gauss_mask[3][3] = {{-1,-1,-1},{-1,8,-1},{-1,-1,-1}} ;
    int coeffMask = 0;

    cv::Mat result(rows, cols, CV_32F);

    for(int i = 1; i < rows-1 ; i++){
        for(int j = 1; j < cols-1 ; j++){

            sum = 0;
            sum = gauss_mask[0][0]*image.at<float>(i-1, j-1)
                + gauss_mask[0][1]*image.at<float>(i-1, j)
                + gauss_mask[0][2]*image.at<float>(i-1, j+1)
                + gauss_mask[1][0]*image.at<float>(i, j-1)
                + gauss_mask[1][1]*image.at<float>(i, j)
                + gauss_mask[1][2]*image.at<float>(i, j+1)
                + gauss_mask[2][0]*image.at<float>(i+1, j-1)
                + gauss_mask[2][1]*image.at<float>(i+1, j)
                + gauss_mask[2][2]*image.at<float>(i+1, j+1);

            result.at<float>(i, j) = sum;
        }
    }

    return result;

}

int main() {
    /*float maskTab[] = {-1, -1, -1,
                       -1,  8, -1,
                       -1, -1, -1};*/
    /*float maskTab[] = {0, -1, 0,
                       -1, 5, -1,
                       0, -1, 0};*/
//    Matrix<float> mask(3, 3, maskTab);

    short maskTab[] = {1, 2, 1,
                       2, 4, 2,
                       1, 2, 1};
    /*short maskTab[] = {0, 0, 0,
                       0, 1, 0,
                       0, 0, 0};*/
    Matrix<short> mask(3, 3, maskTab);

    cv::VideoCapture camera(0, cv::CAP_V4L2);
    camera.set(3, WIDTH);
    camera.set(4, HEIGHT);

	int fd = open (PORTNAME, O_RDWR | O_NOCTTY | O_SYNC);
    if (!camera.isOpened()) {
        std::cerr << "ERROR: Could not open camera" << std::endl;
        return 1;
    }

    cv::namedWindow("Webcam");
    cv::Mat frame, grey;

    while (1) {
        // Get camera image
        camera >> frame;

        // Convert the image to grayscale
        cvtColor(frame, grey, cv::COLOR_BGR2GRAY);


        // Convert the image to unsigned char
        grey.convertTo(grey, CV_8UC1);

        // Convert the image to an HiveEntropy matrix
        Matrix<uchar> heMatrix(grey.rows, grey.cols, grey.data);

        heMatrix = heMatrix.convolve(mask, EdgeHandling::Crop);

        cv::Mat output(heMatrix.getRows(), heMatrix.getColumns(), CV_8UC1, heMatrix.getData());

        // Display the image
        cv::imshow("Webcam", output);

        if (cv::waitKey(10) >= 0){

        	close(fd);
            break;
        }
    }


    return 0;
}
