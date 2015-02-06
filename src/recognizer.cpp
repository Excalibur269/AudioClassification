#include <cstdio>

#include "Collection.h"
#include "CommandLineOptions.h"

#include <fstream>
#include <iostream>
#include <iomanip>


#include "HZCRR.h"
#include "LSTER.h"
#include "APD.h"
#include "BP.h"
#include "NFR.h"
#include "RTPD.h"
#include "Rms.h"
#include "STE.h"
#include "SFR.h"
#include "WavRead.h"
#include "Collection.h"
#include "CommandLineOptions.h"
#include "ReadFeature.h"
#include "SVMClassifier.h"
//#include <windows.h>

using namespace std;

CommandLineOptions cmd_options;

#define PURESPEECH 1
#define SPEECHMUSIC 2
#define SPEECHBGD 3
#define MUSIC 4
#define BGD 5

int helpopt;
int usageopt;

long offset = 0;
long duration = 1000 * 44100;
float start = 0.0f;
float length = 1000.0f;
bool loop = false;
bool onetick = false;
bool pluginMute = false;
int framelength = 10; //帧长

void printUsage(string progName) {
	MRSDIAG("sfplugin.cpp - printUsage");
	cerr << "Usage : " << progName << " [-c collection] file1"
			<< endl;
	cerr << endl;
	exit(1);
}

void printHelp(string progName) {
	cerr << "--------------------------------------------" << endl;
	cerr << "Prints information about the sound files provided as arguments "
			<< endl;
	cerr << endl;
	cerr << "Usage : " << progName << "[-c collection] file1 file2 file3"
			<< endl;
	cerr << endl;
	cerr
			<< "where file1, ..., fileN are sound files in a Marsyas supported format"
			<< endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage      : 显示用户信息" << endl;
	cerr << "-h --help       : 显示帮助信息 " << endl;
	cerr << "-d              : 模型所在目录 " << endl;
	exit(1);
}

void recognize(vector<string> soundfiles) {

	string sfName;
	double srate;
	int memsize;	// 段长
	string chooseSN;	// 语音/非语音决策方式
	string choosePN;	// 纯语音/非纯语音决策方式
	string chooseMB;	// 音乐/环境音决策方式
	string chooseSMB;	// 带音乐语音/带环境音语音决策方式
	int samplesize;	// 一次处理的采样点数
	vector<string>::iterator sfi;

	for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) {
		// 更新音频源文件
		sfName = *sfi;
		cout << "sfName = " << sfName << endl;
		string SNsvmModelName, PNsvmModelName, MBsvmModelName, SMBsvmModelName;
		string SNmaxMinFilename, PNmaxMinFilename, MBmaxMinFilename, SMBmaxMinFilename;
		// playback offset and duration
//		offset = (int)(start * srate);
//		duration = (int)(length * srate);
		WavRead *wavread = new WavRead();
		wavread->readWavFile(sfName.c_str());
		realvec wavdata, in, out;
		wavdata.create(wavread->DataNum);
		for (int i = 0; i < wavdata.getSize(); i++) {
			wavdata(i) = wavread->XN[i];
		}

		int millisec = wavread->DataNum / 16; //16000位采样率

		realvec frameData; //1帧为10ms
		frameData.create(160);
		frameData.setval(0);
		SVMClassifier *svmtrain = new SVMClassifier();
		realvec inZCR;
//		inZCR.create(1, millisec / framelength);
		readZCR(sfName, inZCR);
		HZCRR hzcrr;
		double value_HZCRR = hzcrr.computeHZCRR(inZCR);
		realvec inMFCC;
		inMFCC.create(1, 13); //13维MFCC
		readMFCC(sfName, inMFCC);

		realvec inLSP;
		inLSP.create(1, 10); //10维LSP
		readLSP(sfName, inLSP);
//				realvec inLPCC;
//				inLPCC.create(count, 10); //10维LSP
//				readLPCC(sfName, inLPCC);


		for (int i = 0; i < (millisec / 1000); i++) {
			int labels[100]  = {0};
			for (int j = 0; j < 100; j++) {
				//得到第i秒的第j帧的数据
				frameData = wavdata.getSubVector(160 * (100 * i + j), 160);
				realvec inLSTER, outLSTER;
//				int count = frameData.getCols() / 160; //1帧10ms
//				if ((frameData.getCols() % 160) != 0) {
//					count++;
//				}
				inLSTER.create(1, 160);
				inLSTER.setval(0);
				for(int k = 0;k < frameData.getSize();k++){
					inLSTER(0, k) = frameData(k);
				}
				outLSTER.create(1, 1);
				ShortTimeEnergy ste;
				ste.computeSTE(inLSTER, outLSTER);
				LSTER lster;
				double value_LSTER = lster.computeLSTER(outLSTER);

				Rms rms;
				double value_RMS = rms.computeRms(outLSTER);

//				realvec outBP;
//				outBP.create(4, 1);
//				BP bp;
//				bp.computeBP(frameData, outBP);
//				NFR nfr;
//				double value_NFR = nfr.computeNFR(outBP);

				//HZCRR(1)+LSTER(1)+RMS(1)+LPCC(10)+LSP(10)+MFCC_size
				int allFeatureOrder = 1 + 1 + 1 + 10 + 13;
				in.create(allFeatureOrder, 1);
				in(0, 0) = value_HZCRR;
				in(1, 0) = value_LSTER;
				in(2, 0) = value_RMS;

//				for (int i = 8; i < (8 + 10); i++) {
//					in(0, i) = inLPCC(0, i - 8);
//				}
				int index = 3;
				for(int k = index;k < (index + inLSP.getCols());k++){
					in(k, 0) = inLSP((100*i+j), k - index);
				}
				index = index + inLSP.getCols();
				for(int k = index;k < (index + inMFCC.getCols());k++){
					in(k, 0) = inMFCC((100*i+j), k - index);
				}

				out.create(1);
				svmtrain->svmProcess(in, out, SNsvmModelName, SNmaxMinFilename);
				if (out(0) == 0) { // Speech

					SFR sfr;
					double value_SFR = sfr.computeSFR(outLSTER);

					//ZCR_size+SFR(1)+RMS(1++LSP(10)+MFCC_size
					int allFeatureOrder = 1 + 1 + 1 + 10 + 13;
					in.create(allFeatureOrder, 1);
					in(0, 0) = value_SFR;
					in(1, 0) = value_RMS;

					int index = 2;
					if(i < inZCR.getCols())
						in(index++, 0) = inZCR(0, (100*i+j));
					else
						in(index++, 0) = 0;
					for(int k = index;k < (index + inLSP.getCols());k++){
						in(k, 0) = inLSP((100*i+j), k - index);
					}
					index = index + inLSP.getCols();
					for(int k = index;k < (index + inMFCC.getCols());k++){
						in(k, 0) = inMFCC((100*i+j), k - index);
					}

					svmtrain->svmProcess(in, out, PNsvmModelName, PNmaxMinFilename);
					if(out(0) == 0){ //purespeech
						labels[j] = PURESPEECH;
					}else if(out(1) == 1){ //non-purespeech
						//	STE_size+LPCC+LSP(10)
						int allFeatureOrder = 1 + 10;
						in.create(allFeatureOrder, 1);
						int index = 0;
						in(index++, 0) = outLSTER(0, 0);

						for(int j = index;j < (index + inLSP.getCols());j++){
							in(j, 0) = inLSP((100*i+j), j - index);
						}
						svmtrain->svmProcess(in, out, SMBsvmModelName, SMBmaxMinFilename);
						if(out(0) == 0){ //speechmusic
							labels[j] = SPEECHMUSIC;
						}else{ //speechbgd
							labels[j] = SPEECHBGD;
						}
					}
				}else if(out(0) == 1){ //Non-Speech
					//	STE_size+LPCC+LSP(10)
					int allFeatureOrder = 1 + 10;
					in.create(allFeatureOrder, 1);
					int index = 0;
					in(index++, 0) = outLSTER(0, 0);

					for(int j = index;j < (index + inLSP.getCols());j++){
						in(j, 0) = inLSP((100*i+j), j - index);
					}
					svmtrain->svmProcess(in, out, MBsvmModelName, MBmaxMinFilename);
					if(out(0) == 0){ //music
						labels[j] = MUSIC;
					}else{ //bgd
						labels[j] = BGD;
					}
				}

			}
			int results[6] = {0};
			int max = 0;
			int audioclass;
			for(int k = 0;k < 100;k++){
				switch(labels[k]){
				case PURESPEECH:
					results[PURESPEECH]++;
					break;
				case SPEECHMUSIC:
					results[PURESPEECH]++;
					break;
				case SPEECHBGD:
					results[PURESPEECH]++;
					break;
				case MUSIC:
					results[PURESPEECH]++;
					break;
				case BGD:
					results[PURESPEECH]++;
					break;
				}
			}
			for(int k = 1;k < 6;k++){
				if(results[k] > max){
					max = results[k];
					audioclass = k;
				}
			}
			string audioclassname;
			switch(audioclass){
			case PURESPEECH:
					audioclassname = "PURESPEECH";
					break;
				case SPEECHMUSIC:
					audioclassname = "SPEECHMUSIC";
					break;
				case SPEECHBGD:
					audioclassname = "SPEECHBGD";
					break;
				case MUSIC:
					audioclassname = "MUSIC";
					break;
				case BGD:
					audioclassname = "BGD";
					break;
			}
			cout << i << "s: " <<  audioclassname <<endl;
		}
//		int pos = 0;
//		int count = 1;
//		int temp_pos = 0;
//		int loopsizeSN = 0;
//		int loopsizePN = 0;
//		int loopsizeMB = 0;
//		int loopsizeSMB = 0;
//		string tempLablesSN;
//		string tempLablesPN;
//		string tempLablesMB;
//		string tempLablesSMB;
//		double tempConfSN;
//		double tempConfPN;
//		double tempConfMB;
//		double tempConfSMB;
//		bool enableConf;
//		bool crossBound;
//		int partALength;
//		int partBLength;


	}

}

void initOptions() {
	// 显示帮助信息
	cmd_options.addBoolOption("help", "h", false);
	// 显示用户信息
	cmd_options.addBoolOption("usage", "u", false);
	// 设置模型目录
	cmd_options.addStringOption("dic", "d", "./");
//	// 设置识别时是否播放音频文件
//	cmd_options.addBoolOption("pluginMute", "pm", false);
}

void loadOptions() {
	// 帮助信息
	helpopt = cmd_options.getBoolOption("help");
	// 用户信息
	usageopt = cmd_options.getBoolOption("usage");

}

int main(int argc, const char **argv) {
	MRSDIAG("sfplugin.cpp - main");

	string progName = argv[0];

	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();

	if (argc == 1)
		printUsage(progName);

	vector<string> soundfiles = cmd_options.getRemaining();

	if (helpopt)
		printHelp(progName);

	if (usageopt)
		printUsage(progName);

	recognize(soundfiles);
	exit(0);
}

