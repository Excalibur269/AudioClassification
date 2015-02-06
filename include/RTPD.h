/*
 * RTPD.h
 *
 *  Created on: 2015年1月15日
 *      Author: zhangbihong
 */

#ifndef INCLUDE_RTPD_H_
#define INCLUDE_RTPD_H_
#include <iostream>
#include "realvec.h"
/************************************************************
	RPTD是依APD（帧输出）为输入的段特征，对于段的计算不再只是求均
	值和方差，而是定义tonal-frame（有调帧）和non-tonal-frame（无调
	帧）两种帧类型，然后计算有调能量帧比率
*************************************************************/

class RTPD {
private:
	double threshold;
	int obrow;
public:
	RTPD();
	virtual ~RTPD();
	double computeRTPD(realvec& in, realvec& out);
};

#endif /* INCLUDE_RTPD_H_ */
