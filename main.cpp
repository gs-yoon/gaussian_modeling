#include<gaussianPDF.h>
#include<iostream>
#include<fstream>
#include<stdio.h>

#define TRAIN 0


void printPDF(GaussianPDF* distribution)
{
	std::cout << "mean = " << distribution->mean << ", sigam = " << distribution->sigma << "\n";
}

void printAll(int start, int end, GaussianPDF* distribution)
{
	for (int i = 0; i < 10; i++)
		std::cout << i <<": mean = " << (distribution+i)->mean << ", sigam = " << (distribution+i)->sigma << "\n";
}

double generateGaussian(float mean, float sigma) {
	double v1, v2, s;

	do {
		v1 = 2 * ((double)rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
		v2 = 2 * ((double)rand() / RAND_MAX) - 1;      // -1.0 ~ 1.0 까지의 값
		s = v1 * v1 + v2 * v2;
	} while (s >= 1 || s == 0);

	s = sqrt((-2 * log(s)) / s);

	double value = v1* s;
	return value*sigma + mean ;
}

float mean[10] = { 2.6, 2.7, 2.8 ,2.9 , 3.0, 3.1 , 3.2 ,3.3 ,3.5 , 3.6 };
//float mean[] = { 1.,1., 1., 1., 1., 1., 1., 1., 1., 1.};
float sigma[] = { 1.,1., 5., 5., 5., 5., 5., 5., 5., 5. };
//float sigma[10] = { 1, 2, 3, 1.5, 2.5 ,3.5, 1.2 ,2.3, 3.4, 4 };

float input[10] = { 1,2,3,4,5,6,7,8,9,10 };
float input_2[10] = {1.1,2.2,3.3,4.4,5.5,6.6,7.7,8.8,9.9,11};


int main()
{
	GaussianPDF noiseModel[10];
	memset(noiseModel, 0, sizeof(struct GaussianPDF) * 10 );

	std::vector<float> errors[10];

#if TRAIN == 1
	int data_num = 1000;
	// gen pesudo data
	for ( int i =0 ; i < 10 ; i++)
		for (int j = 0; j < data_num; j++)
			errors[i].push_back(generateGaussian(mean[i], sigma[i]));
	

	for (int i = 0; i < 10; i++)
		noiseModel[i] = calcGaussianPDF(errors[i]);

	printAll(0, _countof(noiseModel), noiseModel);

	GaussianPDF D_hat = productMultipleGaussian(0, sizeof(noiseModel) / sizeof(GaussianPDF), noiseModel);

	std::ofstream writeFile;
	writeFile.open("noise_modeling.txt");
	if (writeFile.is_open()) {
		for (int i = 0; i < 10; i++)
			writeFile << noiseModel[i].mean << " " << noiseModel[i].sigma << " ";
	}

	std::cout << "D_hat : ";
	printPDF(&D_hat);

#endif

#if TRAIN==0

	//get noise model
	std::ifstream readFile;
	readFile.open("noise_modeling.txt");

	if (readFile.is_open())
	{
		for (int i = 0; i < 10; i++)
			readFile >> noiseModel[i].mean >> noiseModel[i].sigma;
	}

	printAll(0, _countof(noiseModel), noiseModel );

	GaussianPDF noise_hat = productMultipleGaussian(0, sizeof(noiseModel) / sizeof(GaussianPDF), noiseModel);
	std::cout << "noise hat : ";
	printPDF(&noise_hat);

	//get measure model
	GaussianPDF measurModel[10];
	memset(measurModel, 0, sizeof(struct GaussianPDF) * 10 );

	selcetReliablePDF(10, measurModel, input, 1);
	for (int i = 0; i < 10; i++)
		addMeasurment(measurModel + i, noiseModel + i, input[i]);
	
	for (int i = 0; i < 10; i++)
		std::cout << "unRel = " << measurModel[i].unReliable << " ";
	std::cout << std::endl;

	GaussianPDF D_hat = productMultipleGaussian(0, sizeof(measurModel) / sizeof(GaussianPDF), measurModel);
	std::cout << "D_hat : ";
	printPDF(&D_hat);

	//update measrue model
	updatePDF(0, sizeof(measurModel) / sizeof(GaussianPDF), measurModel, &D_hat);

	printAll(0, _countof(measurModel), measurModel);


	//get measure model2

	selcetReliablePDF(10, measurModel, input_2, 1);
	for (int i = 0; i < 10; i++)
		addMeasurment(measurModel + i, noiseModel + i, input_2[i]);

	for (int i = 0; i < 10; i++)
		std::cout << "unRel = " << measurModel[i].unReliable << " ";
	std::cout << std::endl;

	D_hat = productMultipleGaussian(0, sizeof(measurModel) / sizeof(GaussianPDF), measurModel);
	std::cout << "D_hat : ";
	printPDF(&D_hat);

	//update measrue model
	updatePDF(0, sizeof(measurModel) / sizeof(GaussianPDF), measurModel, &D_hat);

	printAll(0, _countof(measurModel), measurModel);


#endif
	//std::cout << "\n";
	return 0;
}


// TODO
// 모델에 측정값 추가 ( 평균값 += 측정값 , 시그마는 그대로)
//  
// 몇 시그마인지 계산
// 시그마의 분포 계산
// 시그마에서 1?시그마 내의 값만 측정에 사용