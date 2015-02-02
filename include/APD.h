/*
 * APD.h
 *
 *  Created on: 2015骞�1鏈�15鏃�
 *      Author: zhangbihong
 */

#ifndef INCLUDE_APD_H_
#define INCLUDE_APD_H_

//#include "alize.h"
#include <iostream>
#include <string>
#include <math.h>
#include "realvec.h"
class APD {
private:
	int N2_;
	double re_, im_;

	MyFFT myfft_;
	realvec tempVec_;
	realvec rc_;

//	void myUpdate(MarControlPtr sender);

public:
	APD(std::string name);
	APD(const APD&);
	~APD();
//	MarSystem* clone() const;

	void getAPD(realvec& in, realvec& out);

};

#endif /* INCLUDE_APD_H_ */
