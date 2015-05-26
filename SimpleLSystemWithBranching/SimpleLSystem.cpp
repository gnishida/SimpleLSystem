#include "SimpleLSystem.h"

using namespace std;

SimpleLSystem::SimpleLSystem() {
}

cv::Mat_<double> SimpleLSystem::computeDensity(int grid_size, const cv::Mat_<double>& param, bool smooth, bool crossing_check) {
	cv::Mat_<double> ret = cv::Mat_<double>::zeros(grid_size, grid_size);
	//cout << param << endl;

	cv::Point2d pt(grid_size * 0.5, 0);
	moveForward(pt, ret, crossing_check);

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

	int b_index = 3;
	cv::Point2d pt2 = pt;

	for (int i = 0; i < grid_size - 1; ++i) {
		if (i < b_index) {
			if (param(0, i) == -1) {
				moveLeftForward(pt, ret, crossing_check);
			} else if (param(0, i) == 0) {
				moveForward(pt, ret, crossing_check);
			} else {
				moveRightForward(pt, ret, crossing_check);
			}
			pt2 = pt;
		} else if (i == b_index) {
			moveLeftForward(pt, ret, crossing_check);
			moveRightForward(pt2, ret, crossing_check);
		} else {
			if (param(0, i-1) == -1) {
				moveLeftForward(pt, ret, crossing_check);
			} else if (param(0, i-1) == 0) {
				moveForward(pt, ret, crossing_check);
			} else {
				moveRightForward(pt, ret, crossing_check);
			}
			i++;
			if (param(0, i-1) == -1) {
				moveLeftForward(pt2, ret, crossing_check);
			} else if (param(0, i-1) == 0) {
				moveForward(pt2, ret, crossing_check);
			} else {
				moveRightForward(pt2, ret, crossing_check);
			}
		}
	}

	if (smooth) {
		cv::GaussianBlur(ret, ret, cv::Size(5, 5), 1, 1);
	}

	return ret.reshape(1, 1);
}

void SimpleLSystem::moveForward(cv::Point2d& pt, cv::Mat_<double> density, bool crossing_check) {
	pt.y += 1;

	if (pt.y > density.rows) return;

	if (pt.x > 0 && pt.x <= density.cols) {
		if (crossing_check && density(pt.y - 1, pt.x - 1) > 0.5) throw "More than one branch in a cell.";
		density(pt.y - 1, pt.x - 1) += 0.5;
	}
	if (pt.x >= 0 && pt.x < density.cols) {
		if (crossing_check && density(pt.y - 1, pt.x) > 0.5) throw "More than one branch in a cell.";
		density(pt.y - 1, pt.x) += 0.5;
	}
}

void SimpleLSystem::moveLeftForward(cv::Point2d& pt, cv::Mat_<double> density, bool crossing_check) {
	pt.x -= 1;
	pt.y += 1;

	if (pt.y > density.rows || pt.x < 0) return;

	if (pt.x >= 0 && pt.x < density.cols) {
		if (crossing_check && density(pt.y - 1, pt.x) > 0) throw "More than one branch in a cell.";
		density(pt.y - 1, pt.x)++;
	}
}

void SimpleLSystem::moveRightForward(cv::Point2d& pt, cv::Mat_<double> density, bool crossing_check) {
	pt.x += 1;
	pt.y += 1;

	if (pt.y > density.rows || pt.x > density.cols) return;

	if (pt.x > 0 && pt.x <= density.cols) {
		if (crossing_check && density(pt.y - 1, pt.x - 1) > 0) throw "More than one branch in a cell.";
		density(pt.y - 1, pt.x - 1)++;
	}
}