#pragma once

#include <opencv/cv.h>
#include <opencv/highgui.h>

class SimpleGreedyLSystem {
public:
	SimpleGreedyLSystem();

	cv::Mat_<double> generate(int grid_size, const cv::Mat_<double>& param, bool smooth);
	cv::Mat_<double> generateByTarget(int grid_size, const cv::Mat_<double>& target, bool smooth);

private:
	int chooseParam(const cv::Point2d& pt, const cv::Mat_<double> target_density);
	bool moveForward(cv::Point2d& pt, cv::Mat_<double> density);
	bool moveLeftForward(cv::Point2d& pt, cv::Mat_<double> density);
	bool moveRightForward(cv::Point2d& pt, cv::Mat_<double> density);
};

