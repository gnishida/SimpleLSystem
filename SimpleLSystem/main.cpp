#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <algorithm>
#include <vector>
#include "Radix.h"
#include "SimpleLSystem.h"
#include "MLUtils.h"
#include "LinearRegression.h"

using namespace std;

void test_linearity() {
	Radix r;
	SimpleLSystem sls;

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
		cv::Mat_<double> density = sls.computeDensity(grid_size, X.row(i), false);
			
		density.copyTo(Y.row(i));
	}

	ml::saveDataset("dataX.txt", X);
	ml::saveDataset("dataY.txt", Y);


	cv::Mat_<double> trainX, trainY;
	cv::Mat_<double> testX, testY;
	ml::splitDataset(X, 0.8, trainX, testX);
	ml::splitDataset(Y, 0.8, trainY, testY);

	// Forward
	{
		LinearRegression lr;
		lr.train(trainX, trainY);
		cv::Mat_<double> Y_hat = lr.predict(testX);

		cv::Mat_<double> Y_avg;
		cv::reduce(trainY, Y_avg, 0, CV_REDUCE_AVG);
		Y_avg = cv::repeat(Y_avg, testY.rows, 1);

		cout << "-----------------------" << endl;
		cout << "Forward:" << endl;
		cout << "RMSE: " << ml::rmse(testY, Y_hat, true) << endl;
		cout << "Baselime: " << ml::rmse(testY, Y_avg, true) << endl;
	}

	// Inverse
	{
		LinearRegression lr;
		lr.train(trainY, trainX);
		
		cv::Mat_<double> X_hat = lr.predict(testY);

		// Xの各値を-1,0,1にdiscretizeする
		{
			for (int r = 0; r < X_hat.rows; ++r) {
				for (int c = 0; c < X_hat.cols; ++c) {
					if (X_hat(r, c) < -0.5) {
						X_hat(r, c) = -1;
					} else if (X_hat(r,c ) > 0.5) {
						X_hat(r, c) = 1;
					} else {
						X_hat(r, c) = 0;
					}
				}
			}
		}
		
		for (int i = 0; i < testX.cols; ++i) {
			cout << ml::rmse(testX.col(i), X_hat.col(i), true) << endl;
		}


		cv::Mat_<double> Y_hat(testY.rows, testY.cols);
		for (int i = 0; i < testX.rows; ++i) {
			cv::Mat_<double> density_hat = sls.computeDensity(grid_size, X_hat.row(i), false);
			density_hat.copyTo(Y_hat.row(i));
		}

		cv::Mat X_avg;
		cv::reduce(trainX, X_avg, 0, CV_REDUCE_AVG);
		double baselineX = ml::rmse(testX, cv::repeat(X_avg, testX.rows, 1), true);

		cv::Mat Y_avg = sls.computeDensity(grid_size, X_avg, false);
		//cv::reduce(trainY, Y_avg, 0, CV_REDUCE_AVG);
		double baselineY = ml::rmse(testY, cv::repeat(Y_avg, testY.rows, 1), true);

		cout << "-----------------------" << endl;
		cout << "Inverse:" << endl;
		cout << "RMSE in Parameter: " << ml::rmse(testX, X_hat, true) << endl;
		cout << "Baselime: " << baselineX << endl;
		cout << "RMSE in Indicator: " << ml::rmse(testY, Y_hat, true) << endl;
		cout << "Baseline: " << baselineY << endl;
	}
}

void test_quadratic_linearity() {
	Radix r;
	SimpleLSystem sls;

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
		cv::Mat_<double> density = sls.computeDensity(grid_size, X.row(i), true);
			
		density.copyTo(Y.row(i));
	}

	cv::Mat_<double> qX;
	ml::quadratic_dataset(X, qX);
	cv::Mat_<double> qY;
	ml::quadratic_dataset(Y, qY);

	ml::saveDataset("dataX.txt", X);
	ml::saveDataset("dataY.txt", Y);


	cv::Mat_<double> trainX, trainY;
	cv::Mat_<double> testX, testY;
	cv::Mat_<double> qtrainX, qtestX;
	cv::Mat_<double> qtrainY, qtestY;
	ml::splitDataset(X, 0.8, trainX, testX);
	ml::splitDataset(Y, 0.8, trainY, testY);
	ml::splitDataset(qX, 0.8, qtrainX, qtestX);
	ml::splitDataset(qY, 0.8, qtrainY, qtestY);

	// Forward
	{
		LinearRegression lr;
		lr.train(qtrainX, trainY);
		cv::Mat_<double> Y_hat = lr.predict(qtestX);

		cv::Mat_<double> Y_avg;
		cv::reduce(trainY, Y_avg, 0, CV_REDUCE_AVG);
		Y_avg = cv::repeat(Y_avg, testY.rows, 1);

		cout << "-----------------------" << endl;
		cout << "Forward:" << endl;
		cout << "RMSE: " << ml::rmse(testY, Y_hat, true) << endl;
		cout << "Baselime: " << ml::rmse(testY, Y_avg, true) << endl;
	}

	// Inverse
	{
		LinearRegression lr;
		lr.train(qtrainY, trainX);
		
		cv::Mat_<double> X_hat = lr.predict(qtestY);

		// Xの各値を-1,0,1にdiscretizeする
		{
			for (int r = 0; r < X_hat.rows; ++r) {
				for (int c = 0; c < X_hat.cols; ++c) {
					if (X_hat(r, c) < -0.5) {
						X_hat(r, c) = -1;
					} else if (X_hat(r,c ) > 0.5) {
						X_hat(r, c) = 1;
					} else {
						X_hat(r, c) = 0;
					}
				}
			}
		}
		
		for (int i = 0; i < testX.cols; ++i) {
			cout << ml::rmse(testX.col(i), X_hat.col(i), true) << endl;
		}


		cv::Mat_<double> Y_hat(testY.rows, testY.cols);
		for (int i = 0; i < testX.rows; ++i) {
			cv::Mat_<double> density_hat = sls.computeDensity(grid_size, X_hat.row(i), true);
			density_hat.copyTo(Y_hat.row(i));
		}

		cv::Mat X_avg;
		cv::reduce(trainX, X_avg, 0, CV_REDUCE_AVG);
		double baselineX = ml::rmse(testX, cv::repeat(X_avg, testX.rows, 1), true);

		cv::Mat Y_avg = sls.computeDensity(grid_size, X_avg, true);
		//cv::reduce(trainY, Y_avg, 0, CV_REDUCE_AVG);
		double baselineY = ml::rmse(testY, cv::repeat(Y_avg, testY.rows, 1), true);

		cout << "-----------------------" << endl;
		cout << "Inverse:" << endl;
		cout << "RMSE in Parameter: " << ml::rmse(testX, X_hat, true) << endl;
		cout << "Baselime: " << baselineX << endl;
		cout << "RMSE in Indicator: " << ml::rmse(testY, Y_hat, true) << endl;
		cout << "Baseline: " << baselineY << endl;
	}
}

void draw_curve() {
	Radix r;
	SimpleLSystem sls;

	int grid_size = 10;
	//int grid_size = 4;
	int N = powf(3, grid_size - 1);

	srand(12345);
	int index = rand() % N;

	vector<int> param = r.to(index, 3, grid_size - 1);
	cv::Mat paramMat(param);
	paramMat = paramMat.t();
	paramMat.convertTo(paramMat, CV_64F);
	paramMat -= 1;

	cout << paramMat << endl;

	ofstream ofs("density_effect.txt");

	for (int i = 0; i < 3; ++i) {
		ofs << i - 1;
		paramMat.at<double>(0, 2) = i - 1;

		cv::Mat_<double> density = sls.computeDensity(grid_size, paramMat, true);
		for (int c = 0; c < density.cols; ++c) {
			ofs << " " << density(0, c);
		}
		ofs << endl;
	}
}

int main() {
	/*
	cv::Mat_<double> T = (cv::Mat_<double>(4, 4) << 1,0,0,0,0,0,0,0,0,0,0,0,0,0,0,0);
	cv::GaussianBlur(T, T, cv::Size(3, 3), 2, 2);
	cout << T << endl;
	*/

	test_linearity();

	//test_quadratic_linearity();

	//draw_curve();

	return 0;
}
