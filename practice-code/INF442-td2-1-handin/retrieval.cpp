#include <algorithm>  // for sort
#include <cassert>    // for assertions
#include <cfloat>     // for DBL_MAX
#include <cmath>      // for math operations like sqrt, log, etc
#include <cstdlib>    // for rand, srand
#include <ctime>      // for clock
#include <fstream>    // for ifstream
#include <iostream>   // for cout

#include "retrieval.hpp"

using std::cout;
using std::endl;

/*****************************************************
 * TD 2: K-Dimensional Tree (kd-tree)                *
 *****************************************************/

void print_point(point p, int dim) {
    std::cout << "[ ";
    for (int i = 0; i < dim; ++i) {
        std::cout << p[i] << " ";
    }
    std::cout << "]" << std::endl;
}

void pure_print(point p, int dim) {
    std::cout << p[0];
    for (int j = 1; j < dim; j++)
        std::cout << " " << p[j];
    std::cout << "\n";
}

/*****************************************************
 * Exercise 1: dist                                  *
 *****************************************************/
/**
 * This function computes the Euclidean distance between two points
 *
 * @param p  the first point
 * @param q the second point
 * @param dim the dimension of the space where the points live
 * @return the distance between p and q, i.e., the length of the segment pq
 */
double dist(point p, point q, int dim) {
    // Exercise 1
    double distance = 0.0;
    for(int i=0; i<dim; i++){
        double inc = *(p+i) - *(q+i);
        distance += inc*inc;
    }
    return sqrt(distance);
}

/*****************************************************
 * Exercise 2: linear_scan                           *
 *****************************************************/
/**
 * This function for a given query point q  returns the index of the point
 * in an array of points P that is closest to q using the linear scan algorithm.
 *
 * @param q the query point
 * @param dim the dimension of the space where the points live
 * @param P a set of points
 * @param n the number of points in P
 * @return the index of the point in p that is closest to q
 */
int linear_scan(point q, int dim, point* P, int n) {
    // Exercise 2
    int idx = -1;  // It will contain the index of the closest point
    double min_dist = DBL_MAX;
    for(int i=0; i<n; i++){
        double cur_dist = dist(q, *(P+i), dim);
        if(cur_dist < min_dist) idx = i, min_dist = cur_dist;
    }
    return idx;
}

/*****************************************************
 * Exercise 3: compute_median                        *
 *****************************************************/
/**
 * This function computes the median of all the c coordinates 
 * of an subarray P of n points that is P[start] .. P[end - 1]
 *
 * @param P a set of points
 * @param start the starting index
 * @param end the last index; the element P[end] is not considered
 * @return the median of the c coordinate
 */
double compute_median(point* P, int start, int end, int c) {
    // Exercise 3
    double* values = new double[end-start];
    for(int i=start; i<end; i++) values[i-start] = P[i][c];
    std::sort(values, values + end-start);
    return values[(end-start)/2];
}

/*****************************************************
 * Exercise 4: partition                             *
 *****************************************************/
/**
 * Partitions the the array P wrt to its median value along a coordinate
 *
 * @param P a set of points (an array)
 * @param start the starting index
 * @param end the last index; the element P[end] is not considered
 * @param c the coordinate that we will consider the median
 * @return the index of the median value
 */
int partition(point* P, int start, int end, int c) {
    // Exercise 4
    double m = compute_median(P, start, end, c);
    int idx = start + (end-start)/2;  // this is where we store the index of the median
    for(int j=start, i=start-1; j<end; j++){
        if(P[j][c] <  m) {
            i++, std::swap(P[i], P[j]);
            if(P[j][c] == m) idx = j;
        }
        else if(P[j][c] == m) idx = j;
    }
    std::swap(P[start+(end-start)/2], P[idx]);
    return start + (end-start)/2;
}

/*****************************************************
 * Exercise 5: create_node                           *
 *****************************************************/
/**
 * Creates a leaf node in the kd-tree
 *
 * @param val the value of the leaf node
 * @return a leaf node that contains val
 */
node* create_node(int _idx) {
  // Exercise 5
    node* novo = new node;
    novo->idx = _idx;
    novo->left = NULL;
    novo->right = NULL;
    return novo;
}

/**
 * Creates a internal node in the kd-tree
 *
 * @param idx the value of the leaf node
 * @return an internal node in the kd-tree that contains val
 */
node* create_node(int _c, double _m, int _idx,
                  node* _left, node* _right) {
    node* novo = new node;
    novo->c = _c;
    novo->m = _m;
    novo->idx = _idx;
    novo->left = _left;
    novo->right = _right;
    return novo;
}

node* build(point* P, int start, int end, int c, int dim) {
    // builds tree for sub-cloud P[start -> end-1]
    assert(end - start >= 0);
    if (debug)
        std::cout << "start=" << start << ", end=" << end << ", c=" << c
                  << std::endl;
    if (end - start == 0)  // no data point left to process
        return NULL;
    else if (end - start == 1)  // leaf node
        return create_node(start);
    else {  // internal node
        if (debug) {
            std::cout << "array:\n";
            for (int i = start; i < end; i++)
                print_point(P[i], dim);
            // std::cout << P[i] << ((i==end-1)?"\n":" ");
        }
        // compute partition
        // rearrange subarray (less-than-median first, more-than-median last)
        int p = partition(P, start, end, c);
        if (p == -1) { return NULL; }
        double m = P[p][c];
        // prepare for recursive calls
        int cc = (c + 1) % dim;  // next coordinate
        return create_node(c, m, p, build(P, start, p, cc, dim),
                           build(P, p + 1, end, cc, dim));
    }
}

/*****************************************************
 * Exercise 6: defeatist_search                      *
 *****************************************************/
/**
 *  Defeatist search in a kd-tree
 *
 * @param n the roots of the kd-tree
 * @param q the query point
 * @param dim the dimension of the points
 * @param P a pointer to an array of points
 * @param res the distance of q to its NN in P
 * @param nnp the index of the NN of q in P
 */
void defeatist_search(node* n, point q, int dim, point* P, double& res, int& nnp) {
    if (n == NULL) return;
    if(dist(q, P[n->idx], dim) < res) res = dist(q, P[n->idx], dim), nnp = n->idx;
    if(n->left != NULL || n->right != NULL)  // internal node
    defeatist_search ( (q[n->c] <= n->m) ? n->left : n->right, q, dim, P, res, nnp);
}

/*****************************************************
 * Exercise 7: backtracking_search                   *
 *****************************************************/
/**
 *  Backtracking search in a kd-tree
 *
 * @param n the roots of the kd-tree
 * @param q the query point
 * @param dim the dimension of the points
 * @param P a pointer to an array of points
 * @param res the distance of q to its NN in P
 * @param nnp the index of the NN of q in P
 */
void backtracking_search(node* n, point q, int dim, point* P, double& res, int& nnp) {
    // if(n == NULL) return;

    // if(dist(q, P[n->idx], dim) < res) res = dist(q, P[n->idx], dim), nnp = n->idx;

    // if(n->left != NULL || n->right != NULL){
    //     if(q[n->c] - res <= n->m) {
    //         backtracking_search(n->left, q, dim, P, res, nnp);
    //     }
    //     if(q[n->c] + res >= n->m) {
    //         backtracking_search(n->right, q, dim, P, res, nnp);
    //     }
    // }

    double new_res = std::min(dist(q, P[n->idx], dim), res);
    if(new_res < res) res = new_res, nnp = n->idx;

    if(n->left != NULL && q[n->c]-res <= n->m) backtracking_search(n->left, q, dim, P, res, nnp);
    if(n->right != NULL && q[n->c]+res >= n->m) backtracking_search(n->right, q, dim, P, res, nnp);
}

