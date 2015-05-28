#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class SimpleGreedyLSystemWithBranching {
public:
	SimpleGreedyLSystemWithBranching();

	cv::Mat_<double> computeDensity(int grid_size, const cv::Mat_<double>& param, bool smooth, bool crossing_check);
	void moveForward(cv::Point2d& pt, cv::Mat_<double> density, bool crossing_check);
	void moveLeftForward(cv::Point2d& pt, cv::Mat_<double> density, bool crossing_check);
	void moveRightForward(cv::Point2d& pt, cv::Mat_<double> density, bool crossing_check);
};

