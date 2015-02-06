/*
 * LSTER.h
 *
 *  Created on: 2014年12月22日
 *      Author: zbh
 */

#ifndef INCLUDE_LSTER_H_
#define INCLUDE_LSTER_H_
#include <iostream>
#include "realvec.h"
class LSTER {
public:
	LSTER();
	~LSTER();
	double computeLSTER(realvec& in);
private:
	double av;
	int obrow;
};

#endif /* INCLUDE_LSTER_H_ */
