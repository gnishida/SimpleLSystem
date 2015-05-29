#include "SimpleGreedyLSystem.h"
#include "MLUtils.h"

using namespace std;

SimpleGreedyLSystem::SimpleGreedyLSystem() {
}

cv::Mat_<double> SimpleGreedyLSystem::generate(int grid_size, const cv::Mat_<double>& param, bool smooth) {
	cv::Mat_<double> ret = cv::Mat_<double>::zeros(grid_size, grid_size);

	cv::Point2d pt(grid_size * 0.5, 0);
	moveForward(pt, ret);

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

cv::Mat_<double> SimpleGreedyLSystem::generateByTarget(int grid_size, const cv::Mat_<double>& target, bool smooth) {
	cv::Mat_<double> ret = cv::Mat_<double>::zeros(grid_size, grid_size);

	cv::Mat_<double> target_density = target.reshape(0, grid_size);

	cv::Point2d pt(grid_size * 0.5, 0);
	moveForward(pt, ret);

	for (int i = 0; i < grid_size - 1; ++i) {
		int p = chooseParam(pt, target_density);
		if (p == -1) {
			moveLeftForward(pt, ret);
		} else if (p == 0) {
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

int SimpleGreedyLSystem::chooseParam(const cv::Point2d& pt, const cv::Mat_<double> target_density) {
	if (pt.y >= target_density.rows) return 0;

	for (int r = pt.y; r < target_density.rows; ++r) {
		cv::Point2d vec(0, 0);
		bool found = false;

		// 直近の2行のdensity情報をもとに、方向ベクトルを計算する
		for (int rr = 0; rr < 2; ++rr) {
			if (r + rr >= target_density.rows) break;

			for (int c = 0; c < target_density.cols; ++c) {
				if (target_density(r + rr, c) == 0) continue;

				found = true;
				vec.x += (c + 0.5f - pt.x) * target_density(r + rr, c);
				vec.y += (r + rr - pt.y) * target_density(r + rr, c);
			}
		}

		// この行にdensity情報がなければ、次の行を探す
		if (!found) continue;

		if (vec.x == 0 && vec.y == 0) {
			return 0;
		} else if (vec.x >= vec.y) {
			return 1;
		} else if (vec.x <= -vec.y) {
			return -1;
		} else {
			return 0;
		}
	}


	// hack
	// 領域外に出させる
	if (pt.x <= 0) return -1;
	else if (pt.x >= target_density.cols) return -1;
	else return 0;
}

bool SimpleGreedyLSystem::moveForward(cv::Point2d& pt, cv::Mat_<double> density) {
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

bool SimpleGreedyLSystem::moveLeftForward(cv::Point2d& pt, cv::Mat_<double> density) {
	pt.x -= 1;
	pt.y += 1;

	if (pt.y > density.rows || pt.x < 0) return false;

	if (pt.x >= 0 && pt.x < density.cols) {
		density(pt.y - 1, pt.x)++;
	}

	return true;
}

bool SimpleGreedyLSystem::moveRightForward(cv::Point2d& pt, cv::Mat_<double> density) {
	pt.x += 1;
	pt.y += 1;

	if (pt.y > density.rows || pt.x > density.cols) return false;

	if (pt.x > 0 && pt.x <= density.cols) {
		density(pt.y - 1, pt.x - 1)++;
	}

	return true;
}