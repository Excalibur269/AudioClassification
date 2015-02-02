/*
 * ReadFeature.h
 *
 *  Created on: 2015年1月30日
 *      Author: zhangbihong
 */

#ifndef READFEATURE_H_
#define READFEATURE_H_

#include "common.h"
#include "realvec.h"
#include <string>
#include <iostream>

void readMFCC(std::string filename, realvec& in);
void readLSP(std::string filename, realvec& in);
void readLPCC(std::string filename, realvec& in);
void readZCR(std::string filename, realvec& in);


#endif /* READFEATURE_H_ */
