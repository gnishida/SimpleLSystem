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
	N = 4;
	delta = 90;
	axiom = "F-F-F-F";
	rules['F'] = "F+FF-FF-F-F+F+FF-F-F+F+FF+FF-F";
	*/

	N = 8;
	delta = 70;
	axiom = "F";
	rules['F'] = "F[+FF]F[-FF]F";

	//rule = derive(target_density);
}

cv::Mat_<double> SimpleGreedyFractal::derive(const cv::Mat_<double>& target_density) {
	Rule result(axiom, 0);

	for (int n = 0; n < N; ++n) {
		// 一旦描画して、大きさなどを計算し、セグメントの長さなどを計算する
		glm::vec2 minPt, maxPt;
		//draw(result, 1, minPt, maxPt);

		//float scale = computeScale(target_width, target_height, maxPt.x - minPt.x, maxPt.y - minPt.y);
		cv::Mat_<double> density = computeDensity(result, 100.0f, glm::vec2(50, 0));
		ml::mat_save("density.png", density, false);

		float score = computeScore(target_density, density);

		vector<bool> replace;
		for (int i = 0; i < result.length(); ++i) {
			if (result[i].level == n && rules.find(result[i].c) != rules.end()) {
				// 試しに描画して、スコアが上がるなら、書き換え
				Rule next = result;
				next.replace(i, Rule(rules[result[i].c], n + 1));

				glm::vec2 minPt2, maxPt2;
				//draw(next, 1, minPt2, maxPt2);

				//float scale2 = computeScale(target_width, target_height, maxPt2.x - minPt2.x, maxPt2.y - minPt2.y);
				cv::Mat_<double> density2 = computeDensity(next, 100.0f, glm::vec2(50, 0));
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
			if (rules.find(result[i].c) == rules.end() || result[i].level != n) {
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

	return computeDensity(result, 100.0f, glm::vec2(50, 0));
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
			drawForward(modelMat, segment_length / pow(3.0, rule[i].level), density);
			modelMat = glm::translate(modelMat, glm::vec3(0, segment_length / pow(3.0, rule[i].level), 0));
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

	cv::line(density, cv::Point(p1.x, p1.y), cv::Point(p2.x, p2.y), cv::Scalar(0), 1);
}

float SimpleGreedyFractal::computeScore(const cv::Mat_<double>& target_density, const cv::Mat_<double>& density) {
	//return -ml::mat_sum(ml::mat_square(target_density - density));
	float score = 0.0f;
	for (int r = 0; r < target_density.rows; ++r) {
		for (int c = 0; c < target_density.cols; ++c) {
			if (target_density(r, c) == 0 && density(r, c) == 0) {
				score += 100.0f;
			} else if (target_density(r, c) == 1 && density(r, c) == 1) {
				score += 10.0f;
			} else {
				score -= 1.0f;
			}
		}
	}
	return score;
}

/*float SimpleGreedyFractal::computeScale(float target_width, float target_height, float width, float height) {
	float scale1 = target_width / width;
	float scale2 = target_height / height;

}*/

float SimpleGreedyFractal::deg2rad(float deg) {
	return deg * M_PI / 180.0;
}


