#include <cmath> // for pow, should you need it

#include <point.hpp>
#include <cloud.hpp>
#include <radial.hpp>

#include <iostream>
// TODO 2.1: density and radial constructor
// Use profile and volume... although it will only be implemented in the "sisters" classes
// Use kernel's constructor

radial::radial(cloud* data_, double bandwidth_)
: kernel(data_), bandwidth{bandwidth_} { }


double radial::density(const point& p) const
{
	double ans = 0;
	int n = data->get_n();
	int d = p.get_dim();
	for(int i=0; i<n; ++i){
		double dist = p.dist(data->get_point(i));
		ans += profile(dist*dist/(bandwidth*bandwidth));
	}
	
	ans /= (volume()*n*pow(bandwidth, d));
	return ans;
}
