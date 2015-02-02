#include <stdio.h>
#include <string.h>
#include <stdlib.h>
//#include "vad_core.h"
typedef unsigned short      WORD;
typedef unsigned int       DWORD;

#ifndef CLOCKS_PER_SEC
#define CLOCKS_PER_SEC ((clock_t)1000000)
#endif
#ifndef WAVREAD_H_
#define WAVREAD_H_

class WavRead{
public:
	void ReadAditerL(int datanum,int Channel);
	void ReadAditerS(int datanum,int Channel);
	void ReadAditerC(int datanum,int Channel);
	FILE * file;  
	double *XN;   
	bool isWavFormat;

	void readWavFile(const char *fname); 
	void CreatWavFile(const char *fname,double *Source,float sampleRate,int bits,int buffsize);

	WavRead();
    ~WavRead(){if (XN != NULL)free(XN);}

	struct RIFF_HEADER
	{
		char szRiffID[4]; 
		DWORD dwRiffSize; 
		char szRiffFormat[4];
	};
	struct WAVE_FORMAT
	{
		WORD wFormatTag;
		WORD wChannels;
		DWORD dwSamplesPerSec;
		DWORD dwAvgBytesPerSec;
		WORD wBlockAlign;
		WORD wBitsPerSample;
	};
	struct FMT_BLOCK
	{
		char szFmtID[4];
		DWORD dwFmtSize;
		WAVE_FORMAT wavFormat;
	};
	struct DATA_BLOCK
	{
		char szDataID[4];
		DWORD dwDataSize;
	};
	long datasize;
	float sampleRate;
	int bits,DataNum,Channel;
};

#endif
/*
*  加载音频
*/
//int load_wav(char *filename, short ** data, int *sampleRate);
/*
* 创建目录
* 传入参数：dir（目录），dir_symbol（目录符号）
* 返回参数: 创建成功（true）,失败（false）
*/
//bool create_dir(char* dir, char dir_symbol);

//int cutscene(char* file, char* destdir);
