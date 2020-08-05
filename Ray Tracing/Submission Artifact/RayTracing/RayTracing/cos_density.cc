#include "constants.h"

#include <iostream>
#include <math.h>
#include <iomanip>
#include <stdlib.h>

//int main() {
//	int N = 1000000;
//
//	auto sum = 0.0;
//	for (int i = 0; i < N; i++) {
//		auto v = random_cosine_direction();
//		sum += v.z() * v.z() * v.z() / (v.z() / pi);
//	}
//
//	std::cout << std::fixed << std::setprecision(12);
//	std::cout << "Pi/2     = " << pi / 2 << '\n';
//	std::cout << "Estimate = " << sum / N << '\n';
//}