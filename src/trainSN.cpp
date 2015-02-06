/* 区分Speech，NonSpeech训练网络 */

#include <cstdio>
#include <string> 
#include <iostream>
#include <map>
#include <stdlib.h>

#include "HZCRR.h"
#include "LSTER.h"
#include "APD.h"
#include "BP.h"
#include "NFR.h"
#include "RTPD.h"
#include "Rms.h"
#include "STE.h"
#include "WavRead.h"
#include "Collection.h"
#include "CommandLineOptions.h"
#include "ReadFeature.h"
#include "SVMClassifier.h"

using namespace std;

int helpopt;
int usageopt;

CommandLineOptions cmd_options;

/*初始化*/
int memSize = 1; //段长，以帧数目的形式决定，后面加载时也有设置
int winSize = 512; //每一帧的采样点数
int hopSize = 512; //每次实际处理的采样点数，即向前跳过的采样点数。
bool pluginMute = 0.0; //识别时是否播放
int numThreshold = 20; //初始化静音帧数目阈值
double slThreshold = 0.01; //初始化静音帧能量阈值
double nfrThreshold = 0.003; //初始化噪音帧阈值
int k = 3; //初始化KNN k值
int kernelType = 0; //初始化SVM核函数

string pluginName = EMPTYSTRING;
string wekafname = EMPTYSTRING;
string extractorName = EMPTYSTRING;
string classifierName = EMPTYSTRING;
string svmModelName = EMPTYSTRING;
string maxMinFilename = EMPTYSTRING;
string choose = EMPTYSTRING;

// 以下数据勿需更改
int offset = 0; //偏移量
double duration = 30.0f; //长度
double start = 0.0;
double length = -1.0;
int accSize_ = 40; //累积次数
double samplingRate_ = 16000.0; //采样率
int framelength = 10; //帧长
/*定义默认的特征和分类器*/

#define DEFAULT_EXTRACTOR "BPSTFTMFCC" 
#define DEFAULT_CLASSIFIER  "SVM"


/*
 *按帧决策方式
 */
void train_frame(vector<Collection> cls, Collection cl, string pluginName,
		string classNames, string wekafname, int memSize, string extractorStr,
		string classifierName) {
	MRSDIAG("train_Speech_Non.cpp - train_Speech_Non");

	if (length == -1.0f)
		length = 30.0f;

	if (classifierName == EMPTYSTRING)
		classifierName = DEFAULT_CLASSIFIER;

	if (extractorStr == EMPTYSTRING)
		extractorStr = DEFAULT_EXTRACTOR;

	cout << "classifierName = " << classifierName << endl;



	// 提取标记数（类别数）
	Collection l = cl;
	int nLabels = l.getNumLabels();



	///////////////////////////////////////////////////////////////////////////
	// 模型训练主循环
	///////////////////////////////////////////////////////////////////////////

//	int wc = 0;
//	int samplesPlayed = 0;

	SVMClassifier *svmtrain = new SVMClassifier();
	svmtrain->setMode("train");
//	WavRead wavread;/
	for (int k = 0; k < l.size(); k++) {
		//reset texture analysis stats between files
		cout << "Processed " << l.entry(k) << endl;
		cout << "start read wav" << endl;
		WavRead *wavread = new WavRead();
		int millisec = 0;
		string label = l.labelEntry(k);
		int labelNum = l.labelNum(label);
		string filename = l.entry(k);
		wavread->readWavFile(filename.c_str());
		realvec wavdata, in, out;
		wavdata.create(1, wavread->DataNum);
		millisec = wavread->DataNum / 16;
		for(int i = 0;i < wavdata.getSize();i++){
			wavdata(0, i) = wavread->XN[i];
		}

		cout << "start compute HZCRR" << endl;
		realvec inZCR;
//		inZCR.create(1,millisec/framelength);
		readZCR(filename, inZCR);
		HZCRR hzcrr;
		double value_HZCRR = hzcrr.computeHZCRR(inZCR);

		cout << "start compute LSTER" << endl;
		realvec inLSTER, outLSTER;
		int count = wavdata.getCols() / 160; //1帧10ms
		if((wavdata.getCols() % 160) != 0){
			count++;
		}
		inLSTER.create(count, 160);
		inLSTER.setval(0);
		for (int i = 0; i < count; i++) {
			if (i == count - 1) {
				for (int j = 160 * i; j < wavdata.getSize(); j++)
					inLSTER(i, j - 160 * i) = wavdata(j);
			} else {
				for (int j = 0; j < 160; j++)
					inLSTER(i, j) = wavdata(160 * i + j);
			}
		}
		outLSTER.create(count, 1);
		ShortTimeEnergy ste;
		ste.computeSTE(inLSTER, outLSTER);
		LSTER lster;
		double value_LSTER = lster.computeLSTER(outLSTER);
		cout << "start compute RMS" << endl;
		Rms rms;
		double value_RMS = rms.computeRms(outLSTER);

//		cout << "start compute BP" << endl;
//		realvec outBP;
//		outBP.create(4,1);
//		BP bp;
//		bp.computeBP(wavdata, outBP);
//		cout << "start compute NFR" << endl;
//		NFR nfr;
//		double value_NFR = nfr.computeNFR(outBP);

		cout << "read MFCCfile" << endl;
		realvec inMFCC;
		inMFCC.create(count, 13); //13维MFCC
		readMFCC(filename, inMFCC);

		cout << "read LSPfile" << endl;
		realvec inLSP;
		inLSP.create(count, framelength); //10维LSP
		readLSP(filename, inLSP);

//		cout << "read LPCCfile" << endl;
//		realvec inLPCC;
//		inLPCC.create(millisec/framelength + 1, framelength); //10维LSP
//		readLPCC(filename, inLPCC);

		//HZCRR(1)+LSTER(1)+RMS(1)+LPCC_size+LSP_size+MFCC_size
//		int allFeatureOrder = 1 + 1 + 1 + inLPCC.getSize() + inLSP.getSize() + inMFCC.getSize();
		int allFeatureOrder = 1 + 1 + 1 + 10 + 13;
		in.create(allFeatureOrder + 1, 1); //最后一位填入labelNum
		in(0,0) = value_HZCRR;
		in(1,0) = value_LSTER;
		in(2,0) = value_RMS;
		for(int i = 0;i < inLSP.getRows();i++){
			int index = 3;
			for(int j = index;j < (index + inLSP.getCols());j++){
				in(j, 0) = inLSP(i, j - index);
			}
			index = index + inLSP.getCols();
			for(int j = index;j < (index + inMFCC.getCols());j++){
				in(j, 0) = inMFCC(i, j - index);
			}
			in(in.getRows() -1, 0) = labelNum; //最后一个填入标注名称数字
			svmtrain->svmProcess(in, out, svmModelName, maxMinFilename);
		}
//		for(int i = 3;i < (3 + 4);i++){
//			in(0, i) = outBP(0, i - 3);
//		}
//		in (0,7) = value_NFR;

//		for(int i = index;i < (index + inLPCC.getSize());i++){
//			in(0,i) = inLPCC(i - index);
//		}
//		index = index + inLPCC.getSize();
//		wc = 0;
//		samplesPlayed = 0;
		delete wavread;
	}

	// 训练结束更新状态


	// 重新构建网络，并更新状态，为识别做准备
	// 默认状态（mode）为“train”，在训练完做识别时需将mode改为“predict”
	svmtrain->setMode("predict");
	realvec in, out;
	svmtrain->svmProcess(in, out, svmModelName, maxMinFilename);
}

/*
 *按段决策
 */
//void train_window(vector<Collection> cls, Collection cl, string pluginName,
//		string classNames, int memSize, string extractorStr,
//		string classifierName) {
//	// 从“静音剔除”到“帧特征提取”都与按帧决策完全一致
//
//	MRSDIAG("train_Speech_Non.cpp - train_Speech_Non");
//
//	if (length == -1.0f)
//		length = 30.0f;
//
//	if (classifierName == EMPTYSTRING)
//		classifierName = DEFAULT_CLASSIFIER;
//
//	if (extractorStr == EMPTYSTRING)
//		extractorStr = DEFAULT_EXTRACTOR;
//
//	//////////////////////////////////////////////////////////////////////////
//	// Find proper sound file format and create SignalSource
//	//////////////////////////////////////////////////////////////////////////
//	// 获取首个Collection（类别）的首个训练文件名
//	Collection linitial = cls[0];
//	string sfName = linitial.entry(0);
//
//
//
//
//	//////////////////////////////////////////////////////////////////////////
//	// 创建对特征向量矩阵进行处理的组件
//	//////////////////////////////////////////////////////////////////////////
//
//	MarSystem* statistics = mng.create("Fanout", "statistics");
//	// 计算均值和方差对任意特征向量都适用
//	statistics->addMarSystem(mng.create("Mean", "mn"));
//	statistics->addMarSystem(mng.create("StandardDeviation", "std"));
//	if (extractorName == "ZSNFR") {
//		statistics->addMarSystem(mng.create("HZCRR", "hzcrr"));
//		statistics->addMarSystem(mng.create("LSTER", "lster"));
//		statistics->addMarSystem(mng.create("NFR", "nfr"));
//		statistics->updctrl("HZCRR/hzcrr/int/obrow", 0);
//		statistics->updctrl("LSTER/lster/int/obrow", 1);
//		statistics->updctrl("NFR/nfr/int/obrow", 2);
//		statistics->updctrl("NFR/nfr/double/threshold", nfrThreshold);
//		statistics->updctrl("NFR/nfr/int/memSize", memSize);
//	} else if (extractorName == "ZSRLL_BPSTFTMFCC"
//			|| extractorName == "ZSRELL_BPSTFTMFCC") {
//		statistics->addMarSystem(mng.create("HZCRR", "hzcrr"));
//		statistics->addMarSystem(mng.create("LSTER", "lster"));
//		statistics->updctrl("HZCRR/hzcrr/int/obrow", 0);
//		statistics->updctrl("LSTER/lster/int/obrow", 1);
//	} else {
//		cerr << "Unsuported Feature : " << endl;
//		return;
//	}
//
//
//
//
//
//
//
//	// 提取标记数（类别数）
//	Collection l = cl;
//	int nLabels = l.getNumLabels();
//
//
//
//	///////////////////////////////////////////////////////////////////////////
//	// 模型训练主循环
//	///////////////////////////////////////////////////////////////////////////
//	int wc = 0;
//	int samplesPlayed = 0;
//	for (int i = 0; i < l.size(); i++) {
//		total->updctrl("Annotator/annotator/int/label",
//				l.labelNum(l.labelEntry(i)));
//		total->updctrl(ctrl_filename_, l.entry(i));
//		wc = 0;
//		samplesPlayed = 0;
//		while (ctrl_notEmpty_->to<bool>() && (duration > samplesPlayed)) {
//			total->tick();
//			wc++;
//			samplesPlayed = wc * onSamples;
//		}
//		total->tick();
//		cout << "Processed " << l.entry(i) << endl;
//	}
//
//	// 训练结束更新状态
//	if (classifierName == "GS")
//		total->updctrl("GaussianClassifier/gaussian/bool/done", true);
//	else if (classifierName == "KNN")
//		total->updctrl("KNNClassifier/knn/bool/done", true);
//	else if (classifierName == "SVM")
//		;
//
//	// 重新构建网络，并更新状态，为识别做准备
//	// 默认状态（mode）为“train”，在训练完做识别时需将mode改为“predict”
//
//	if (classifierName == "GS") {
//		total->updctrl("GaussianClassifier/gaussian/bool/done", false);
//		total->updctrl("GaussianClassifier/gaussian/mrs_string/mode",
//				"predict");
//	} else if (classifierName == "KNN") {
//		total->updctrl("KNNClassifier/knn/bool/done", false);
//		total->updctrl("KNNClassifier/knn/mrs_string/mode", "predict");
//		total->updctrl("KNNClassifier/knn/int/k", k); //[!] hardcoded!!!
//	} else if (classifierName == "SVM") {
//		total->updctrl("SVMClassifier/svm/mrs_string/mode", "predict");
//	}
//
//	total->tick();
//
//
//
//
//	// 输出训练网络
//
//	// 未设置pluginName，则直接将结果（整个网络）打印在屏幕上
//	if (pluginName == EMPTYSTRING)
//		cout << (*total) << endl;
//	// 否则将结果（整个网络）写入设置的pluginName文件中
//	else {
//		ofstream oss(pluginName.c_str());
//		oss << (*total) << endl;
//	}
//
//	delete total;
//}
//
///*
// *读取文件
// */
void readCollection(Collection& l, string name) {
	cout << "sfplay.cpp - readCollection" << endl;;
	ifstream from1(name.c_str());
	int attempts = 0;

	MRSDIAG("Trying current working directory: " + name);
	if (from1.good() == false) {
		attempts++;
	} else {
		from1 >> l;
		l.setName(name.substr(0, name.rfind(".", name.length())));
	}

	if (attempts == 1) {
		string warn;
		warn += "Problem reading collection ";
		warn += name;
		warn +=
				" - tried both default mf directory and current working directory";
		cerr << warn << endl;;
		exit(1);
	}
}

/*
 *添加相应的选项信息，并初始化
 */
void initOptions() {
	// 显示帮助信息
	cmd_options.addBoolOption("help", "h", false);
	// 显示用法信息
	cmd_options.addBoolOption("usage", "u", false);
	// 设置段大小，默认为40
	cmd_options.addNaturalOption("memory", "m", 40);
	// 设置每次处理的采样点数，即每帧的采样点数，默认为512
	cmd_options.addNaturalOption("nwinsamples", "ws", 512);
	// 设置每次移入的采样点数，每次实际处理的采样点数，默认为512
	cmd_options.addNaturalOption("nhopsamples", "hp", 512);
	// 选择特征
	cmd_options.addStringOption("extractor", "e", EMPTYSTRING);
	// 设置被确定为静音帧的阈值，默认为0.01
	cmd_options.addRealOption("slthreshold", "sth", 0.01);
	// 设置静音帧数目阈值，默认为20
	cmd_options.addNaturalOption("numThreshold", "nth", 20);
	// 设置被确定为噪音帧的阈值，默认为0.003
	cmd_options.addRealOption("nfrThreshold", "nfrth", 0.003);
	// 选择分类器，默认为高斯混合模型分类器(-cl GS)
	cmd_options.addStringOption("classifier", "cl", EMPTYSTRING);
	// 设置K近邻分类器中的k值，默认为3
	cmd_options.addNaturalOption("k", "k", 3);
	// 设置SVM核函数，0：线性，1：RBF，2：POLY，3：SIGMOID
	cmd_options.addNaturalOption("kernelType", "ktp", 0);
	// 设置SVM 模型文件名
	cmd_options.addStringOption("svmmodelname", "mdl", EMPTYSTRING);
	// 设置SVM maxmin文件名
	cmd_options.addStringOption("maxmin", "mm", EMPTYSTRING);
	// 选择决策方式，默认为f
	cmd_options.addStringOption("frame_window", "fw", "f");
	// 设置要输出的插件文件名(存放分类网络)
	cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
	// 设置分类时是否播放文件,有此选项不播放，默认为播放
	cmd_options.addBoolOption("pluginmute", "pm", false);
}

/*
 *从命令行读入各个选项的信息
 */
void loadOptions() {
	// 帮助信息
	helpopt = cmd_options.getBoolOption("help");
	// 用法信息
	usageopt = cmd_options.getBoolOption("usage");
	// 段大小，默认为40
	memSize = cmd_options.getNaturalOption("memory");
	// 每次处理的采样点数，即每帧的采样点数，默认为512
	winSize = cmd_options.getNaturalOption("nwinsamples");
	// 每次移入的采样点数，每次实际处理的采样点数，默认为512
	hopSize = cmd_options.getNaturalOption("nhopsamples");
	// 被确定为静音帧的阈值，默认为0.01
	slThreshold = cmd_options.getRealOption("slthreshold");
	// 静音帧数目阈值，默认为20
	numThreshold = cmd_options.getNaturalOption("numThreshold");
	// 被确定为噪音帧的阈值，默认为0.003
	nfrThreshold = cmd_options.getRealOption("nfrThreshold");
	// SVM核函数，0：线性，1：RBF，2：POLY，3：SIGMOID
	kernelType = cmd_options.getNaturalOption("kernelType");
	// SVM 模型文件名
	svmModelName = cmd_options.getStringOption("svmmodelname");
	// SVM maxmin文件名
	maxMinFilename = cmd_options.getStringOption("maxmin");
	// 决策方式，默认为f
	choose = cmd_options.getStringOption("frame_window");
}

/*
 **打印用法信息
 */
void printUsage(string progName) {
	MRSDIAG("train_Speech_Non.cpp - printUsage");
	cerr << "Usage : " << progName
			<< " [-e extractor] [-h help] [-m memory]  [-u usage] collection1 collection2 ... collectionN"
			<< endl;
	cerr << endl;
}

/*
 **打印用户信息
 */
void printHelp(string progName) {
	MRSDIAG("train_Speech_Non.cpp - printHelp");
	cerr << "train------instructions: " << endl;
	cerr << "---------------------------------------------------------------"
			<< endl;
	cerr << "Prints information about the sound files provided as arguments "
			<< endl;
	cerr << "Usage : " << progName << "[-c collection] file1 file2 file3"
			<< endl;
	cerr << endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage		: 显示帮助信息(false)" << endl;
	cerr << "-h --help		: 显示用法信息(false)" << endl;
	cerr << "-m --memory		: 设置段大小(40)" << endl;
	cerr << "-ws --nwinsamples	: 设置每帧的采样点数(512)" << endl;
	cerr << "-hp --nhopsamples	: 设置实际处理的采样点数(512)" << endl;
	cerr << "-sth --slthreshold	: 设置被确定为静音帧的阈值(0.01)" << endl;
	cerr << "-nth --numthreshold	: 设置静音帧数目阈值(20)" << endl;
	cerr << "-nfrth --nfrthreshold	: 设置被确定为噪音帧的阈值(0.003)" << endl;
	cerr << "-ktp --kernel type	: SVM核函数(0)" << endl;
	cerr << "-mdl --SVM model name	:SVM模型文件名" << endl;
	cerr << "-mm --SVM maxmin name	:SVM maxmin文件名" << endl;
	cerr << "-fw --method		：选择决策方式(f)" << endl;
//	cerr << "-pm --pluginmute	: 分类时是否播放文件(false)" << endl;

	cerr << endl;
}

int main(int argc, const char **argv) {
	MRSDIAG("train.cpp - main");

	string progName = argv[0];
	if (argc == 1) {
		printUsage(progName);
		return 0;
	}

	initOptions();
	cmd_options.readOptions(argc, argv);
	loadOptions();

	if (helpopt)
		printHelp(progName);

	if (usageopt)
		printUsage(progName);

	//////////////////////////////////////////////////////////////////////////
	// Print analysis options
	//////////////////////////////////////////////////////////////////////////
	cout << endl;
	cout << "Memory Size (in analysis windows):" << memSize << endl;
	cout << "Window Size (in samples): " << winSize << endl;
	cout << "Hop Size (in samples): " << hopSize << endl;
	cout << "Extractor = " << extractorName << endl;
	cout << endl;

	string classNames = "";	// 类别名，如nonspeech、speech
	vector<Collection> cls;
	vector<string>::iterator sfi;
	// 获取命令行中余下的未处理的字符串，即Collection
	vector<string> soundfiles = cmd_options.getRemaining();

	// 将Collection加上类别标记
	for (sfi = soundfiles.begin(); sfi != soundfiles.end(); ++sfi) {
		string sfname = *sfi;
		Collection l;
		readCollection(l, sfname);

		if (!l.hasLabels()) {
			l.labelAll(l.name());
			classNames += (l.name() + ',');
		}

		cls.push_back(l);
	}

	// 将所有的文件组织成一个文件，统一输入
	Collection single;
	single.concatenate(cls);

	// 判断输入的特征是否是系统所支持的特征
	string extractorStr = extractorName;

	// 	根据所选决策方式的不同，选择不同的训练函数
	if (choose == "f")
		train_frame(cls, single, pluginName, classNames, wekafname, memSize,
				extractorName, classifierName);
	else if (choose == "w")
//		train_window(cls, single, pluginName, classNames, memSize,
//				extractorName, classifierName);

	return 0;
}
