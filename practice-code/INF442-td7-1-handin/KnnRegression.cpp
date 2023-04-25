
#include "KnnRegression.hpp"
#include<iostream>
#include <ANN/ANN.h>


KnnRegression::KnnRegression(int k, Dataset* dataset, int col_regr)
: Regression(dataset, col_regr) {
	m_k = k;
 	// TODO Exercise 5

	int d = dataset->GetDim();
    int n = dataset->GetNbrSamples();
    m_dataPts = new ANNpoint[n];

    for(int i=0; i<n; ++i){
        std::vector<double> instance = dataset->GetInstance(i);
        m_dataPts[i] = new double[d-1];
        for(int j=0, c=0; j<instance.size(); ++j){
            if(j == col_regr) continue;
            m_dataPts[i][c++] = instance[j];
        }
    }

    m_kdTree = new ANNkd_tree(m_dataPts, n, d-1);
}

KnnRegression::~KnnRegression() {
	// TODO Exercise 5

	delete[] m_dataPts;
    delete m_kdTree;
}

double KnnRegression::Estimate(const Eigen::VectorXd & x) const {
	assert(x.size()==m_dataset->GetDim()-1);
	// TODO Exercise 6

	int m = x.size();
	int k = GetK();
    ANNidxArray nn_idx = new ANNidx[k];
    ANNdistArray dd = new ANNdist[k];

	ANNpoint x_ann = new double[m];
	for(int i = 0; i < m; ++i) x_ann[i] = x(i);
    m_kdTree->annkSearch(x_ann, k, nn_idx, dd);
    
    double pred = 0;
    for(int i=0; i<k; ++i) 
        pred += m_dataset->GetInstance(nn_idx[i])[m_col_regr];
    pred/=k;
    return pred;

}

int KnnRegression::GetK() const {
	return m_k;
}

ANNkd_tree* KnnRegression::GetKdTree() const {
	return m_kdTree;
}
