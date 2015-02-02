/*
 * ReadFeature.cpp
 *
 *  Created on: 2015年1月30日
 *      Author: zhangbihong
 */
#include "ReadFeature.h"
#include <fstream>
using namespace std;

void readMFCC(std::string filename, realvec& in) {
	ifstream readMFCCFile;
	readMFCCFile.open((filename + "mfc").c_str());
	char line[1024] = { 0 };
	int row  = 0, col = 0;
	while(readMFCCFile.getline(line, sizeof(line))){
		stringstream word(line);
		word >> in (row, col);
		col++;
		if(col == 13){
			col = 0;
			row++;
		}
	}
}
void readLSP(std::string filename, realvec& in) {
	ifstream readLSPFile;
	readLSPFile.open((filename + "lsp").c_str());
	char line[1024] = { 0 };
	int col = 0;
	while(readLSPFile.getline(line, sizeof(line))){
		stringstream word(line);
		word >> in(0, col++);
	}
}
void readLPCC(std::string filename, realvec& in) {
	ifstream readLPCCFile;
	readLPCCFile.open((filename + ".lpcc").c_str());
	char line[1024] = { 0 };
	int col = 0;
	while(readLPCCFile.getline(line, sizeof(line))){
		stringstream word(line);
		word >> in(0, col++);
	}
}

void readZCR(std::string filename, realvec& in) {
	ifstream readZCRFile;
	readZCRFile.open((filename + ".zcr").c_str());
	int lines = 0;
	char line[1024] = { 0 };
	while (readZCRFile.getline(line, sizeof(line))) {
		lines++;
	}
	readZCRFile.close();
	readZCRFile.open((filename + ".zcr").c_str());
	in.create(1, lines);
	int count = 0;
	while (readZCRFile.getline(line, sizeof(line))) {
		stringstream word(line);
		word >> in(0, count++);
	}
}

