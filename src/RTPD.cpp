/*
 * RTPD.cpp
 *
 *  Created on: 2015年1月15日
 *      Author: zhangbihong
 */

#include "RTPD.h"
using namespace std;

RTPD::RTPD() {
	// TODO Auto-generated constructor stub

}

RTPD::~RTPD() {
	// TODO Auto-generated destructor stub
}

//in为RMS
double RTPD::computeRTPD(realvec& in, realvec& out) {
	long tonal_num = 0, all_num = 0;
	double rms_tonal = 0.0, rms_all = 0.0;
	long memSize = 40;

	int inSamples_ = in.getCols() / 2; //帧数的一半
	double RTPD = 0.0;
	obrow = 0;
	threshold = 0.14;

	// RCforRTPD的输出是第一列为rc的最大值，第二行为rms
	for (int t = 0; t < inSamples_; t += 2) {
		if (in(obrow, t) > threshold) {
			rms_tonal += in(obrow, t + 1);
			tonal_num++;
		}
		rms_all += in(obrow, t + 1);
		all_num++;
	}

	if (tonal_num == 0 || rms_all == 0)
		RTPD = 0;
	else
		RTPD = (double)(rms_tonal / tonal_num) / (rms_all / memSize);
	RTPD /= 4.0;
	return RTPD;
}
