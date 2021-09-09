#include <iostream>
#include "ssd_match.h"
#include <pcl/point_cloud.h>
#include <pcl/point_types.h>
#include <pcl/io/io.h>
#include <pcl/io/ply_io.h>

int main() {
    const int window_size = 6;
    const int max_disparity = 50;
    const int tranwin_size = 7;
    const Stereo::cost_function cost = Stereo::cost_function::census;
    const std::string outname = "result.jpg";
    cv::Mat dis;
    cv::Mat left = cv::imread("im1.png", 0); //read images into grayscale
    cv::Mat right = cv::imread("im2.png", 0);

    Stereo s(window_size, max_disparity, tranwin_size, cost);
    dis = s.stereo_match(left, right);
    cv::imwrite(outname, dis);
    pcl::PointCloud<pcl::PointXYZ>::Ptr cloud (new pcl::PointCloud<pcl::PointXYZ>);
    for(int i = 0; i < dis.rows; i++) {
        for (int j = 0; j < dis.cols; j++) {
            int pt = dis.at<uchar>(i, j);
            pcl::PointXYZ point = pcl::PointXYZ(i, j, pt);
            cloud->push_back(point);
        }
    }
    pcl::io::savePLYFileASCII("test.ply",*cloud);
    return 0;
}