#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtx/string_cast.hpp>
#include <opencv/cv.h>
#include <opencv/highgui.h>
#include <vector>
#include <map>
#include <random>

using namespace std;

class Literal {
public:
	char c;
	int level;

public:
	Literal(char c, int level) : c(c), level(level) {}
};

class Rule {
public:
	vector<Literal> rule;

public:
	Rule();
	Rule(string str, int level);
	Rule(const Literal& l);

	int length() const;
	Literal operator[](int index) const;
	Literal& operator[](int index);
	void operator+=(const Rule& r);

	void replace(int index, const Rule& r);
};

class SimpleGreedyFractal {
public:
	int N;
	double delta;
	string axiom;
	map<char, string> rules;

public:
	SimpleGreedyFractal();
	cv::Mat_<double> derive(const cv::Mat_<double>& target_density);

private:
	//void draw(string rule, float segment_length, glm::vec2& minPt, glm::vec2& maxPt);
	cv::Mat_<double> computeDensity(const Rule& rule, float segment_length, const glm::vec2& startPt);
	void drawForward(const glm::mat4& modelMat, float segment_length, cv::Mat_<double>& density);
	//float computeScale(float target_width, float target_height, float width, float height);
	float computeScore(const cv::Mat_<double>& target_density, const cv::Mat_<double>& density);
	float deg2rad(float deg);
};


