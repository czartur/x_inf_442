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
        point()
        {
            coords = new double[d];
            label = 0;
        }

	   ~point()
	   {
        // std::cout << "destructor" << std::endl;
		delete[] coords;
	   }
	   void print() const
	   {
	       for (int i = 0; i < d; i++)
           {
               std::cout << coords[i];
               if (i < d - 1) std::cout << "\t";
           }
	   }
	   double squared_dist(const point &q) const
	   {
	       double distance = 0;
	       for (int i = 0; i < d; i++)
           {
               distance += (coords[i] - q.coords[i]) * (coords[i] - q.coords[i]);
           }
           return distance;
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
	    double m = 0;
	    for (int i = 0; i < n; i++)
        {
            int label = points[i].label;
            for (int j = 0; j < d; j++)
            {
                m += (points[i].coords[j] - centers[label].coords[j]) * (points[i].coords[j] - centers[label].coords[j]);
            }

        }
		return m / n;
	}

	int set_voronoi_labels()
	{
	    int change = 0;
	    for (int i = 0; i < n; i++)
        {
            int label = points[i].label;
            for (int j = 0; j < k; j++)
            {
                if (points[i].squared_dist(centers[j]) < points[i].squared_dist(centers[points[i].label]))
                {
                    points[i].label = j;
                }
            }
            if (label != points[i].label) change++;
        }
		return change;
	}

	void set_centroid_centers()
	{
        int* clusters = new int[k];
        double clusterscoord[k][d];
        for (int i = 0; i < k; i++)
        {
            clusters[i] = 0;
            for (int j = 0; j < d; j++)
                clusterscoord[i][j] = 0;
        }

	    for (int i = 0; i < n; i++)
        {
            clusters[points[i].label] ++;
            for (int j = 0; j < d; j++)
            {
                clusterscoord[points[i].label][j] += points[i].coords[j];
            }
        }
        for (int i = 0; i < k; i++)
        {
            if (clusters[i] != 0)
            {
                for (int j = 0; j < d; j++)
                {
                    centers[i].coords[j] = clusterscoord[i][j] / clusters[i];
                }
            }
        }
	}

	void init_random_partition()
	{
	    for (int i = 0; i < n; i++)
        {
            points[i].label = rand() % k;
        }
	    set_centroid_centers();
	}

	void lloyd()
	{
	    while(set_voronoi_labels() != 0)
        {
            set_centroid_centers();
        }
	}

	void init_forgy()
	{
	    int* chosen = new int[n];
	    for (int i = 0; i < n; i++)
            chosen[i] = 0;
	    for (int i = 0; i < k; i++)
        {
            int lab = rand() % n;
            while (chosen[lab] != 0)
                lab = rand() % n;
            for (int j = 0; j < d; j++)
                centers[i].coords[j] = points[lab].coords[j];
            chosen[lab] = 1;
        }
	}

	void init_plusplus()
	{
		//std::cout << n << std::endl;
	    int idx_center = rand() % n;
		for (int m = 0; m < d; m++)         
        	centers[0].coords[m] = points[idx_center].coords[m];
    
	    double *min_dist = new double[n];
	    for (int i = 0; i < n; i++)
            min_dist[i] = points[i].squared_dist(centers[0]);
	    for (int i = 1; i < k; i++)
        {
            double total_dist= 0;
            for (int j = 0; j < n; j++)
            {
                if (min_dist[j] > points[j].squared_dist(centers[i - 1]))
                {
                    min_dist[j] = points[j].squared_dist(centers[i - 1]);
                }
                total_dist+= min_dist[j];
            }
            double r = ((double)rand() / RAND_MAX);

            //check what point we will choose
            double dist = min_dist[0];
            int j = 0;
            while (dist/total_dist< r && j < n)
            {
                dist+= min_dist[j];
                j++;
            }
			if (j == n) j--;
			//std::cout << j << std::endl;
            for (int m = 0; m < d; m++)
            {
                centers[i].coords[m] = points[j].coords[m];
            }
        }
	}
};
