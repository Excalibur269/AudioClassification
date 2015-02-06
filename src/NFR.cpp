/*
 * NFC.cpp
 *
 *  Created on: 2014年12月22日
 *      Author: zbh
 */
#include "NFR.h"

using namespace std;

NFR::NFR() {
	this->threshold = 0.003;
	this->obrow = 0;
}
NFR::NFR(const NFR& a){
}

NFR::~NFR(){
}

double NFR::computeNFR(realvec& in) {
	int num = 0;
	int memSize = 30;
	double NFR = 0;
	int inSamples_ = in.getRows();
	for (int t = 0; t < inSamples_; t++) {
		if (in(t, obrow) < threshold)
			num++;
	}

	NFR = (double) (num / memSize);
	return NFR;
}

