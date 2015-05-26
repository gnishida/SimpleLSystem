#include "SimpleLSystem.h"

using namespace std;

SimpleLSystem::SimpleLSystem() {
}

cv::Mat_<double> SimpleLSystem::computeDensity(int grid_size, const cv::Mat_<double>& param, bool smooth) {
	cv::Mat_<double> ret = cv::Mat_<double>::zeros(grid_size, grid_size);

	cv::Point2d pt(grid_size * 0.5, 0);
	moveForward(pt, ret);

	/*
	for (int i = 0; i < param.cols; ++i) {
		if (param(0, i) == -1) {
			if (!moveLeftForward(pt, ret)) throw "Tree grew out of the space.";
		} else if (param(0, i) == 0) {
			if (!moveForward(pt, ret)) throw "Tree grew out of the space.";
		} else {
			if (!moveRightForward(pt, ret)) throw "Tree grew out of the space.";
		}
	}
	*/

	for (int i = 0; i < param.cols; ++i) {
		if (param(0, i) == -1) {
			moveLeftForward(pt, ret);
		} else if (param(0, i) == 0) {
			moveForward(pt, ret);
		} else {
			moveRightForward(pt, ret);
		}
	}

	if (smooth) {
		cv::GaussianBlur(ret, ret, cv::Size(5, 5), 1, 1);
	}

	return ret.reshape(1, 1);
}

bool SimpleLSystem::moveForward(cv::Point2d& pt, cv::Mat_<double> density) {
	pt.y += 1;

	if (pt.y > density.rows) return false;

	if (pt.x > 0 && pt.x <= density.cols) {
		density(pt.y - 1, pt.x - 1) += 0.5;
	}
	if (pt.x >= 0 && pt.x < density.cols) {
		density(pt.y - 1, pt.x) += 0.5;
	}

	return true;
}

bool SimpleLSystem::moveLeftForward(cv::Point2d& pt, cv::Mat_<double> density) {
	pt.x -= 1;
	pt.y += 1;

	if (pt.y > density.rows || pt.x < 0) return false;

	if (pt.x >= 0 && pt.x < density.cols) {
		density(pt.y - 1, pt.x)++;
	}

	return true;
}

bool SimpleLSystem::moveRightForward(cv::Point2d& pt, cv::Mat_<double> density) {
	pt.x += 1;
	pt.y += 1;

	if (pt.y > density.rows || pt.x > density.cols) return false;

	if (pt.x > 0 && pt.x <= density.cols) {
		density(pt.y - 1, pt.x - 1)++;
	}

	return true;
}