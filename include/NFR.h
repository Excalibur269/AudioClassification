/*
 * NFC.h
 *
 *  Created on: 2014年12月22日
 *      Author: zbh
 */

#ifndef INCLUDE_NFRH_
#define INCLUDE_NFRH_

#include <iostream>
#include "realvec.h"

/**
	\class Noise frame ratio
	\brief Time-domain
	SFR is defined as the ratio of the number of noise frame
	whose maximum local peak of its normalized correlation function is lower than a preset threshold.
*/
class NFR{
private:
	double threshold;
	int obrow;
public:
	NFR();
	NFR(const NFR& nfr);

	~NFR();
	double computeNFR(realvec& in);
};




#endif /* INCLUDE_NFRH_ */
