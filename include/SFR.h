/*
 ** Author:Luo Yong
 ** Date:4/10/2008
 */

#ifndef MARSYAS_SFR_H
#define MARSYAS_SFR_H

#include <iostream>
#include "realvec.h"
#include "common.h"
/**
 \class Noise frame ratio
 \brief Time-domain
 SFR is defined as the ratio of the number of noise frame
 whose maximum local peak of its normalized correlation function is lower than a preset threshold.
 */

class SFR{
private:
	double threshold;
	int obrow;

public:

	SFR();
	SFR::SFR(const SFR& a);

	~SFR();

	double computeSFR(realvec& in);
};

#endif

