#pragma once

#include<string>
#include<iostream>
#include <vector>

class Radix {
public:
	Radix();

	std::vector<int> to(long long p, int q);
	std::vector<int> to(long long p, int q, int num_digits);
};