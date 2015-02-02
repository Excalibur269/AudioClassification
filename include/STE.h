
/*
** Author:Li Zhonghua
** Date:4/05/2008
*/
#ifndef MARSYAS_STE_H
#define MARSYAS_STE_H

#include <iostream>
#include <math.h>


class ShortTimeEnergy
{
private:
	//mrs_real threshold_;
//	void addControls();
//	void myUpdate(MarControlPtr sender);
//	MarControlPtr ctrl_threshold_;
//	MarControlPtr ctrl_notEmpty_;

public:
	ShortTimeEnergy();
//	ShortTimeEnergy(const ShortTimeEnergy&);
	~ShortTimeEnergy();
//	MarSystem * clone() const;
	void computeSTE(realvec& in, realvec& out);

};//
	

#endif

