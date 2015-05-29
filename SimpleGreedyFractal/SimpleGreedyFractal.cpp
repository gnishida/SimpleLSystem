#include "SimpleGreedyFractal.h"
#include <iostream>
#include <time.h>
#include <list>
#include "MLUtils.h"

const double M_PI = 3.141592653592;

Rule::Rule() {
}

Rule::Rule(string str, int level) {
	for (int i = 0; i < str.length(); ++i) {
		rule.push_back(Literal(str[i], level));
	}
}

Rule::Rule(const Literal& l) {
	rule.push_back(l);
}

int Rule::length() const {
	return rule.size();
}

Literal Rule::operator[](int index) const {
	return rule[index];
}

Literal& Rule::operator[](int index) {
	return rule[index];
}

void Rule::operator+=(const Rule& r) {
	rule.insert(rule.end(), r.rule.begin(), r.rule.end());
}

void Rule::replace(int index, const Rule& r) {
	rule.erase(rule.begin() + index);
	rule.insert(rule.begin() + index, r.rule.begin(), r.rule.end());
}

SimpleGreedyFractal::SimpleGreedyFractal() {
	/*
	N = 5;
	delta = 25.7;
	axiom = 'F';
	rules['F'].push_back(pair<double, string>(1.0, "F[+F]F[-F]F"));
	*/

	/*
	N = 5;
	delta = 20;
	axiom = 'F';
	rules['F'].push_back(pair<double, string>(1.0, "F[+F]F[-F][F]"));
	*/

	/*
	N = 4;
	delta = 22.5;
	axiom = 'F';
	rules['F'].push_back(pair<double, string>(1.0, "FF-[-F+F+F]+[+F-F-F]"));
	*/

	/*
	N = 7;
	delta = 20;
	axiom = 'X';
	rules['X'].push_back(pair<double, string>(1.0, "F[+X]F[-X]+X"));
	rules['F'].push_back(pair<double, string>(1.0, "FF"));
	*/

	/*
	N = 7;
	delta = 25.7;
	axiom = 'X';
	rules['X'].push_back(pair<double, string>(1.0, "F[+X][-X]FX"));
	rules['F'].push_back(pair<double, string>(1.0, "FF"));
	*/

	/*
	N = 5;
	delta = 22.5;
	axiom = 'X';
	rules['X'].push_back(pair<double, string>(1.0, "F-[[X]+X]+F[+FX]-X"));
	rules['F'].push_back(pair<double, string>(1.0, "FF"));
	*/

	/*
	N = 7;
	delta = 22.5;
	axiom = 'A';
	rules['A'].push_back(pair<double, string>(1.0, "[&FL!A]/////'[&FL!A]///////'[&FL!A]"));
	rules['F'].push_back(pair<double, string>(1.0, "S/////F"));
	rules['S'].push_back(pair<double, string>(1.0, "FL"));
	rules['L'].push_back(pair<double, string>(1.0, "['''^^f]"));
	*/

	/*
	N = 5;
	delta = 25.7;
	axiom = 'F';
	rules['F'].push_back(pair<double, string>(0.33, "F[+F]F[-F]F"));
	rules['F'].push_back(pair<double, string>(0.33, "F[+F]F"));
	rules['F'].push_back(pair<double, string>(0.34, "F[-F]F"));
	*/

	N = 2;
	delta = 90;
	axiom = "F-F-F-F";
	rules['F'] = "F+FF-FF-F-F+F+FF-F-F+F+FF+FF-F";

	//rule = derive(target_density);
}

cv::Mat_<double> SimpleGreedyFractal::derive(const cv::Mat_<double>& target_density) {
	Rule result(axiom, 0);

	for (int n = 0; n < N; ++n) {
		// 一旦描画して、大きさなどを計算し、セグメントの長さなどを計算する
		glm::vec2 minPt, maxPt;
		//draw(result, 1, minPt, maxPt);

		//float scale = computeScale(target_width, target_height, maxPt.x - minPt.x, maxPt.y - minPt.y);
		cv::Mat_<double> density = computeDensity(result, 50.0f, glm::vec2(25, 25));
		ml::mat_save("density.png", density, false);

		float score = computeScore(target_density, density);

		vector<bool> replace;
		for (int i = 0; i < result.length(); ++i) {
			if (rules.find(result[i].c) != rules.end()) {
				// 試しに描画して、スコアが上がるなら、書き換え
				Rule next = result;
				next.replace(i, Rule(rules[result[i].c], n + 1));

				glm::vec2 minPt2, maxPt2;
				//draw(next, 1, minPt2, maxPt2);

				//float scale2 = computeScale(target_width, target_height, maxPt2.x - minPt2.x, maxPt2.y - minPt2.y);
				cv::Mat_<double> density2 = computeDensity(next, 50.0f, glm::vec2(25, 25));
				ml::mat_save("density2.png", density2, false);
				float score2 = computeScore(target_density, density2);

				if (score2 > score) {
					replace.push_back(true);
				} else {
					replace.push_back(false);
				}
			}
		}

		// replaceフラグに基づいて、書き換え
		Rule next;
		int count = 0;
		for (int i = 0; i < result.length(); ++i) {
			if (rules.find(result[i].c) == rules.end()) {
				next += Rule(result[i]);
			} else {
				if (replace[count++]) {
					next += Rule(rules[result[i].c], n + 1);
				} else {
					next += result[i];
				}
			}
		}

		result = next;
	}

	return computeDensity(result, 50.0f, glm::vec2(25, 25));
}

/*
void SimpleGreedyFractal::draw(const Rule& rule, float segment_length, glm::vec2& minPt, glm::vec2& maxPt) {
	std::list<glm::mat4> list;

	minPt = glm::vec2(0, 0);
	maxPt = glm::vec2(0, 0);

	glm::mat4 modelMat;
	for (int i = 0; i < rule.length(); ++i) {
		if (rule[i] == '[') {
			list.push_back(modelMat);
		} else if (rule[i] == ']') {
			modelMat = list.back();
			list.pop_back();
		} else if (rule[i] == '+') {
			modelMat = glm::rotate(modelMat, deg2rad(delta), glm::vec3(0, 0, 1));
		} else if (rule[i] == '-') {
			modelMat = glm::rotate(modelMat, deg2rad(-delta), glm::vec3(0, 0, 1));
		} else if (rule[i] == '\\') {
			modelMat = glm::rotate(modelMat, deg2rad(delta), glm::vec3(0, 1, 0));
		} else if (rule[i] == '/') {
			modelMat = glm::rotate(modelMat, deg2rad(-delta), glm::vec3(0, 1, 0));
		} else if (rule[i] == '&') {
			modelMat = glm::rotate(modelMat, deg2rad(delta), glm::vec3(1, 0, 0));
		} else if (rule[i] == '^') {
			modelMat = glm::rotate(modelMat, deg2rad(-delta), glm::vec3(1, 0, 0));
		} else if (rule[i] == '|') {
			modelMat = glm::rotate(modelMat, deg2rad(180), glm::vec3(0, 0, 1));
		} else if (rule[i] == 'F') {
			//drawForward(modelMat, segment_length, density);
			modelMat = glm::translate(modelMat, glm::vec3(0, segment_length, 0));

			glm::vec4 pt(0, 0, 0, 1);
			pt = modelMat * pt;
			minPt.x = min(pt.x, minPt.x);
			minPt.y = min(pt.y, minPt.y);
			maxPt.x = max(pt.x, maxPt.x);
			maxPt.y = max(pt.y, maxPt.y);
		}
	}
}*/

cv::Mat_<double> SimpleGreedyFractal::computeDensity(const Rule& rule, float segment_length, const glm::vec2& startPt) {
	std::list<glm::mat4> list;

	cv::Mat_<double> density = cv::Mat_<double>::ones(100, 100);

	glm::mat4 modelMat;
	modelMat = glm::translate(modelMat, glm::vec3(startPt.x, startPt.y, 0));

	for (int i = 0; i < rule.length(); ++i) {
		if (rule[i].c == '[') {
			list.push_back(modelMat);
		} else if (rule[i].c == ']') {
			modelMat = list.back();
			list.pop_back();
		} else if (rule[i].c == '+') {
			modelMat = glm::rotate(modelMat, deg2rad(delta), glm::vec3(0, 0, 1));
		} else if (rule[i].c == '-') {
			modelMat = glm::rotate(modelMat, deg2rad(-delta), glm::vec3(0, 0, 1));
		} else if (rule[i].c == '\\') {
			modelMat = glm::rotate(modelMat, deg2rad(delta), glm::vec3(0, 1, 0));
		} else if (rule[i].c == '/') {
			modelMat = glm::rotate(modelMat, deg2rad(-delta), glm::vec3(0, 1, 0));
		} else if (rule[i].c == '&') {
			modelMat = glm::rotate(modelMat, deg2rad(delta), glm::vec3(1, 0, 0));
		} else if (rule[i].c == '^') {
			modelMat = glm::rotate(modelMat, deg2rad(-delta), glm::vec3(1, 0, 0));
		} else if (rule[i].c == '|') {
			modelMat = glm::rotate(modelMat, deg2rad(180), glm::vec3(0, 0, 1));
		} else if (rule[i].c == 'F') {
			drawForward(modelMat, segment_length / pow(6.0, rule[i].level), density);
			modelMat = glm::translate(modelMat, glm::vec3(0, segment_length / pow(6.0, rule[i].level), 0));
		}
	}

	//cv::GaussianBlur(density, density, cv::Size(3, 3), 1, 1);

	return density;
}

/**
 * XZ面を底面とし、Y軸に伸びる円筒形を描画する。
 *
 * @modelMat		モデル行列
 * @top_radius		円筒形の上面の半径
 * @base_radius		円筒形の底面の半径
 * @height			円筒形の高さ
 * @color			色
 */
void SimpleGreedyFractal::drawForward(const glm::mat4& modelMat, float segment_length, cv::Mat_<double>& density) {
	glm::vec4 p1(0, 0, 0, 1);
	glm::vec4 p2(0, segment_length, 0, 1);

	p1 = modelMat * p1;
	p2 = modelMat * p2;

	cv::line(density, cv::Point(p1.x, p1.y), cv::Point(p2.x, p2.y), cv::Scalar(0), 3);
}

float SimpleGreedyFractal::computeScore(const cv::Mat_<double>& target_density, const cv::Mat_<double>& density) {
	return -ml::mat_sum(ml::mat_square(target_density - density));
}

/*float SimpleGreedyFractal::computeScale(float target_width, float target_height, float width, float height) {
	float scale1 = target_width / width;
	float scale2 = target_height / height;

}*/

float SimpleGreedyFractal::deg2rad(float deg) {
	return deg * M_PI / 180.0;
}


