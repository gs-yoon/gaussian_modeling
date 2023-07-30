#include<gaussianPDF.h>
#include<math.h>
#include<iostream>


double calcGaussianMean(const std::vector<float> &errors)
{
	double sum = 0;
	double mean = 0;
	for (int i = 0; i < errors.size(); i++)
		sum += errors[i];
	mean = sum / errors.size();
	return mean;
}

double calcGaussianSigma(double mean, const std::vector<float> &errors)
{
	double sum=0;
	double sigma = 0;
	for (int i = 0; i < errors.size(); i++)
	{
		double diff = mean - errors[i];
		sum += diff * diff;
	}
	sigma = sqrt(sum / (float)errors.size());
	return sigma;
}

GaussianPDF calcGaussianPDF(const std::vector<float>& errors)
{
	GaussianPDF result;
	result.mean = calcGaussianMean(errors);
	result.sigma = calcGaussianSigma(result.mean, errors);
	return result;
}

double getSigmaInPDF(GaussianPDF* distribution, double input)
{
	if (distribution->sigma != 0)
		return (input - distribution->mean) / distribution->sigma;
	else
		return 0;
}

GaussianPDF productMultipleGaussian(int start, int end, GaussianPDF* distribution)
{
	int sz = sizeof(GaussianPDF);
	double sum = 0;
	GaussianPDF* cur;

	double sigma_hat = 0;
	for (int i = start; i < end; i++)
	{
		if ( (distribution+i)->inValid + (distribution+i)->unReliable == true)
			continue;
		cur = distribution + i;
		sum += 1 / (cur->sigma * cur->sigma);
	}
	sigma_hat = sqrt(1 / sum);

	double mean_hat = 0;
	sum = 0;
	for (int i = start; i < end; i++)
	{
		if ((distribution + i)->inValid + (distribution + i)->unReliable == true)
			continue;
		cur = distribution + i;
		sum += cur->mean / (cur->sigma * cur->sigma);
	}
	mean_hat = sigma_hat * sigma_hat * sum;

	GaussianPDF result{mean_hat, sigma_hat};
	return result;
}

void addMeasurment(GaussianPDF* measure, GaussianPDF* noise, double measurement)
{
	measure->mean = noise->mean + measurement;
	measure->sigma = noise->sigma;
}

void updatePDF(int start, int end, GaussianPDF* distribution, const GaussianPDF* D_hat)
{
	for(int i = start ; i < end ; i++)
		(distribution + i)->mean = D_hat->mean;
}


void selcetReliablePDF(int start, int end, GaussianPDF* measurePDF, float* input, float sigmaBoundary)
{
	//기존 분포에서, 입력이 몇 시그마에 해당되는 지 찾기
	std::vector<float> inSigmaVec;
	int num = end - start;
	for (int i = start; i < end; i++)
	{
			inSigmaVec.push_back(getSigmaInPDF(measurePDF + i, *(input + i)));
	}
	//시그마를 가우시안 분포로 만들기
	GaussianPDF sigmaPDF = calcGaussianPDF(inSigmaVec);

	if (inSigmaVec.size() != num)
	{
		std::cout << "selcetReliablePDF Err \n";
		return;
	}

	//시그마의 분포에서, 시그마바운더리 내에 오는 분포만 enable하기
	for (int i = 0; i < num; i++)
	{
		float sigma_dash = 0;
		if(sigmaPDF.sigma != 0)
			sigma_dash = sigmaPDF.mean - inSigmaVec[i] / sigmaPDF.sigma;

		if (abs(sigma_dash) <= sigmaBoundary )
			(measurePDF + i + start)->unReliable = 0;
		else
			(measurePDF + i + start)->unReliable = 1;
	}
}

void selcetValidNoisePDF(int num, GaussianPDF* noise, float sigmaBoundary, bool* outEnableTable)
{
	//노이즈 분포에서, 시그마바운더리 내에 오는 분포만 enable하기
	for (int i = 0; i < num; i++)
	{
		if ((noise + i)->sigma <= sigmaBoundary)
			(noise + i)->inValid = 0;
		else
			(noise + i)->inValid = 1;
	}
}