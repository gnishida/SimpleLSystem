#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <algorithm>
#include <vector>
#include "Radix.h"
#include "SimpleGreedyLSystem.h"
#include "MLUtils.h"
#include "LinearRegression.h"

using namespace std;

void test_linearity() {
	Radix r;
	SimpleGreedyLSystem sls;

	int grid_size = 10;
	//int grid_size = 4;
	int N = powf(3, grid_size - 1);

	cv::Mat_<double> X(N, grid_size - 1);
	cv::Mat_<double> Y(N, grid_size * grid_size);
	for (int i = 0; i < N; ++i) {
		vector<int> param = r.to(i, 3, grid_size - 1);
		cv::Mat paramMat(param);
		paramMat = paramMat.t();
		paramMat.convertTo(paramMat, CV_64F);
		paramMat -= 1;

		paramMat.copyTo(X.row(i));
	}

	ml::shuffle(X);

	for (int i = 0; i < X.rows; ++i) {
		cv::Mat_<double> density = sls.generate(grid_size, X.row(i), false);
			
		density.copyTo(Y.row(i));
	}

	ml::saveDataset("dataX.txt", X);
	ml::saveDataset("dataY.txt", Y);


	cv::Mat_<double> trainX, trainY;
	cv::Mat_<double> testX, testY;
	ml::splitDataset(X, 0.8, trainX, testX);
	ml::splitDataset(Y, 0.8, trainY, testY);

	cout << "-----------------------" << endl;
	cout << "RMSE: " << endl;

	// Inverse
	for (int k = 3; k < grid_size; ++k) {
		cv::Mat_<double> testY2 = testY.clone();

		// 最初のk行分を0にする
		cv::Mat_<double> z = cv::Mat_<double>::zeros(testY2.rows, 1);
		for (int c = 0; c < k * grid_size; ++c) {
			z.copyTo(testY2.col(c));
		}

		cv::Mat_<double> Y_hat(testY.rows, testY.cols);
		for (int i = 0; i < testY.rows; ++i) {
			SimpleGreedyLSystem sgls;


			cv::Mat_<double> density = sgls.generateByTarget(grid_size, testY2.row(i), false);
			density.copyTo(Y_hat.row(i));

			// check
			/*
			{
				bool ng = false;
				for (int c = k * grid_size; c < density.cols; ++c) {
					if (density(0, c) != testY(i, c)) {
						ng = true;
						break;
					}
				}

				if (ng) {
					cout << testY2.row(i).reshape(1, grid_size) << endl;
					cout << density.reshape(1, grid_size) << endl;
				}
			}
			*/
		}

		// 最初のk行分を0にする
		for (int c = 0; c < k * grid_size; ++c) {
			z.copyTo(Y_hat.col(c));
		}

		cout << k << ": " << ml::rmse(testY2, Y_hat, true) << endl;
	}

	cv::Mat X_avg;
	cv::reduce(trainX, X_avg, 0, CV_REDUCE_AVG);
	double baselineX = ml::rmse(testX, cv::repeat(X_avg, testX.rows, 1), true);

	cv::Mat Y_avg = sls.generate(grid_size, X_avg, true);
	double baselineY = ml::rmse(testY, cv::repeat(Y_avg, testY.rows, 1), true);

	cout << "-----------------------" << endl;
	cout << "Baseline: " << baselineY << endl;
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
