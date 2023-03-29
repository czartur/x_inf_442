#include <cmath> // for pow, atan, should you need them

#include <point.hpp>
#include <flat.hpp>

// TODO 2.1.1: implement volume and profile
// HINT: pi = std::atan(1) * 4.0
double flat::volume() const {
	int d = point().get_dim();
	double pi = std::atan(1) * 4.0;
	double ans = d&1 ? 1/sqrt(pi) : 1;
	ans *= pow(pi, d/2.0);

	if(d&1) for(double i = (d+1); i>(d+1)/2; i--) ans *= 4.0/i;
	else for(double i = d/2; i>1; i--) ans /= i; 

	return ans;

	// return pow(pi, d/2.0)/std::tgamma(d/2.0+1);
}

double flat::profile(double t) const {
	return (t <= 1);
}