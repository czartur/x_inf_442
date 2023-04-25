#include "ConfusionMatrix.hpp"
#include <iostream>

using namespace std;

ConfusionMatrix::ConfusionMatrix() {
    // TODO Exercise 3
    for(int i=0; i<2; ++i)
        for(int j=0; j<2; ++j)
            m_confusion_matrix[i][j] = 0;
}

ConfusionMatrix::~ConfusionMatrix() {
    // Attribute m_confusion_matrix is deleted automatically
}

void ConfusionMatrix::AddPrediction(int true_label, int predicted_label) {
    // TODO Exercise 3
    ++m_confusion_matrix[true_label][predicted_label];
}

void ConfusionMatrix::PrintEvaluation() const{
    // Prints the confusion matrix
    cout <<"\t\tPredicted\n";
    cout <<"\t\t0\t1\n";
    cout <<"Actual\t0\t"
        <<GetTN() <<"\t"
        <<GetFP() <<endl;
    cout <<"\t1\t"
        <<GetFN() <<"\t"
        <<GetTP() <<endl <<endl;
    // Prints the estimators
    cout <<"Error rate\t\t"
        <<error_rate() <<endl;
    cout <<"False alarm rate\t"
        <<false_alarm_rate() <<endl;
    cout <<"Detection rate\t\t"
        <<detection_rate() <<endl;
    cout <<"F-score\t\t\t"
        <<f_score() <<endl;
    cout <<"Precision\t\t"
        <<precision() <<endl;
}

int ConfusionMatrix::GetTP() const {
    return m_confusion_matrix[1][1];
}

int ConfusionMatrix::GetTN() const {
   return m_confusion_matrix[0][0];
}

int ConfusionMatrix::GetFP() const {
    return m_confusion_matrix[0][1];
}

int ConfusionMatrix::GetFN() const {
   return m_confusion_matrix[1][0];
}

double ConfusionMatrix::f_score() const {
    // TODO Exercise 3
    return 2*precision()*detection_rate()/(precision() + detection_rate());
}

double ConfusionMatrix::precision() const {
    // TODO Exercise 3
    double TP = m_confusion_matrix[0][0];
    double FP = m_confusion_matrix[1][0];
    return TP/(TP+FP);
}

double ConfusionMatrix::error_rate() const {
    // TODO Exercise 3
    double TP = m_confusion_matrix[0][0];
    double FP = m_confusion_matrix[1][0];
    double TN = m_confusion_matrix[1][1];
    double FN = m_confusion_matrix[0][1];
    return (FP + FN)/(FP + FN + TP + TN);
}

double ConfusionMatrix::detection_rate() const {
    // TODO Exercise 3
    double TP = m_confusion_matrix[0][0];
    double FN = m_confusion_matrix[0][1];
    return TP/(TP + FN);
}

double ConfusionMatrix::false_alarm_rate() const {
    // TODO Exercise 3
    double FP = m_confusion_matrix[1][0];
    double TN = m_confusion_matrix[1][1];
    return FP/(FP + TN);
}
