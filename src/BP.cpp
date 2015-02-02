/*
 * BP.cpp
 *
 *  Created on: 2014年12月22日
 *      Author: zbh
 */

#include "BP.h"

using namespace std;

BP::BP() {
	// TODO Auto-generated constructor stub

}

BP::~BP() {
	// TODO Auto-generated destructor stub
}

void BP::computeBP(realvec& in, realvec& out) {

	Spectrum spectrum;
	realvec outSpectrum;
	outSpectrum.create(in.getSize());
	outSpectrum.setval(0);
	realvec tempIn;
	realvec tempOut;
	int length = in.getSize();
	int count = length / 256;
	if((length % 256) != 0)
		count = count + 1;
	outSpectrum.create(count * 256);
	outSpectrum.setval(0);
	for(int i = 0;i < count;i++){
		if(i == count - 1){
			for(int j = 256 * i; j < in.getSize();j++)
				tempIn(j - 256 * i) = in(j);
		}else{
			tempIn = in.getSubVector(256 * i, 256);
			tempOut.create(256);
			spectrum.computeSpectrum(tempIn, tempOut);
			outSpectrum.appendRealvec(tempOut);
		}
	}

	//?
//	long N = (in.getRows() - 1) * 2;
	long N = 16000;

	// 分段求自相关函数及峰值
	for (int i = 0; i < 4; i++) {		//(4个子频带)
		int LowIndex, HighIndex;
		switch (i) {
			case 0: {
				LowIndex = N / 16;
				HighIndex = N / 8;
				break;
			}
			case 1: {
				LowIndex = N / 8;
				HighIndex = N / 4;
				break;
			}
			case 2: {
				LowIndex = N / 4;
				HighIndex = 3 * N / 8;
				break;
			}
			case 3: {
				LowIndex = 3 * N / 8;
				HighIndex = N / 2;
				break;
			}
		}
		realvec tmp_fftabs;
		tmp_fftabs.create(HighIndex - LowIndex);		// 临时存放傅氏变换模平方
		realvec tmp_rss;
		tmp_rss.create(2 * (HighIndex - LowIndex) - 1);	// 自相关序列

		for (int j = LowIndex; j < HighIndex; j++)
			tmp_fftabs(j - LowIndex) = tempOut(j, 0);
//			tmp_fftabs(j - LowIndex) = in(j, 0);
		// 求自相关序列
		Rxy(tmp_fftabs, (HighIndex - LowIndex), tmp_fftabs,
				(HighIndex - LowIndex), tmp_rss);
		// 求峰值
		double peak = 0.0;
		for (int k = 0; k < (2 * (HighIndex - LowIndex) - 1); k++) {
			if (tmp_rss(k) > peak)
				peak = tmp_rss(k);
		}
		out(i, 0) = peak;
		//cout<<peak<<endl;
	}
}

////////////////////////////////////////////////////////////
///********************自相关序列************************///
////////////////////////////////////////////////////////////
void BP::Rxy(realvec &x, int N, realvec &y, int M, realvec &rxy) {

	/* Rxy()计算两个序列的互相关函数，返回值为结果序列的0点位置，注意0点位置是从0开始的，不是从1开始。*/
	/*  x[] 存放x序列，y[] 存放y序列，rxy[]存放结果序列，其中rxy[]长度为x序列长度+y序列长度-1 */
	/*  N 是x序列长度， M是y序列长度 */

	int t;
	int n;
	int indexOriginal = 0; // 原点位置
	int indexCount = 0;
	for (t = 1 - M; t < N; t++) {
		if (t == 0)
			indexOriginal = indexCount;
		indexCount++;
		rxy(t + M - 1) = 0.0;
		for (n = 0; n < N; n++) {
			if (n - t < 0)
				continue;
			if (n - t >= M)
				continue;
			rxy(t + M - 1) = rxy(t + M - 1) + x(n) * y(n - t);
		}
	}
	//return indexOriginal;
}
