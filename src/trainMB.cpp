
#include <cstdio>
#include <string> 

#include "Collection.h"
#include "CommandLineOptions.h"

using namespace std;

int helpopt;
int usageopt;

CommandLineOptions cmd_options;

int memSize = 1;
int winSize = 512;
int hopSize = 512;
bool pluginMute = 0.0;
int numThreshold = 20;
double slThreshold = 0.01;
double nfrThreshold = 0.003;
int k = 3;
int kernelType = 0;

string pluginName = EMPTYSTRING;
string wekafname = EMPTYSTRING;
string extractorName = EMPTYSTRING;
string classifierName = EMPTYSTRING;
string svmModelName = EMPTYSTRING;
string maxMinFilename = EMPTYSTRING;
string choose = EMPTYSTRING;

int offset = 0;
double duration = 30.0f;
double start = 0.0;
double length = -1.0;
int accSize_ = 40;
double samplingRate_ = 16000.0;

#define DEFAULT_EXTRACTOR "STFTMFCC"
#define DEFAULT_CLASSIFIER  "SVM"

void train_frame(vector<Collection> cls, Collection cl, string pluginName,
		string classNames, string wekafname, int memSize, string extractorStr,
		string classifierName) {
	MRSDIAG("train_Pure_Non.cpp - train_Pure_Non");

	if (length == -1.0f)
		length = 30.0f;

	if (classifierName == EMPTYSTRING)
		classifierName = DEFAULT_CLASSIFIER;

	if (extractorStr == EMPTYSTRING)
		extractorStr = DEFAULT_EXTRACTOR;

	//////////////////////////////////////////////////////////////////////////
	// Find proper sound file format and create SignalSource
	//////////////////////////////////////////////////////////////////////////

	Collection linitial = cls[0];
	string sfName = linitial.entry(0);

	Collection l = cl;
	int nLabels = l.getNumLabels();

	///////////////////////////////////////////////////////////////////////////
	// 模型训练主循环
	///////////////////////////////////////////////////////////////////////////

	//	int wc = 0;
	//	int samplesPlayed = 0;

	SVMClassifier *svmtrain = new SVMClassifier();
	svmtrain->setMode("train");
//	WavRead wavread;
	for (int i = 0; i < l.size(); i++) {
		//reset texture analysis stats between files
		WavRead *wavread = new WavRead();
		int millisec = 0;
		string label = l.labelEntry(i);
		int labelNum = l.labelNum(label);
		string filename = l.entry(i);
		wavread->readWavFile(filename.c_str());
		realvec wavdata, in, out;
		wavdata.create(1, wavread->DataNum);
		millisec = wavread->DataNum / samplingRate_;
		for (int i = 0; i < wavdata.getSize(); i++) {
			wavdata(0, i) = wavread->XN[i];
		}


		realvec inLSTER, outLSTER;
		int count = wavdata.getCols() / 160;
		if ((wavdata.getCols() % 160) != 0) {
			count++;
		}
		inLSTER.create(count, 160);
		inLSTER.setval(0);
		for (int i = 0; i < count; i++) {
			if (i == count - 1) {
				for (int j = 160 * i; j < wavdata.getSize(); j++)
					inLSTER(j - 160 * i) = wavdata(j);
			} else {
				inLSTER = wavdata.getSubVector(160 * i, 160);
			}
		}
		outLSTER.create(count, 1);
		ShortTimeEnergy ste;
		ste.computeSTE(inLSTER, outLSTER);

		realvec outBP;
		outBP.create(4,1);
		BP bp;
		bp.computeBP(wavdata, outBP);

		NFR nfr;
		double value_NFR = nfr.computeNFR(outBP);


		realvec inLSP;
		inLSP.create(0, 10); //10维LSP
		readLSP(filename, inLSP);

		realvec inLPCC;
		inLPCC.create(0, 10); //10维LSP
		readLPCC(filename, inLPCC);

		//	STE_size+NFR(1)+LPCC(10)+LSP(10)
		int allFeatureOrder = outLSTER.getRows() + 1 + 10 + 10;
		in.create(1, allFeatureOrder + 1);//最后一位填入labelNum
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
		in(0, in.getCols() - 1) = labelNum; //最后一个填入标注名称数字
		svmtrain->svmProcess(in, out, svmModelName, maxMinFilename);
		//		wc = 0;
		//		samplesPlayed = 0;
		cout << "Processed " << l.entry(i) << endl;
		delete wavread;
	}

	// 训练结束更新状态

	// 重新构建网络，并更新状态，为识别做准备
	// 默认状态（mode）为“train”，在训练完做识别时需将mode改为“predict”
	svmtrain->setMode("predict");
	realvec in, out;
	svmtrain->svmProcess(in, out, svmModelName, maxMinFilename);
}

//
//void train_window(vector<Collection> cls, Collection cl, string pluginName,
//		string classNames, int memSize, string extractorStr,
//		string classifierName) {
//
//	MRSDIAG("train_Pure_Non.cpp - train_Pure_Non");
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
//	Collection linitial = cls[0];
//	string sfName = linitial.entry(0);
//
//	MarSystemManager mng;
//	MarSystem *src = mng.create("SoundFileSource", "src");
//	src->updctrl("mrs_string/filename", sfName);
//
//	// ������ƫ�����ͳ���ʱ�䣬Ϊѵ��ʱ����ÿ��С��Ƶ�ļ���׼��
//	if (start > 0.0)
//		offset = (int) (start * src->getctrl("double/israte")->to<double>()
//				* src->getctrl("int/onObservations")->to<int>());
//
//	duration = (int) (length * src->getctrl("double/israte")->to<double>()
//			* src->getctrl("int/onObservations")->to<int>());
//
//	cout << "duration = " << duration << endl;
//	cout << "offset = " << offset << endl;
//
//	// ���������޳��������������һЩ���Ƶĳ�ʼֵ
//	MarSystem *gen = mng.create("GetEnergy", "gen");
//	gen->updctrl("mrs_bool/mute", true);// ѵ��ʱ���ж��Ƿ�Ϊ��������������ʶ��ʱ����
//	gen->updctrl("mrs_bool/enableConf", true);
//	// ���þ����˳�����slThreshold��numThreshold
//	gen->updctrl("int/numThreshold", numThreshold);	//the default value is 20
//	gen->updctrl("double/threshold", slThreshold);
//	// ����һ���������������Ŀ
//	gen->updctrl("int/memSize", memSize);
//
//	cout << "numThreshold=" << numThreshold << endl;
//	cout << "silence energy threshold=" << slThreshold << endl;
//
//	MarSystem * srcsink = mng.create("Series", "srcsink");
//	srcsink->addMarSystem(src);
//	// ������Ƶ�������(��ѵ��ʱ�Ƚ������ε���������,���������Plugin)
//	if (pluginName != EMPTYSTRING) {
//		MarSystem* dest = mng.create("AudioSink", "dest");
//		dest->updctrl("mrs_bool/mute", true);
//		srcsink->addMarSystem(dest);
//	}
//	gen->addMarSystem(srcsink);
//
//	//////////////////////////////////////////////////////////////////////////
//	// Build the overall feature calculation network��������������featureNetwork��
//	//////////////////////////////////////////////////////////////////////////
//	MarSystem* featureNetwork = mng.create("Series", "featureNetwork");
//	featureNetwork->addMarSystem(gen);
//
//	//////////////////////////////////////////////////////////////////////////
//	// Feature Extractor(ͨ��ӳ������������ȡģ��)
//	//////////////////////////////////////////////////////////////////////////
//	MarSystem* featExtractor = (*featExtractors[extractorStr])();
//	featExtractor->updctrl("int/winSize", winSize);
//	featureNetwork->addMarSystem(featExtractor);
//
//	// �����ۻ��������������������ȫ�����룬��������ִ�д���
//	MarSystem * acc = mng.create("Accumulator", "acc");
//	if (accSize_ != memSize)
//		accSize_ = memSize;
//	acc->updctrl("int/nTimes", accSize_);
//	acc->addMarSystem(featureNetwork);
//
//	// �����ܵ�����
//	MarSystem * total = mng.create("Series", "total");
//	total->addMarSystem(acc);
//
//	//////////////////////////////////////////////////////////////////////////
//	// ��������������������д�������
//	//////////////////////////////////////////////////////////////////////////
//
//	MarSystem* statistics = mng.create("Fanout", "statistics");
//	// �����ֵ�ͷ����������������������
//	statistics->addMarSystem(mng.create("Mean", "mn"));
//	statistics->addMarSystem(mng.create("StandardDeviation", "std"));
//	if (extractorName == "RZLL_STFTMFCC") {
//		statistics->addMarSystem(mng.create("SFR", "sfr"));
//		statistics->updctrl("SFR/sfr/double/threshold", slThreshold);
//		statistics->updctrl("SFR/sfr/int/memSize", memSize);
//		statistics->updctrl("SFR/sfr/double/obrow", 0);
//	} else {
//		cerr << "Unsuported Feature : " << endl;
//		return;
//	}
//
//	total->updctrl("int/inSamples", hopSize);
//	total->updctrl(
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/"
//					+ src->getType() + "/src/int/pos", offset);
//
//	//////////////////////////////////////////////////////////////////////////
//	// add the Annotator(����ȡ�������������һ�м������ͣ��ο�����)
//	//////////////////////////////////////////////////////////////////////////
//	total->addMarSystem(statistics);
//	total->addMarSystem(mng.create("Annotator", "annotator"));
//
//	//////////////////////////////////////////////////////////////////////////
//	// add classifier(���������������svmͬʱ����model�ͺ˺���)
//	//////////////////////////////////////////////////////////////////////////
//	cout << "classifierName = " << classifierName << endl;
//	if (classifierName == "GS")
//		total->addMarSystem(mng.create("GaussianClassifier", "gaussian"));
//	else if (classifierName == "KNN")
//		total->addMarSystem(mng.create("KNNClassifier", "knn"));
//	else if (classifierName == "SVM") {
//		total->addMarSystem(mng.create("SVMClassifier", "svm"));
//		total->updctrl("SVMClassifier/svm/mrs_string/filename", svmModelName);
//		total->updctrl("SVMClassifier/svm/mrs_string/maxminfile",
//				maxMinFilename);
//		total->updctrl("SVMClassifier/svm/int/kernelType", kernelType);
//	} else {
//		cerr << "Unsuported classifier : " << classifierName << endl;
//		return;
//	}
//
//	// ������Ŷ�ģ�飨ͶƱ��
//	total->addMarSystem(mng.create("Confidence", "confidence"));
//	total->updctrl("Confidence/confidence/mrs_string/fw", choose);
//	total->updctrl("Confidence/confidence/int/memSize", memSize);
//
//	//////////////////////////////////////////////////////////////////////////
//	// link controls�����ӿ��Ʋ�����
//	//////////////////////////////////////////////////////////////////////////
//
//	total->linkctrl("mrs_string/filename",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/mrs_string/filename");
//	total->linkctrl(
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/mrs_string/currentlyPlaying",
//			"Confidence/confidence/mrs_string/fileName");
//	total->linkctrl("double/israte",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/double/israte");
//	total->linkctrl("int/pos",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/int/pos");
//	total->linkctrl("Confidence/confidence/mrs_bool/enableConf",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/mrs_bool/enableConf");
//	total->linkctrl("Confidence/confidence/mrs_bool/firstTest",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/mrs_bool/firstTest");
//	total->linkctrl("Confidence/confidence/mrs_bool/crossBound",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/mrs_bool/crossBound");
//	total->linkctrl("Confidence/confidence/int/partALength",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/int/partALength");
//	total->linkctrl("Confidence/confidence/int/partBLength",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/int/partBLength");
//	total->linkctrl("Confidence/confidence/int/pos",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/int/pos");
//	total->linkctrl("Confidence/confidence/double/rate",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/double/israte");
//	total->linkctrl("mrs_bool/notEmpty",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/mrs_bool/notEmpty");
//	total->linkctrl("mrs_bool/initAudio",
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/AudioSink/dest/mrs_bool/initAudio");
//
//	MarControlPtr ctrl_filename_ =
//			total->getctrl(
//					"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/mrs_string/filename");
//	MarControlPtr ctrl_notEmpty_ =
//			total->getctrl(
//					"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/SoundFileSource/src/mrs_bool/notEmpty");
//
//	int onSamples = total->getctrl("int/onSamples")->to<int>();
//
//	if (classifierName == "GS")
//		total->updctrl("GaussianClassifier/gaussian/int/nClasses",
//				(int) cls.size());
//	else if (classifierName == "KNN")
//		total->updctrl("KNNClassifier/knn/int/nLabels", (int) cls.size());
//	else if (classifierName == "SVM")
//		;	//��ΪSVMû����д���ƣ�����û��д
//
//	total->updctrl("Confidence/confidence/int/nLabels", (int) cls.size());
//	total->updctrl("Confidence/confidence/mrs_bool/mute", true);
//	total->updctrl("Confidence/confidence/mrs_string/labelNames", classNames);
//	total->updctrl("Confidence/confidence/mrs_bool/print", true);
//
//	// ��ȡ��������������
//	Collection l = cl;
//	int nLabels = l.getNumLabels();
//
//	if (classifierName == "GS")
//		total->updctrl("GaussianClassifier/gaussian/int/nClasses", nLabels);
//	else if (classifierName == "KNN")
//		total->updctrl("KNNClassifier/knn/int/nLabels", nLabels);
//	else if (classifierName == "SVM") {
//		//SVMĬ�ϵ����ü��ɣ�����ڴ�û�в���
//	}
//
//	total->updctrl("Confidence/confidence/int/nLabels", nLabels);
//	total->updctrl("Confidence/confidence/mrs_bool/mute", true);
//	total->updctrl("Confidence/confidence/mrs_string/labelNames",
//			l.getLabelNames());
//	total->updctrl("Confidence/confidence/mrs_bool/print", true);
//
//	///////////////////////////////////////////////////////////////////////////
//	// ģ��ѵ����ѭ��
//	///////////////////////////////////////////////////////////////////////////
//	int wc = 0;
//	int samplesPlayed = 0;
//	for (int i = 0; i < l.size(); i++) {
//		total->updctrl("Annotator/annotator/int/label",
//				l.labelNum(l.labelEntry(i)));
//		total->updctrl(ctrl_filename_, l.entry(i));
//		wc = 0;
//		samplesPlayed = 0;
//		while (ctrl_notEmpty_->to<mrs_bool>() && (duration > samplesPlayed)) {
//			total->tick();
//			wc++;
//			samplesPlayed = wc * onSamples;
//		}
//		total->tick();
//		cout << "Processed " << l.entry(i) << endl;
//	}
//
//	// ѵ����������״̬
//	if (classifierName == "GS")
//		total->updctrl("GaussianClassifier/gaussian/mrs_bool/done", true);
//	else if (classifierName == "KNN")
//		total->updctrl("KNNClassifier/knn/mrs_bool/done", true);
//	else if (classifierName == "SVM")
//		;
//
//	// ���¹������磬������״̬��Ϊʶ����׼��
//	// Ĭ��״̬��mode��Ϊ��train������ѵ������ʶ��ʱ�轫mode��Ϊ��predict��
//
//	if (classifierName == "GS") {
//		total->updctrl("GaussianClassifier/gaussian/mrs_bool/done", false);
//		total->updctrl("GaussianClassifier/gaussian/mrs_string/mode",
//				"predict");
//	} else if (classifierName == "KNN") {
//		total->updctrl("KNNClassifier/knn/mrs_bool/done", false);
//		total->updctrl("KNNClassifier/knn/mrs_string/mode", "predict");
//		total->updctrl("KNNClassifier/knn/int/k", k); //[!] hardcoded!!!
//	} else if (classifierName == "SVM") {
//		total->updctrl("SVMClassifier/svm/mrs_string/mode", "predict");
//	}
//
//	total->tick();
//
//	// ���¾����޳����״̬
//
//	total->updctrl(
//			"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/mrs_bool/mute",
//			false);
//	if (pluginName != EMPTYSTRING && !pluginMute) {
//		total->updctrl(
//				"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/AudioSink/dest/mrs_bool/mute",
//				false);
//		total->updctrl(
//				"Accumulator/acc/Series/featureNetwork/GetEnergy/gen/Series/srcsink/AudioSink/dest/mrs_bool/initAudio",
//				true);
//	}
//	total->updctrl("Confidence/confidence/mrs_bool/mute", false);
//	total->updctrl("Confidence/confidence/mrs_bool/enableConf", true);
//
//	// ���ѵ������
//
//	// δ����pluginName����ֱ�ӽ�������������磩��ӡ����Ļ��
//	if (pluginName == EMPTYSTRING)
//		cout << (*total) << endl;
//	// ���򽫽�����������磩д�����õ�pluginName�ļ���
//	else {
//		ofstream oss(pluginName.c_str());
//		oss << (*total) << endl;
//	}
//
//	delete total;
//}

void readCollection(Collection& l, string name) {
	MRSDIAG("sfplay.cpp - readCollection");
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
		MRSWARN(warn);
		exit(1);
	}
}

void initOptions() {
	cmd_options.addBoolOption("help", "h", false);
	cmd_options.addBoolOption("usage", "u", false);
	cmd_options.addNaturalOption("memory", "m", 40);
	cmd_options.addNaturalOption("nwinsamples", "ws", 512);
	cmd_options.addNaturalOption("nhopsamples", "hp", 512);
	cmd_options.addStringOption("extractor", "e", EMPTYSTRING);
	cmd_options.addRealOption("slthreshold", "sth", 0.01);
	cmd_options.addNaturalOption("numThreshold", "nth", 20);
	cmd_options.addRealOption("nfrThreshold", "nfrth", 0.003);
	cmd_options.addStringOption("classifier", "cl", EMPTYSTRING);
	cmd_options.addNaturalOption("k", "k", 3);
	cmd_options.addNaturalOption("kernelType", "ktp", 0);
	cmd_options.addStringOption("svmmodelname", "mdl", EMPTYSTRING);
	cmd_options.addStringOption("maxmin", "mm", EMPTYSTRING);
	cmd_options.addStringOption("frame_window", "fw", "f");
	cmd_options.addStringOption("plugin", "p", EMPTYSTRING);
	cmd_options.addBoolOption("pluginmute", "pm", false);
}

void loadOptions() {
	helpopt = cmd_options.getBoolOption("help");
	usageopt = cmd_options.getBoolOption("usage");
	memSize = cmd_options.getNaturalOption("memory");
	winSize = cmd_options.getNaturalOption("nwinsamples");
	hopSize = cmd_options.getNaturalOption("nhopsamples");
	extractorName = cmd_options.getStringOption("extractor");
	slThreshold = cmd_options.getRealOption("slthreshold");
	numThreshold = cmd_options.getNaturalOption("numThreshold");
	nfrThreshold = cmd_options.getRealOption("nfrThreshold");
	classifierName = cmd_options.getStringOption("classifier");
	k = cmd_options.getNaturalOption("k");
	kernelType = cmd_options.getNaturalOption("kernelType");
	svmModelName = cmd_options.getStringOption("svmmodelname");
	maxMinFilename = cmd_options.getStringOption("maxmin");
	choose = cmd_options.getStringOption("frame_window");
	pluginName = cmd_options.getStringOption("plugin");
	pluginMute = cmd_options.getBoolOption("pluginmute");
}

void printUsage(string progName) {
	MRSDIAG("train_Speech_Non.cpp - printUsage");
	cerr << "Usage : " << progName
			<< " [-e extractor] [-h help] [-m memory]  [-u usage] collection1 collection2 ... collectionN"
			<< endl;
	cerr << endl;
}

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
	cerr
			<< "where file1, ..., fileN are sound files in a Marsyas supported format"
			<< endl;
	cerr << "Help Options:" << endl;
	cerr << "-u --usage		: 显示帮助信息(false)" << endl;
	cerr << "-h --help		: 显示用法信息(false)" << endl;
	cerr << "-m --memory		: 设置段大小(40)" << endl;
	cerr << "-ws --nwinsamples	: 设置每帧的采样点数(512)" << endl;
	cerr << "-hp --nhopsamples	: 设置实际处理的采样点数(512)" << endl;
	cerr << "-e --extractor		: 选择特征(BPSTFTMFCC)" << endl;
	cerr << "-sth --slthreshold	: 设置被确定为静音帧的阈值(0.01)" << endl;
	cerr << "-nth --numthreshold	: 设置静音帧数目阈值(20)" << endl;
	cerr << "-nfrth --nfrthreshold	: 设置被确定为噪音帧的阈值(0.003)" << endl;
	cerr << "-cl --classifier	: 选择分类器(GS)" << endl;
	cerr << "-k --k			: K近邻分类器中的k值(3)" << endl;
	cerr << "-ktp --kernel type	: SVM核函数(0)" << endl;
	cerr << "-mdl --SVM model name	:SVM模型文件名" << endl;
	cerr << "-mm --SVM maxmin name	:SVM maxmin文件名" << endl;
	cerr << "-fw --method		：选择决策方式(f)" << endl;
	cerr << "-p --plugin		: 输出plugin文件名(NULL)" << endl;
	cerr << "-pm --pluginmute	: 分类时是否播放文件(false)" << endl;

	cerr << "---------------------------附注----------------------------" << endl;
	cerr << "可支持的分类器:GS,KNN,SVM" << endl;
	cerr << "SVM核函数:0-线性 1-RBF 2-POLY 3-SIGMOID" << endl;
	cerr << "-----------------------------------------------------------"
			<< endl;
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

	string classNames = "";
	vector<Collection> cls;
	vector<string>::iterator sfi;
	vector<string> soundfiles = cmd_options.getRemaining();

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

	Collection single;
	single.concatenate(cls);

	if (choose == "f")
		train_frame(cls, single, pluginName, classNames, wekafname, memSize,
				extractorName, classifierName);
	else if (choose == "w")
		train_window(cls, single, pluginName, classNames, memSize,
				extractorName, classifierName);

	return 0;
}
