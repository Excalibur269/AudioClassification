/*
 * APD.cpp
 *
 *      Author: zhangbihong
 */

#include "APD.h"
using namespace std;

APD::APD(string name) {
	re_ = 0.0;
	im_ = 0.0;

//	addControls();
}

APD::APD(const APD& a) {
}

APD::~APD() {
}

void APD::getAPD(realvec& in, realvec& out) {
	// 求FFT
	tempVec_.create(in.getSize());
	rc_.create(in.getSize()/2);
	int inSamples_ = in.getCols(); //帧数
	for (int t = 0; t < inSamples_; t++) {
		tempVec_(t) = in(0, t);
	}
	double *tempData;
	tempData = (double *) malloc(sizeof(double) * 2 * inSamples_);
	for (int i = 0; i < inSamples_; i++) {
		tempData[2 * i] = tempVec_(i);
		tempData[2 * i + 1] = 0.0;
	}
	myfft_.fft(tempData, inSamples_);
	//myfft_.ifft(tempData,inSamples_);
	//myfft_.fft(tempData,inSamples_);

	// 取一半求模值取对数,求逆FFT
	double *tempData2;
	tempData2 = (double *) malloc(sizeof(double) * 2 * inSamples_);
	for (int t = 0; t < inSamples_; ++t) {
		re_ = tempData[2 * t];
		im_ = tempData[2 * t + 1];
		if (re_ != 0.0 || im_ != 0.0)
			tempData2[2 * t] = log(sqrt(re_ * re_ + im_ * im_));
		else
			tempData2[2 * t] = 0.0;
		tempData2[2 * t + 1] = 0.0;
	}
	myfft_.ifft(tempData2, inSamples_);

	// 取实部
	for (int t = 0; t < N2_; t++)
		rc_(t) = tempData2[2 * t];

	// 求PD（按帧决策）
	double sum = 0.0, pd = 0.0;
	int L1, L2;
	L1 = 28;
	L2 = 256;	//高维系数
	for (int i = L1; i < L2; i++) {
		sum += abs(rc_(i));
		//cout<<"rc\t"<<abs(rc_(i))<<endl;
	}
	pd = sum / (L2 - L1);
	out(0) = sum;
	//cout<<pd<<endl;

	free(tempData);
	free(tempData2);
}
