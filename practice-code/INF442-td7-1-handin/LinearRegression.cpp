#include "LinearRegression.hpp"
#include "Dataset.hpp"
#include "Regression.hpp"
#include<iostream>
#include<cassert>


LinearRegression::LinearRegression( Dataset* dataset, int col_regr ) 
: Regression(dataset, col_regr) {
	m_beta = NULL;
	SetCoefficients();
}

LinearRegression::~LinearRegression() {
	if (m_beta != NULL) {
		m_beta->resize(0);
		delete m_beta;
	}
}


Eigen::MatrixXd LinearRegression::ConstructMatrix() {
	// TODO Exercise 1

	int n = m_dataset->GetNbrSamples();
	int d = m_dataset->GetDim();
	Eigen::MatrixXd X(n, d);
	for(int i = 0; i < n; ++i){
		auto &row = m_dataset->GetInstance(i);
		X(i, 0) = 1;
		for(int j = 0; j < d; ++j){
			if(j < m_col_regr) X(i, j+1) = row[j];
			else if(j > m_col_regr) X(i, j) = row[j];
		}
	}
	return X;
}

Eigen::VectorXd LinearRegression::ConstructY() {
	// TODO Exercise 1

	int n = m_dataset->GetNbrSamples();
	Eigen::VectorXd y(n);
	for(int i = 0; i < n; ++i){
		auto &row = m_dataset->GetInstance(i);
		y(i) = row[m_col_regr];
	}
	return y;
}

void LinearRegression::SetCoefficients() {
	// TODO Exercise 2

	auto X = ConstructMatrix(); 
	auto y = ConstructY();

	auto A = X.transpose()*X;
	auto b = X.transpose()*y; 

	// solving linear system
	m_beta = new Eigen::VectorXd(A.colPivHouseholderQr().solve(b));
}

const Eigen::VectorXd* LinearRegression::GetCoefficients() const {
	if (!m_beta) {
		std::cout<<"Coefficients have not been allocated."<<std::endl;
		return NULL;
	}
	return m_beta;
}

void LinearRegression::ShowCoefficients() const {
	if (!m_beta) {
		std::cout<<"Coefficients have not been allocated."<<std::endl;
		return;
	}
	
	if (m_beta->size() != m_dataset->GetDim()) {  // ( beta_0 beta_1 ... beta_{d} )
		std::cout<< "Warning, unexpected size of coefficients vector: " << m_beta->size() << std::endl;
	}
	
	std::cout<< "beta = (";
	for (int i=0; i<m_beta->size(); i++) {
		std::cout<< " " << (*m_beta)[i];
	}
	std::cout << " )"<<std::endl;
}

void LinearRegression::PrintRawCoefficients() const {
	std::cout<< "{ ";
	for (int i=0; i<m_beta->size()-1; i++) {
		std::cout<< (*m_beta)[i]  << ", ";
	}
	std::cout << (*m_beta)[m_beta->size()-1];
	std::cout << " }"<<std::endl;
}

void LinearRegression::SumsOfSquares( Dataset* dataset, double& ess, double& rss, double& tss ) const {
	assert(dataset->GetDim()==m_dataset->GetDim());
	// TODO Exercise 4

	int m = dataset->GetNbrSamples();
	int d = dataset->GetDim();

	LinearRegression linreg(dataset, m_col_regr);
	auto X_test = linreg.ConstructMatrix();
	auto y_test = linreg.ConstructY();

	auto& beta = *m_beta;
	auto y_pred = X_test*beta;

	auto one_vec = Eigen::VectorXd::Ones(m);
	tss = (y_test - y_test.mean()*one_vec).squaredNorm();
	ess = (y_pred - y_test.mean()*one_vec).squaredNorm();
	rss = (y_test - y_pred).squaredNorm();
}

double LinearRegression::Estimate( const Eigen::VectorXd & x ) const {
	// TODO Exercise 3

	auto& beta = *m_beta;
	return beta(0) + x.transpose()*beta.tail(beta.size()-1);
}
