#include <iostream>
#include <cassert>
#include <cmath>	// for sqrt, fabs
#include <cfloat>	// for DBL_MAX
#include <cstdlib>	// for rand, srand
#include <ctime>	// for rand seed
#include <fstream>
#include <cstdio>	// for EOF
#include <string>
#include <algorithm>	// for count
#include <vector>

using std::rand;
using std::srand;
using std::time;

class point
{
    public:

        static int d;
        double *coords;
        int label;

		point(){
			coords = new double[d]();
			// for(int i=0; i<d; i++) coords[i] = 0;
			label = 0;
		}

		~point(){
			delete[] coords;
		}

		point(const point& q): label{q.label} {
			for(int i=0; i<d; i++) coords[i] = q.coords[i];
		}

		point& operator=(const point& q){
			label = q.label;
			// coords = q.coords;
			for(int i=0; i<d; i++) coords[i] = q.coords[i];
			return *this;
		}

		void print() const {
			for(int i=0; i<d; i++) std::cout << coords[i] << '\t';
			std::cout << '\n';
		}

		double squared_dist(const point &q) const {
			double res = 0, dif;
			for(int i=0; i<d; i++){ 
				dif = q.coords[i] - coords[i];
				res += dif*dif;
			}
			return res;
		}

		point& operator/=(double val){
			for(int i=0; i<d; i++) coords[i]/=val;
			return *this;
		}

		point& operator+=(point &q){
			for(int i=0; i<d; i++) coords[i] += q.coords[i];
			return *this;
		}
};

int point::d;

class cloud
{
	private:

	int d;
	int n;
	int k;

	// maximum possible number of points
	int nmax;

	point *points;
	point *centers;


	public:

	cloud(int _d, int _nmax, int _k)
	{
		d = _d;
		point::d = _d;
		n = 0;
		k = _k;

		nmax = _nmax;

		points = new point[nmax];
		centers = new point[k];

		srand(time(0));
	}

	~cloud()
	{
		delete[] centers;
		delete[] points;
	}

	void add_point(const point &p, int label)
	{
		for(int m = 0; m < d; m++)
		{
			points[n].coords[m] = p.coords[m];
		}

		points[n].label = label;

		n++;
	}

	int get_d() const
	{
		return d;
	}

	int get_n() const
	{
		return n;
	}

	int get_k() const
	{
		return k;
	}

	point& get_point(int i)
	{
		return points[i];
	}

	point& get_center(int j)
	{
		return centers[j];
	}

	void set_center(const point &p, int j)
	{
		for(int m = 0; m < d; m++)
			centers[j].coords[m] = p.coords[m];
	}

	double intracluster_variance() const
	{
		double res = 0;
		for(int i=0; i<n; i++){
			res += points[i].squared_dist(centers[points[i].label]);
		}
		return res/n; 
	}

	int set_voronoi_labels()
	{
		int n_updates = 0;
		for(int i=0; i<n; i++){
			double min_squared_dist = points[i].squared_dist(centers[points[i].label]);
			int idx = points[i].label;
			for(int j=0; j<k; j++){
				double cur_squared_dist = points[i].squared_dist(centers[j]);
				if(cur_squared_dist <= min_squared_dist) {
					min_squared_dist = cur_squared_dist;
					idx = cur_squared_dist == min_squared_dist ? std::min(idx, j) : j;
				}
			}
			if(points[i].label != idx) n_updates++, points[i].label = idx;
		}
		return n_updates;
	}

	void set_centroid_centers()
	{
		int* cluster_size = new int[k]();

		for(int i=0; i<n; ++i) 
			++cluster_size[points[i].label];
 
		for(int i=0; i<k; ++i) if(cluster_size[i]) 
			centers[i] = point();

		for(int i=0; i<n; ++i)
			centers[points[i].label] += points[i];

		for(int i=0; i<k; ++i) if(cluster_size[i])
			centers[i]/=(double) cluster_size[i];
	}

	void init_random_partition()
	{
		for(int i=0; i<n; i++) points[i].label = rand()%k;
		set_centroid_centers();
	}

	void lloyd()
	{
		while(set_voronoi_labels()) set_centroid_centers();
	}

	void init_forgy()
	{
		bool* chosen = new bool[n]();

		for(int i=0; i<k; ++i){
			int choice = rand()%n;
			while(chosen[choice]) choice = rand()%n;
			chosen[choice] = 1;
			centers[i] = points[choice];
		}
	}

	void init_plusplus()
	{
		double* squared_distance = new double[n];
		for(int i=0; i<n; i++) squared_distance[i] = DBL_MAX;
		int last_center = rand()%n;
		centers[0] = points[last_center]; 

		for(int i=1; i<k; ++i){
			double total_weight = 0;
			for(int j=0; j<n; ++j) 
				total_weight += squared_distance[j] = std::min(squared_distance[j], points[j].squared_dist(points[last_center]));
			
			double rnd = (double) rand() / RAND_MAX;
			
			// update last_center
			last_center = -1;
			while(rnd > 0) ++last_center, rnd -= squared_distance[last_center]/total_weight;
			centers[i] = points[last_center];
		}
	}
};
