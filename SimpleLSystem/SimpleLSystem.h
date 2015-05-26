#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class SimpleLSystem {
public:
	SimpleLSystem();

	cv::Mat_<double> computeDensity(int grid_size, const cv::Mat_<double>& param, bool smooth);
	bool moveForward(cv::Point2d& pt, cv::Mat_<double> density);
	bool moveLeftForward(cv::Point2d& pt, cv::Mat_<double> density);
	bool moveRightForward(cv::Point2d& pt, cv::Mat_<double> density);
};

