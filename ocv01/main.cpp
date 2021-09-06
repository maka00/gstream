#include <opencv4/opencv2/core.hpp>
#include <opencv4/opencv2/highgui.hpp>
#include <opencv4/opencv2/videoio.hpp>
#include <opencv4/opencv2/imgproc.hpp>
#include <fmt/format.h>
#include <iostream>


int get_video() {
    cv::VideoCapture cam;
    cam.open("../../data/test.sdp");
    if (!cam.isOpened()) {
        std::cout << "error opening stream" << std::endl;
        return 1;
    }
    cv::Mat img;
    int i = 0;
    int k = 0;
    while (k != 23)
    if (cam.read(img)) {
        // for storing
        const std::string fname = fmt::format("test_{}.jpeg",i++);

        // perform an edge detection
        cv::Mat edges;
        cv::Canny(img, edges, 100,300,3,false);
        cv::Mat flipped;
        cv::flip(edges, flipped,1);
        cv::imshow("output", flipped);

        // write some stats
        std::vector<std::vector<cv::Point>> contours;
        std::vector<cv::Vec4i> hierarchy;
        cv::findContours(flipped, contours,  hierarchy, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);
        std::cout << fmt::format("Contours {}\n", contours.size());

        // wait for 1ms for an escape key and continue
        k = cv::waitKey(1) & 0XFF;
        if (k == 27) break;
    }

    cam.release();
    return 0;
}

int get_single() {
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
    return 0;
}

int main() {
    // set the environment variable for FFMPEG
    setenv("OPENCV_FFMPEG_CAPTURE_OPTIONS", "protocol_whitelist;file,rtp,udp", 1);
    get_video();
    return 0;
}