/*
 * LSTER.cpp
 *
 *  Created on: 2014年12月22日
 *      Author: zbh
 */

#include "LSTER.h"

using namespace std;
LSTER::LSTER() {
	// TODO Auto-generated constructor stub
	av = 0.0;
	obrow = 0;
}

LSTER::~LSTER() {
	// TODO Auto-generated destructor stub
}

double LSTER::computeLSTER(realvec& in) {
	double sum = 0.0;
	double sgn = 0.0;
	double lster = 0;

	int inSamples_ = in.getRows();
	for (int t = 0; t < inSamples_; t++) {
//		av += in(obrow, t);
		av += in(t, obrow);
	}

	av = av / inSamples_;

	for (int t = 0; t < inSamples_; t++) {
//		sgn =  0.5 * av-in(obrow,t);
		sgn =  0.5 * av - in(t, obrow);

		if (sgn > 0)
			sgn = 1;
		else
			sgn = -1;
		sum += (sgn + 1);

	}
	lster = 1.0 * sum / (2 * inSamples_);
	return lster;
	//cout<<"HZCRR  "<<out(0)<<endl;

//	sum = 0.0;
}
