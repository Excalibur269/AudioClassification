/*
 * HZCRR.h
 *
 *  Created on: 2014年12月22日
 *      Author: zbh
 */

#ifndef INCLUDE_HZCRR_H_
#define INCLUDE_HZCRR_H_
#include <iostream>
#include "realvec.h"
class HZCRR {
public:
	HZCRR();
	~HZCRR();
	double computeHZCRR(realvec& in);
private:
	double avZCR;
	int obrow;
};

#endif /* INCLUDE_HZCRR_H_ */
