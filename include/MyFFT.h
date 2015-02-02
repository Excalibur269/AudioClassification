/*
 **Author:Luo Yong
 **Date:3/24/2009
 */

#include <math.h>
#include <iostream>

#ifndef MARSYAS_MYFFT_H 
#define MARSYAS_MYFFT_H

struct compx {
	double real, imag;
};
/*定义一个复数结构*/


class MyFFT {

public:
	void fft(double data[], int m); /*快速傅里叶变换*/
	void ifft(double data[], int m); /*快速逆傅里叶变换*/
	void fft_process(compx *x, int m); /*快速傅里叶变换计算过程*/
	void ifft_process(compx *x, int m); /*快速逆傅里叶变换计算过程*/
};


#endif
