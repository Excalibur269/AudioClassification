/*
**Author:Luo Yong 
**Date:3/24/2009  
*/

#include "MyFFT.h"

//using namespace Marsyas;
using namespace std;

/*快速傅里叶变换*/
void MyFFT::fft(double data[],int n)
{
	compx *xtemp;	
	int m = 1;

	m = log(n*1.0)/log(2.0);
	xtemp=(compx *)malloc(n*sizeof(compx));
	for(int i=0;i<n;i++)
	{
		xtemp[i].real=data[2*i];
		xtemp[i].imag=data[2*i+1];
	}
	fft_process(xtemp,m);
	for(int i=0;i<n;i++)
	{
		if(fabs(xtemp[i].real)<0.00001)
			xtemp[i].real = 0.0;
		if(fabs(xtemp[i].imag)<0.00001)
			xtemp[i].imag = 0.0;
		data[2*i] = xtemp[i].real;
		data[2*i+1] = xtemp[i].imag;
	}
	free(xtemp);
}

/*快速傅里叶逆变换*/
void MyFFT::ifft(double data[],int n)
{
	compx *xtemp;	
	int m = 1;

	m = log(n*1.0)/log(2.0);
	xtemp=(compx *)malloc(n*sizeof(compx));
	for(int i=0;i<n;i++)
	{
		xtemp[i].real=data[2*i];
		xtemp[i].imag=data[2*i+1];
	}
	ifft_process(xtemp,m);
	for(int i=0;i<n;i++)
	{
		if(fabs(xtemp[i].real)<0.00001)
			xtemp[i].real = 0.0;
		if(fabs(xtemp[i].imag)<0.00001)
			xtemp[i].imag = 0.0;
		data[2*i] = xtemp[i].real;
		data[2*i+1] = xtemp[i].imag;
	}
	free(xtemp);
}

/******************************************************************
fft - 时间抽取基2FFT算法函数
输入为复数序列x结构体指针和m，其中m为基2FFT算法中2的整数幂次，即FFT
长度n =2**m（2的m次方），将计算输出覆盖输入的复数序列
******************************************************************/

/*快速傅里叶变换计算过程*/
void MyFFT::fft_process(compx *x,int m)
{
	static compx *w;		/*存储复数序列*/
	static int mstore = 0;	/*存储m用做参考*/
	static int n = 1;		/*fft长度*/
	compx u,temp,tm;
	compx *xi,*xip,*xj,*wptr;
	int i,j,k,l,le,windex;
	double arg,w_real,w_imag,wrecur_real,wrecur_imag,wtemp_real;
	if(m != mstore)
	{
		/*释放原先分配的内存，并设置新m*/
		if(mstore != 0)	free(w);
		mstore = m;
		if(m == 0)	return;	/*若m为0则返回*/
		/*设置fft长度为n=2**m,即2的m次方*/
		n = 1 << m;
		le = n/2;
		/*为w分配内存*/
		w = (compx *)calloc(le-1,sizeof(compx));
		if(!w)
		{
			printf("\nUnable to allocate compx W array\n");
			exit(1);
		}
		/*计算w的值*/
		arg = 4.0*atan(1.0)/le;	/*计算PI/le*/
		wrecur_real = w_real = cos(arg);
		wrecur_imag = w_imag = -sin(arg);
		xj = w;
		for(j = 1;j<le;j++)
		{
			xj->real = (float)wrecur_real;
			xj->imag = (float)wrecur_imag;
			xj++;
			wtemp_real = wrecur_real*w_real - wrecur_imag*w_imag;
			wrecur_imag = wrecur_real*w_imag + wrecur_imag*w_real;
			wrecur_real = wtemp_real;
		}
	}
	/*开始fft计算*/
	le = n;
	windex = 1;
	for(l=0;l<m;l++)
	{
		le = le/2;
		/*不带乘法的第一级运算*/
		for(i=0;i<n;i = i+2*le)
		{
			xi = x + i;
			xip = xi + le;
			temp.real = xi->real + xip->real;
			temp.imag = xi->imag + xip->imag;
			xip->real = xi->real - xip->real;
			xip->imag = xi->imag - xip->imag;
			*xi = temp;
		}
		/*用w进行运算*/
		wptr = w + windex - 1;
		for(j = 1;j<le;j++)
		{
			u = *wptr;
			for(i = j;i<n;i = i+2*le)
			{
				xi = x + i;
				xip = xi + le;
				temp.real = xi->real + xip->real;
				temp.imag = xi->imag + xip->imag;
				tm.real = xi->real - xip->real;
				tm.imag = xi->imag - xip->imag;
				xip->real = tm.real*u.real - tm.imag*u.imag;
				xip->imag = tm.real*u.imag + tm.imag*u.real;
				*xi = temp;
			}
			wptr = wptr + windex;
		}
		windex = 2*windex;
	}
	/*反向重新调整数据*/
	j = 0;
	for(i = 1;i<(n-1);i++)
	{
		k = n/2;
		while(k<=j)
		{
			j=j-k;
			k=k/2;
		}
		j = j+k;
		if(i<j)
		{
			xi = x + i;
			xj = x + j;
			temp = *xj;
			*xj = *xi;
			*xi = temp;
		}
	}
}

/*快速傅里叶逆变换计算过程*/
void MyFFT::ifft_process(compx *x,int m)
{
	static compx *w;		/*存储复数序列w*/
	static int mstore = 0;	/*存储m用做参考*/
	static int n = 1;		/*ifft长度*/
	compx u,temp,tm;
	compx *xi,*xip,*xj,*wptr;
	int i,j,k,l,le,windex;
	double arg,w_real,w_imag,wrecur_real,wrecur_imag,wtemp_real;
	float scale;
	if(m != mstore)
	{
		/*释放原先分配的内存，并设置新m*/
		if(mstore != 0)	free(w);
		mstore = m;
		if(m == 0)	return;	/*若m为0则返回*/
		/*设置ifft长度为n=2**m,即为2的m次方*/
		n = 1 << m;
		le = n/2;
		/*为w分配内存*/
		w = (compx *)calloc(le-1,sizeof(compx));
		if(!w)
		{
			printf("\nUnable to allocate compx W array\n");
			exit(1);
		}
		/*计算w的值*/
		arg = 4.0*atan(1.0)/le;	/*计算PI/le*/
		wrecur_real = w_real = cos(arg);
		wrecur_imag = w_imag = sin(arg);	/*此处与fft中计算符号相反*/
		xj = w;
		for(j=1;j<le;j++)
		{
			xj->real = (float)wrecur_real;
			xj->imag = (float)wrecur_imag;
			xj++;
			wtemp_real = wrecur_real*w_real - wrecur_imag*w_imag;
			wrecur_imag = wrecur_real*w_imag + wrecur_imag*w_real;
			wrecur_real = wtemp_real;
		}
	}
	/*开始反向fft计算*/
	le = n;
	windex = 1;
	for(l=0;l<m;l++)
	{
		le = le/2;
		/*不带乘法的第一级运算*/
		for(i=0;i<n;i=i+2*le)
		{
			xi = x + i;
			xip = xi + le;
			temp.real = xi->real + xip->real;
			temp.imag = xi->imag + xip->imag;
			xip->real = xi->real - xip->real;
			xip->imag = xi->imag - xip->imag;
			*xi = temp;
		}
		/*用w进行计算*/
		wptr = w + windex - 1;
		for(j=1;j<le;j++)
		{
			u = *wptr;
			for(i=j;i<n;i=i+2*le)
			{
				xi = x + i;
				xip = xi + le;
				temp.real = xi->real + xip->real;
				temp.imag = xi->imag + xip->imag;
				tm.real = xi->real - xip->real;
				tm.imag = xi->imag - xip->imag;
				xip->real = tm.real*u.real - tm.imag*u.imag;
				xip->imag = tm.real*u.imag + tm.imag*u.real;
				*xi = temp;
			}
			wptr = wptr + windex;
		}
		windex = 2*windex;
	}
	/*反向重新调整数据*/
	j = 0;
	for(i=1;i<(n-1);i++)
	{
		k = n/2;
		while(k<=j)
		{
			j = j-k;
			k = k/2;
		}
		j = j+k;
		if(i<j)
		{
			xi = x + i;
			xj = x + j;
			temp = *xj;
			*xj = *xi;
			*xi = temp;
		}
	}
	/*所有结果除以n*/
	scale = (float)(1.0/n);
	for(i=0;i<n;i++)
	{
		x->real = scale*x->real;
		x->imag = scale*x->imag;
		x++;
	}
}
