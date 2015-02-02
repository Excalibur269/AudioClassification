/*
 * Tools.cpp
 *
 *  Created on: 2014-5-6
 *      Author: pachira
 */
#include "ACTools.h"

using namespace std;
using namespace alize;


String getPath(const char *file)
{
    string File(file);
    int sepPath = File.find_last_of("/");

    String s;

    // 若 '/' 未找到，默认为当前目录
    // string::npos 是静态成员，线程不安全
    if (!(sepPath >= 0 && sepPath < File.length())) {
        s = "./";
    }
    else {
        s = (File.substr(0, sepPath + 1)).c_str();
    }

    return s;
}

String getName(const char *file)
{
    string File(file);
    int sepPath = File.find_last_of("/");
    int sepName = File.find_last_of(".");

    // 若 '/' 未找到，默认为当前目录
    // string::npos 是静态成员，线程不安全
    if (!(sepPath >= 0 && sepPath < File.length())) {
        sepPath = -1;
    }

    // 若 '.' 未找到或不比sepPath大2以上，或是最后一个字符，
    // 默认为无后缀名
    // string::npos 是静态成员，线程不安全
    if (!(sepName >= 0 && sepName < File.length() - 1) ||
        (sepName - sepPath < 2)) {
        sepName = File.length();
    }

    String s = (File.substr(sepPath + 1, sepName - sepPath - 1)).c_str();

    return s;
}

String getSuffix(const char *file)
{
    string File(file);
    int len = File.length();
    int sepPath = File.find_last_of("/");
    int sepName = File.find_last_of(".");

    // 若 '/' 未找到，默认为当前目录
    // string::npos 是静态成员，线程不安全
    if (!(sepPath >= 0 && sepPath < len)) {
        sepPath = -1;
    }

    // 若 '.' 未找到或不比sepPath大2以上，或是最后一个字符，
    // 则视为无后缀名
    // string::npos 是静态成员，线程不安全
    if (!(sepName >= 0 && sepName < len - 1) ||
        (sepName - sepPath < 2)) {
        sepName = len;
    }

    String s;
    if (sepName == len || sepName == len - 1)
        s = "";
    else
        s = (File.substr(sepName, len - sepName)).c_str();

    return s;
}

void setConfigChecker(ConfigChecker &cc)
{
    cc.addFloatParam("frameLength", true, true,
                     "defines length of a frame (default=10ms)");
    cc.addIntegerParam("nbTrainIt", true, true,
                       "defines the number of it, the ceiling and flooring are moved and the baggedFrameProbability is used");
    cc.addFloatParam("baggedFrameProbabilityInit", false, true,
                     "defines the % of frames taken by component for the initializing of the mixture - mandatory if init from scratch");
    cc.addFloatParam("baggedFrameProbability", true, true,
                     "defines the % of frames taken for each iterations");
    cc.addFloatParam("initVarianceFlooring", true, true,
                     "defines the variance control parameters - relative to global data variance - initial value (moved during the it)");
    cc.addFloatParam("initVarianceCeiling", true, true,
                     "defines the variance control parameters - relative to global data variance - initial value (moved during the it)");
    cc.addFloatParam("finalVarianceFlooring", true, true,
                     "defines the variance control parameters - relative to global data variance - final value");
    cc.addFloatParam("finalVarianceCeiling", true, true,
                     "defines the variance control parameters - relative to global data variance - final value");
    cc.addStringParam("labelFilesPath", false, true,
                      "defines the path where to load original labelFiles");
    cc.addStringParam("labelFilesExtension", false, true,
                      "defines the extension to load original labelFiles");
    //  cc.addStringParam("labelSelectedFrames",false,true,"only the frames from segments with this label will be used");

    // For the Segmentation Algorithm
    cc.addStringParam("listFileToSegment", true, true,
                      "defines the list of the files to segment");
    cc.addStringParam("outputFilesPath", true, true,
                      "defines the path where will be to store the produced files");
    cc.addStringParam("fileRefPath", false, true,
                      "defines the path of the reference files");
    cc.addStringParam("saveSegmentationExtension", true, true,
                      "defines the extension to save the files of the label");
    cc.addStringParam("clusteringCrit", false, true,
                      "defines the criteria (BIC, GLR, DGLR or DELTABIC) for the clustering (default=DGLR)");
    cc.addFloatParam("clusteringCritThresh", false, true,
                     "defines the threshold value for the clustering (default=0.0)");
    cc.addIntegerParam("winSize", false, true,
                       "defines the size of the window for the clustering (default=50)");
    cc.addIntegerParam("winStep", false, true,
                       "defines the step of the window for the clustering (default=5)");
    cc.addFloatParam("alpha", false, true,
                     "factor to find the maxima of the criterion value curve : alpha * standard deviation (default=0.7)");
    //For Clustering
    cc.addIntegerParam("sampleRate", true, true,
                       "Input audio sample rate(6 or 8)");
    cc.addFloatParam("clusteringThreshold", false, true,
                     "Clustering stopping threshold (default=-1.35)");
    cc.addBooleanParam("loadWorldFromExternalFile", false, true,
                       "if set to true, load the model defined by the parameter worldModel, otherwise, compute a world model directly from the targeted signal");
    cc.addStringParam("worldModelfor6k", false, true,
                      "defines the filename of the world model for 6k sample rate");
    cc.addStringParam("worldModelfor8k", false, true,
                      "defines the filename of the world model for 8k sample rate");

    cc.addStringParam("selectionMethod", false, true,
                      "defines a selection mathod : firstSegmentFound (default), firstLimitedLengthSegmentFound, longestSegmentFound, longestLimitedLengthSegmentFound");
    cc.addIntegerParam("selectionLength", true, true,
                       "defines the minimum size of the segment selected according to the selectionMethod");
    cc.addIntegerParam("limitedLength", true, true,
                       "defines the maximum size of the segment selected according to the selectionMethod. This parameter is necessary for both firstLimitedLengthSegmentFound and longestLimitedLengthSegmentFound selection methods");
    cc.addIntegerParam("speakerMinTime", true, true,
                       "defines a minimal length of speech to fix the relevancy of a new speaker");
    cc.addIntegerParam("limitSpeaker", false, true,
                       "defines the maximal number of segments by file (default=2)");
    cc.addStringParam("trainAlgo", false, true,
                      "defines the method of training of the models : MAP or EM (default)");
    cc.addStringParam("testAlgo", false, true,
                      "defines the method of decoding : Viterbi (default)");
    cc.addStringParam("wavfilepath",true,true,
                      "defines the path of the wavfiles to segment");
    cc.addStringParam("featureFilesPath", true, true,
                      "defines the path of the mfcfeature to segment");
    cc.addStringParam("loadAudioFileExtension",true,true,
                      "defines the extension of the audio");
    cc.addStringParam("loadFeatureFileExtension",true,true,
                      "defines the suffix of the feature files");
}
