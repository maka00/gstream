#include <iostream>
#include "ssd_match.h"

int main() {
    const int window_size = 6;
    const int max_disparity = 50;
    const int tranwin_size = 7;
    const std::string cost = "census";
    const std::string outname = "result.jpg";
    cv::Mat dis;
    cv::Mat left = cv::imread("im1.png", 0); //read images into grayscale
    cv::Mat right = cv::imread("im2.png", 0);
    Stereo s(window_size, max_disparity, tranwin_size, cost);
    dis = s.stereo_match(left, right);
    cv::imwrite(outname, dis);
    return 0;
}