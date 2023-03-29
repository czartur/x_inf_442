#include <cmath> // for pow, atan, should you need them
#include <iostream> // for cerr

#include <point.hpp>
#include <cloud.hpp>
#include <gaussian.hpp>

// TODO 2.1.2: implement volume, profile and guess_bandwidth
// HINTS: pi = std::atan(1) * 4.0, e^x is std::exp(x)
double gaussian::volume() const {
	int d = point().get_dim();
	return pow(8.0*std::atan(1), d/2.0);
}

double gaussian::profile(double t) const {
	return std::exp(-t/2.0);
}

void gaussian::guess_bandwidth() {
	double std = data->standard_deviation();
	double n = data->get_n();
	bandwidth = 1.06*std/pow(n, 1.0/5.0);
}
