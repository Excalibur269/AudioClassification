/*
 ** Author:Luo Yong
 ** Date:4/10/2008
 */

#include "SFR.h"

using namespace std;

SFR::SFR() {

}

SFR::SFR(const SFR& a){

}

SFR::~SFR() {
}

double SFR::computeSFR(realvec& in) {
	//checkFlow(in,out);

	int num = 0;
	int memSize = 30;

	obrow = 0;
	threshold = 0.01;

	//for (t = 0; t < inSamples_; t+=2)
	for (int t = 0; t < in.getCols(); t++) {
		if (in(obrow, t) < threshold)
			num++;

	}

	return (double) (num / memSize);
	//cout<<"SFR  "<<out(0)<<endl;
}
