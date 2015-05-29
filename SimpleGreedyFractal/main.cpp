#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <algorithm>
#include <vector>
#include "Radix.h"
#include "SimpleGreedyFractal.h"
#include "MLUtils.h"
#include "LinearRegression.h"

using namespace std;

void test_linearity() {
	Radix r;
	SimpleGreedyFractal sls;

	cv::Mat target_density = cv::imread("target_density.jpg", 0);
	target_density.convertTo(target_density, CV_64F, 1.0/255.0);
	cv::flip(target_density, target_density, 0);
	
	cv::Mat_<double> result = sls.derive(target_density);
	ml::mat_save("result.png", result, false);
}



int main() {
	/*
	cv::Mat_<double> T = (cv::Mat_<double>(4, 4) << 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	cv::GaussianBlur(T, T, cv::Size(3, 3), 2, 2);
	cout << T << endl;
	*/

	test_linearity();

	return 0;
}
