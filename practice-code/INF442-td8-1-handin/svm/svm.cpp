#include <iostream>
#include <string>
#include <vector>
#include <cmath>
#include <numeric>

#include "kernel.hpp"
#include "confusion_matrix.hpp"
#include "svm.hpp"

SVM::SVM(Dataset* dataset, int col_class, Kernel K):
    col_class(col_class), kernel(K) {
    // TODO Exercise 2: put the correct columns of dataset in train_labels and _features AGAIN!
    // BEWARE: transform 0/1 labels to -1/1

    int n = dataset->GetNbrSamples();
    int d = dataset->GetDim();
    train_labels = std::vector<int>(dataset->GetNbrSamples());
    train_features = std::vector<std::vector<double>>(n, std::vector<double>(d-1));
    for(int i = 0; i < n; ++i){
        auto line = dataset->GetInstance(i);
        auto& row = train_features[i];
        auto& label = train_labels[i];
        for(int j = 0; j < d; ++j){
            if(j == col_class) label = 2*line[j] - 1;
            else row[j - (j > col_class)] = line[j];
        }
    }
    compute_kernel();
}

SVM::~SVM() {
}

void SVM::compute_kernel() {

    // TODO Exercise 2: put y_i y_j k(x_i, x_j) in the (i, j)th coordinate of computed_kernel
    const int n = train_features.size();
    alpha = std::vector<double>(n);
    computed_kernel = std::vector<std::vector<double>>(n, std::vector<double>(n));
    for(int i = 0; i < n; ++i){
        int &yi = train_labels[i];
        auto &xi = train_features[i];
        for(int j = 0; j < n; ++j){
            int &yj = train_labels[j];
            auto &xj = train_features[j];
            computed_kernel[i][j] = (double) yi*yj*kernel.k(xi, xj);
        }
    }
}

void SVM::compute_beta_0(double C) {
    // count keeps track of the number of support vectors (denoted by n_s)
    int count = 0;
    beta_0 = 0.0;
    // TODO Exercise 3
    // Use clipping_epsilon < alpha < C - clipping_epsilon instead of 0 < alpha < C
    // This performs 1/n_s
    const int n = train_features.size();
    for(int s = 0; s < n; ++s){
        if(!(clipping_epsilon < alpha[s] && alpha[s] < C-clipping_epsilon)) continue;
        ++count;
        int &ys = train_labels[s];
        auto &xs = train_features[s];
        for(int i = 0; i < n; ++i){
            int &yi = train_labels[i];
            auto &xi = train_features[i];
            beta_0 += alpha[i]*yi*kernel.k(xi, xs);
        }
        beta_0 -= ys;
    }
    beta_0 /= count;
}

void SVM::train(const double C, const double lr) {
    // TODO Exercise 4
    // Perform projected gradient ascent
    // While some alpha is not clipped AND its gradient is above stopping_criterion
    // (1) Set stop = false
    // (2) While not stop do
    // (2.1) Set stop = true
    // (2.2) For i = 1 to n do
    // (2.2.1) Compute the gradient - HINT: make good use of computed_kernel
    // (2.2.2) Move alpha in the direction of the gradient - eta corresponds to lr (for learning rate)
    // (2.2.3) Project alpha in the constraint box by clipping it
    // (2.2.4) Adjust stop if necessary
    // (3) Compute beta_0

    // Update beta_0
    const int n = train_features.size();
    bool stop = false;
    while(!stop){
        stop = true;
        for(int i = 0; i < n; ++i){
            double gradient = 1.0;
            auto &xi = train_features[i];
            int &yi = train_labels[i];
            for(int j = 0; j < n; ++j){
                auto &xj = train_features[i];
                int &yj = train_labels[i];
                gradient -= alpha[j]*computed_kernel[i][j];
            }
            alpha[i] += lr*gradient;
            // if(clipping_epsilon < alpha[i] 
            //     && alpha[i] < C-clipping_epsilon
            //     && abs(gradient) > stopping_criterion) stop = false;
            if(0.0 < alpha[i] 
                && alpha[i] < C
                && std::abs(gradient) > stopping_criterion) stop = false;
            alpha[i] = std::max(0.0, std::min(C, alpha[i]));
        }
    }
    compute_beta_0(C);
}

int SVM::f_hat(const std::vector<double> x) {
    // TODO Exercise 5
    const int n = train_features.size();
    double tmp = -beta_0;
    for(int i = 0; i < n; ++i){
        int &yi = train_labels[i];
        auto &xi = train_features[i];
        tmp += alpha[i]*yi*kernel.k(xi, x);
    }
    return (tmp > 0) - (tmp < 0);
}

ConfusionMatrix SVM::test(const Dataset* test) {
    // Collect test_features and test_labels and compute confusion matrix
    std::vector<double> test_labels (test->GetNbrSamples());
    std::vector<std::vector<double>> test_features (test->GetNbrSamples(), std::vector<double>(test->GetDim() - 1));
    ConfusionMatrix cm;

    // TODO Exercise 6
    // Put test dataset in features and labels
    // Use f_hat to predict and put into the confusion matrix
    // BEWARE: transform -1/1 prediction to 0/1 label

    int n = test->GetNbrSamples();
    int d = test->GetDim();
    for(int i = 0; i < n; ++i){
        auto line = test->GetInstance(i);
        auto& row = test_features[i];
        for(int j = 0; j < d; ++j){
            if(j == col_class) test_labels[i] = line[j];
            row[j - (j > col_class)] = line[j];
        }
    }

    for(int i = 0; i < n; ++i){
        auto &x = test_features[i];
        int y = test_labels[i];
        int pred = f_hat(x);
        cm.AddPrediction(y, pred > 0); 
    }
    
    return cm;
}

int SVM::get_col_class() const {
    return col_class;
}

Kernel SVM::get_kernel() const {
    return kernel;
}

std::vector<int> SVM::get_train_labels() const {
    return train_labels;
}

std::vector<std::vector<double>> SVM::get_train_features() const {
    return train_features;
}

std::vector<std::vector<double>> SVM::get_computed_kernel() const {
    return computed_kernel;
}

std::vector<double> SVM::get_alphas() const {
    return alpha;
}

double SVM::get_beta_0() const {
    return beta_0;
}

void SVM::set_alphas(std::vector<double> alpha) {
    this->alpha = alpha;
}
