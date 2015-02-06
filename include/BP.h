/*
 * BP.h
 *
 *  Created on: 2014年12月22日
 *      Author: zbh
 */

#ifndef INCLUDE_BP_H_
#define INCLUDE_BP_H_
#include <math.h>
#include <iostream>
#include "realvec.h"
#include "Spectrum.h"

class BP {
public:
	BP();
	~BP();
	void computeBP(realvec& in, realvec& out);
private:
	void Rxy(realvec &x,int N,realvec &y,int M,realvec &rxy);
};

#endif /* INCLUDE_BP_H_ */
