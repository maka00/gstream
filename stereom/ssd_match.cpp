#include "ssd_match.h"

static inline int HammingDistance(int a, int b) { return static_cast<int>( __builtin_popcount(a ^ b)); }

cv::Mat Stereo::rank_transform(cv::Mat image, int windowsize) {
    int h = image.rows;
    int w = image.cols;
    cv::Mat imgDisparity8U = cv::Mat(image.rows, image.cols, CV_8U);
    int window_half = windowsize / 2;

    for (int y = window_half; y < h - window_half; ++y) {
        for (int x = window_half; x < w - window_half; ++x) {
            int ssd = 0;

            for (int v = -window_half; v < window_half + 1; ++v) {
                for (int u = -window_half; u < window_half + 1; ++u) {

                    if (image.at<uchar>(y + v, x + u) > image.at<uchar>(y, x)) ++ssd;
                }

            }

            imgDisparity8U.at<uchar>(y, x) = ssd;

        }
    }
    return imgDisparity8U;
}

cv::Mat Stereo::census_transform(cv::Mat image, int windowsize) {
    int h = image.rows;
    int w = image.cols;
    cv::Mat imgDisparity8U = cv::Mat(image.rows, image.cols, CV_8U);
    int window_half = windowsize / 2;

    for (int y = window_half; y < h - window_half; ++y) {
        for (int x = window_half; x < w - window_half; ++x) {
            int ssd = 0;

            for (int v = -window_half; v < window_half + 1; ++v) {
                for (int u = -window_half; u < window_half + 1; ++u) {
                    if (v != 0 && u != 0) { // skip the central pixel
                        ssd <<= 1;
                        if (image.at<uchar>(y + v, x + u) > image.at<uchar>(y, x))
                            ssd = ssd +
                                  1; // assign last digit to 1 if pixel is larger than central pixel in the windows else assign 0
                    }
                }

            }

            imgDisparity8U.at<uchar>(y, x) = ssd;

        }
    }
    return imgDisparity8U;
}

cv::Mat Stereo::stereo_match(cv::Mat left, cv::Mat right) {
    int h = left.rows;
    int w = left.cols;
    cv::Mat imgDisparity8U = cv::Mat(left.rows, left.cols, CV_8U);
    int window_half = win_size_ / 2;
    int adjust = 255 / max_disparity_;
    //decide which matching cost function to use
    if (cost_ == "rank") {
        left = Stereo::rank_transform(left, tran_win_size_);
        right = Stereo::rank_transform(right, tran_win_size_);
    } else if (cost_ == "census") {
        left = Stereo::census_transform(left, tran_win_size_);
        right = Stereo::census_transform(right, tran_win_size_);
    }
    for (int y = window_half; y < h - window_half; ++y) {
        uchar *imgDisparity_y = imgDisparity8U.ptr(y);
        for (int x = window_half; x < w - window_half; ++x) {
            int prev_ssd = INT_MAX;
            int best_dis = 0;
            for (int off = 0; off < max_disparity_; ++off) {
                int ssd = 0;
                int ssd_tmp = 0;
                for (int v = -window_half; v < window_half; ++v) {
                    for (int u = -window_half; u < window_half; ++u) {
                        if (cost_ == "census") {
                            ssd_tmp = HammingDistance(left.at<uchar>(y + v, x + u),
                                                      right.at<uchar>(y + v, std::abs(x + u - off)));
                        } else {
                            ssd_tmp = left.at<uchar>(y + v, x + u) - right.at<uchar>(y + v, x + u - off);
                        }
                        ssd += ssd_tmp * ssd_tmp;
                    }
                }
                if (ssd < prev_ssd) {
                    prev_ssd = ssd;
                    best_dis = off;
                }
            }
            imgDisparity_y[x] = best_dis * adjust;
        }
    }
    return imgDisparity8U;
}
