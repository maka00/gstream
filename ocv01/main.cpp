#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <iostream>

int main() {
    cv::VideoCapture cam;
    cam.open(0);
    if (!cam.isOpened()) {
        std::cout << "error opening cam ";
    }
    cv::Mat img;
    if (cam.read(img)) {
       cv::imshow("output", img);
    }
    cv::waitKey(0);
    cam.release();
    /*
    cv::Mat img = cv::imread("pipline.png", cv::IMREAD_COLOR);
    cv::imshow("Output", img);
    cv::waitKey(0);
     */
    return 0;
}