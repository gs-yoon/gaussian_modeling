#pragma once
#include<vector>

struct GaussianPDF
{
	double mean;
	double sigma;
	int inValid;
	int unReliable;
};

double calcGaussianMean(const std::vector<float>&);
double calcGaussianSigma(double mean, const std::vector<float>&);
GaussianPDF productMultipleGaussian(int start, int end, GaussianPDF* distribution);
//GaussianPDF productMultipleGaussian(int start, int end, GaussianPDF* distribution, bool* enableTable);
void addMeasurment(GaussianPDF* measure, GaussianPDF* noise, double measurement);
GaussianPDF calcGaussianPDF(const std::vector<float>& errors);
double getSigmaInPDF(GaussianPDF* distribution, double input);
void updatePDF(int start, int end, GaussianPDF* distribution, const GaussianPDF* D_hat);
void selcetReliablePDF(int start, int end, GaussianPDF* measurePDF, float* input, float sigmaBoundary);
void selcetValidNoisePDF(int num, GaussianPDF* noise, float sigmaBoundary, bool* outEnableTable);
