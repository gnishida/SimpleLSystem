#include "Radix.h"

Radix::Radix() {
}

std::vector<int> Radix::to(long long p, int q) {
	std::vector<int> ret;

	if (p == 0) {
		ret.push_back(0);
		return ret;
	}

	while (p > 0) {
		ret.push_back(p % q);
		p /= q;
	}

	return ret;
}

std::vector<int> Radix::to(long long p, int q, int num_digits) {
	std::vector<int> ret;

	if (p == 0) {
		ret.push_back(0);
	} else {
		while (p > 0) {
			ret.insert(ret.begin(), p % q);
			p /= q;
		}
	}

	for (int i = ret.size(); i < num_digits; ++i) {
		ret.insert(ret.begin(), 0);
	}

	return ret;
}