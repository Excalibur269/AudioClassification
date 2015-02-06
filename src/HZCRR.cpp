/*
 * HZCRR.cpp
 *
 *  Created on: 2014年12月22日
 *      Author: zbh
 */

#include "HZCRR.h"

HZCRR::HZCRR() {
	// TODO Auto-generated constructor stub
	avZCR = 0.0;
	obrow = 0;
}

HZCRR::~HZCRR() {
	// TODO Auto-generated destructor stub
}

double HZCRR::computeHZCRR(realvec& in){
	double sum = 0.0;
	double sgn = 0.0;
	double hzcrr = 0;
	//cout <<"HZCRR obrow ="<<obrow<<endl;

	int frame = in.getCols();
	//in is ZCR!
	for (int t = 0; t < frame; t++) {
		avZCR += in(obrow, t);
	}

	avZCR = avZCR / frame;

	for (int t = 0; t < frame; t++) {
		sgn = in(obrow,t)-1.5 * avZCR;

		if (sgn > 0)
			sgn = 1;
		else
			sgn = -1;
		sum += (sgn + 1);

	}
	hzcrr = 1.0 * sum / (2 * frame);
	return hzcrr;
	//cout<<"HZCRR  "<<out(0)<<endl;
}
