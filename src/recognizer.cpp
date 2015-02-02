#include <cstdio>

#include "Collection.h"
#include "CommandLineOptions.h"

#include <fstream>
#include <iostream>
#include <iomanip>
#include "ReadFeature.h"
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

void printUsage(string progName) {
	MRSDIAG("sfplugin.cpp - printUsage");
	cerr << "Usage : " << progName << " [-c collection] file1 file2 file3"
			<< endl;
	cerr << endl;
	cerr
			<< "where file1, ..., fileN are sound files in a MARSYAS supported format"
			<< endl;
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
	cerr << "-psn --pluginSN     : 语音/非语音plugin " << endl;
	cerr << "-ppn --pluginPN     : 纯语音/非纯语音plugin " << endl;
	cerr << "-psmb --pluginSMB     : 带音乐语音/带环境音语音plugin " << endl;
	cerr << "-pmb --pluginMB     : 音乐/环境音plugin " << endl;
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

		int millisec = wavread->DataNum / 16000; //16000位采样率

		realvec frameData; //1帧为10ms
		frameData.create(160);
		frameData.setval(0);
		SVMClassifier *svmtrain = new SVMClassifier();
		for (int i = 0; i < (millisec / 1000); i++) {
			int labels[100]  = {0};
			for (int j = 0; j < 100; j++) {
				//得到第i秒的第j帧的数据
				frameData = wavdata.getSubVector(160 * (100 * i + j), 160);
				realvec inZCR;
				inZCR.create(1, millisec / framelength);
				readZCR(sfName, inZCR);

				HZCRR hzcrr;
				double value_HZCRR = hzcrr.computeHZCRR(inZCR);

				realvec inLSTER, outLSTER;
				int count = frameData.getCols() / 160; //1帧10ms
				if ((frameData.getCols() % 160) != 0) {
					count++;
				}
				inLSTER.create(count, 160);
				inLSTER.setval(0);
				for (int i = 0; i < count; i++) {
					if (i == count - 1) {
						for (int j = 160 * i; j < frameData.getSize(); j++)
							inLSTER(j - 160 * i) = frameData(j);
					} else {
						inLSTER = frameData.getSubVector(160 * i, 160);
					}
				}
				outLSTER.create(count, 1);
				ShortTimeEnergy ste;
				ste.computeSTE(inLSTER, outLSTER);
				LSTER lster;
				double value_LSTER = lster.computeLSTER(outLSTER);

				Rms rms;
				double value_RMS = rms.computeRms(inLSTER);

				realvec outBP;
				outBP.create(4, 1);
				BP bp;
				bp.computeBP(frameData, outBP);

				NFR nfr;
				double value_NFR = nfr.computeNFR(outBP);

				realvec inMFCC;
				inMFCC.create(millisec / framelength, framelength);
				readMFCC(sfName, inMFCC);

				realvec inLSP;
				inLSP.create(0, 10); //10维LSP
				readLSP(sfName, inLSP);

				realvec inLPCC;
				inLPCC.create(0, 10); //10维LSP
				readLPCC(sfName, inLPCC);

				//HZCRR(1)+LSTER(1)+RMS(1)+BP(4)+NFR(1)+LPCC(10)+LSP(10)+MFCC_size
				int allFeatureOrder = 1 + 1 + 1 + 4 + 1 + 10 + 10
						+ inMFCC.getSize();
				in.create(1, allFeatureOrder + 1);
				in(0, 0) = value_HZCRR;
				in(0, 1) = value_LSTER;
				in(0, 2) = value_RMS;

				for (int i = 3; i < (3 + 4); i++) {
					in(0, i) = outBP(0, i - 3);
				}
				in(0, 7) = value_NFR;
				for (int i = 8; i < (8 + 10); i++) {
					in(0, i) = inLPCC(0, i - 8);
				}
				for (int i = 18; i < (18 + 10); i++) {
					in(0, i) = inLSP(0, i - 18);
				}
				for (int i = 28; i < (28 + inMFCC.getSize()); i++) {
					in(0, i) = inMFCC(i - 28);
				}

				out.create(1);
				svmtrain->svmProcess(in, out, SNsvmModelName, SNmaxMinFilename);
				if (out(0) == 0) { // Speech

					SFR sfr;
					double value_SFR = sfr.computeSFR(outLSTER);

					//ZCR_size+SFR(1)+RMS(1+LPCC(10)+LSP(10)+MFCC_size
					int allFeatureOrder = inZCR.getCols() + 1 + 1 + 10 + 10
							+ inMFCC.getSize();
					in.create(1, allFeatureOrder); //最后一位填入labelNum
					for (int i = 0; i < inZCR.getCols(); i++) {
						in(0, i) = inZCR(0, i);
					}
					int index = inZCR.getCols();
					in(0, index++) = value_SFR;
					in(0, index++) = value_RMS;

					for (int i = index; i < (index + 10); i++) {
						in(0, i) = inLPCC(0, i - index);
					}
					index += 10;
					for (int i = index; i < (index + 10); i++) {
						in(0, i) = inLSP(0, i - index);
					}
					index += 10;
					for (int i = index; i < (index + inMFCC.getSize()); i++) {
						in(0, i) = inMFCC(i - index);
					}
					svmtrain->svmProcess(in, out, PNsvmModelName, PNmaxMinFilename);
					if(out(0) == 0){ //purespeech
						labels[j] = PURESPEECH;
					}else if(out(1) == 1){ //non-purespeech
						//	STE_size+NFR(1)+LPCC(10)+LSP(10)
						int allFeatureOrder = outLSTER.getRows() + 1 + 10 + 10;
						in.create(1, allFeatureOrder);//最后一位填入labelNum
						for (int i = 0; i < outLSTER.getRows(); i++) {
							in(0, i) = outLSTER(i, 0);
						}
						int index = outLSTER.getRows();
						in(0, index++) = value_NFR;

						for (int i = index; i < (index + 10); i++) {
							in(0, i) = inLPCC(0, i - index);
						}
						index += 10;
						for (int i = index; i < (index + 10); i++) {
							in(0, i) = inLSP(0, i - index);
						}
						svmtrain->svmProcess(in, out, SMBsvmModelName, SMBmaxMinFilename);
						if(out(0) == 0){ //speechmusic
							labels[j] = SPEECHMUSIC;
						}else{ //speechbgd
							labels[j] = SPEECHBGD;
						}
					}
				}else if(out(0) == 1){ //Non-Speech
					//	STE_size+NFR(1)+LPCC(10)+LSP(10)
					int allFeatureOrder = outLSTER.getRows() + 1 + 10 + 10;
					in.create(1, allFeatureOrder);//最后一位填入labelNum
					for (int i = 0; i < outLSTER.getRows(); i++) {
						in(0, i) = outLSTER(i, 0);
					}
					int index = outLSTER.getRows();
					in(0, index++) = value_NFR;

					for (int i = index; i < (index + 10); i++) {
						in(0, i) = inLPCC(0, i - index);
					}
					index += 10;
					for (int i = index; i < (index + 10); i++) {
						in(0, i) = inLSP(0, i - index);
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
			cout << i << "s: " <<  audioclass <<endl;
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
	// 设置区分Speech,Non-Speech网络的Plugin文件
	cmd_options.addStringOption("pluginSN", "psn", EMPTYSTRING);
	// 设置区分Pure Speech,Non-pure Speech网络的Plugin文件
	cmd_options.addStringOption("pluginPN", "ppn", EMPTYSTRING);
	// 设置区分Music,Background Sound网络的Plugin文件
	cmd_options.addStringOption("pluginMB", "pmb", EMPTYSTRING);
	// 设置区分Speech With Music,Speech With Background Sound网络的Plugin文件
	cmd_options.addStringOption("pluginSMB", "psmb", EMPTYSTRING);
	// 设置识别时是否播放音频文件
	cmd_options.addBoolOption("pluginMute", "pm", false);
}

void loadOptions() {
	// 帮助信息
	helpopt = cmd_options.getBoolOption("help");
	// 用户信息
	usageopt = cmd_options.getBoolOption("usage");

	// 识别时是否播放音频文件
	pluginMute = cmd_options.getBoolOption("pluginMute");
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

