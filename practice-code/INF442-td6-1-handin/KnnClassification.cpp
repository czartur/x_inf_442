
#include "KnnClassification.hpp"
#include <iostream>
#include <ANN/ANN.h>


KnnClassification::KnnClassification(int k, Dataset *dataset, int col_class)
: Classification(dataset, col_class), m_k{k} {
    // TODO Exercise 1
    int d = dataset->getDim();
    int n = dataset->getNbrSamples();
    m_dataPts = new ANNpoint[n];

    for(int i=0; i<n; ++i){
        std::vector<double> instance = dataset->getInstance(i);
        m_dataPts[i] = new double[d-1];
        for(int j=0, c=0; j<instance.size(); ++j){
            if(j == col_class) continue;
            m_dataPts[i][c++] = instance[j];
        }
    }

    m_kdTree = new ANNkd_tree(m_dataPts, n, d-1);
}

KnnClassification::~KnnClassification() {
    // TODO Exercise 1
    delete[] m_dataPts;
    delete m_kdTree;
}

int KnnClassification::Estimate(const ANNpoint &x, double threshold) const {
    // TODO Exercise 2
    int k = getK();
    ANNidxArray nn_idx = new ANNidx[k];
    ANNdistArray dd = new ANNdist[k];
    m_kdTree->annkSearch(x, k, nn_idx, dd);
    
    double proba = 0;
    for(int i=0; i<k; ++i) 
        proba += m_dataset->getInstance(nn_idx[i])[m_col_class];
    proba/=k;
    return proba > threshold ? 1 : 0;
}

int KnnClassification::getK() const {
    return m_k;
}

ANNkd_tree *KnnClassification::getKdTree() {
    return m_kdTree;
}
