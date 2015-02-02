/*
 ** Copyright (C) 1998-2005 George Tzanetakis <gtzan@cs.uvic.ca>
 **
 ** This program is free software; you can redistribute it and/or modify
 ** it under the terms of the GNU General Public License as published by
 ** the Free Software Foundation; either version 2 of the License, or
 ** (at your option) any later version.
 **
 ** This program is distributed in the hope that it will be useful,
 ** but WITHOUT ANY WARRANTY; without even the implied warranty of
 ** MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 ** GNU General Public License for more details.
 **
 ** You should have received a copy of the GNU General Public License
 ** along with this program; if not, write to the Free Software
 ** Foundation, Inc., 59 Temple Place - Suite 330, Boston, MA 02111-1307, USA.
 */

#include "Collection.h"
#include <algorithm>

using namespace std;

Collection::Collection() {
	collectionList_.reserve(1000);
	hasLabels_ = false;
}

Collection::~Collection() {
}

void Collection::setName(string name) {
	name_ = name;
}

void Collection::read(string filename) {
	ifstream is(filename.c_str());
	name_ = filename.substr(0, filename.rfind(".", filename.length()));

	is >> (*this);
}

void Collection::write(string filename) {
	ofstream os(filename.c_str());
	os << (*this) << endl;
}

void Collection::labelAll(string label) {
	if (hasLabels_ == false) {
		hasLabels_ = true;
		for (unsigned int i = 0; i < collectionList_.size(); i++)
			labelList_.push_back(label);
	} else {
		for (unsigned int i = 0; i < collectionList_.size(); i++)
			labelList_[i] = label;
	}
}

ostream&
operator<<(ostream& o, const Collection& l) {
	// o << "# MARSYAS Collection " << endl;
	// o << "# name = " << l.name_ << endl << endl;
	for (unsigned int i = 0; i < l.collectionList_.size(); i++) {
		if (!l.hasLabels_)
			o << l.collectionList_[i] << endl;
		else
			o << l.collectionList_[i] << "\t" << l.labelList_[i] << endl;
	}
	o << endl;
	return o;
}

int Collection::size() {
	return (int) collectionList_.size();
}

string Collection::name() {
	return name_;
}

void Collection::add(string entry) {
	collectionList_.push_back(entry);
}

void Collection::add(string entry, string label) {
	collectionList_.push_back(entry);
	labelList_.push_back(label);

	vector<string>::const_iterator it;
	it = find(labelNames_.begin(), labelNames_.end(), label);
	if (it == labelNames_.end()) // label not found add it to list
		labelNames_.push_back(label);

}

int Collection::getSize() {
	int size = collectionList_.size();
	return size;
}

int Collection::getNumLabels() {
	return labelNames_.size();
}

string Collection::labelName(int i) {
	if ((unsigned) i < labelNames_.size())
		return labelNames_[i];

	return EMPTYSTRING;
}

string Collection::getLabelNames() {
	string labelNames;
	vector<string>::iterator vi;
	for (vi = labelNames_.begin(); vi != labelNames_.end(); ++vi)
		labelNames += (*vi + ',');
	return labelNames;
}

bool Collection::hasLabels() {
	return hasLabels_;
}

void Collection::shuffle() {
	unsigned int i;
	unsigned int size = collectionList_.size();
	unsigned int rind;
	string temp;

	for (i = 0; i < size; i++) {
		rind = (unsigned int) (((double) rand() / (double) (RAND_MAX)) * size);
		temp = collectionList_[i];
		collectionList_[i] = collectionList_[rind];
		collectionList_[rind] = temp;
		if (hasLabels_) {
			temp = labelList_[i];
			labelList_[i] = labelList_[rind];
			labelList_[rind] = temp;
		}
	}
}

string Collection::toLongString() {
	string result;

	vector<string>::iterator sfi;
	for (sfi = collectionList_.begin(); sfi != collectionList_.end(); ++sfi) {
		string sfname = *sfi;
		result += sfname + ',';
	}
	return result;
}

int Collection::labelNum(string label) {
	unsigned int i;
	for (i = 0; i < labelNames_.size(); i++) {
		if (labelNames_[i] == label)
			return i;
	}
	return 0;
}

string Collection::labelEntry(unsigned int i) {
	if (hasLabels_)
		return labelList_[i];
	return "No label";
}

string Collection::entry(unsigned int i) {
	return collectionList_[i];
}

void Collection::concatenate(vector<Collection> cls) {
	unsigned int cj;
	unsigned int i;

	for (cj = 0; cj < cls.size(); cj++) {
		Collection l = cls[cj];
		if (l.hasLabels_) {
			hasLabels_ = true;
		}
		for (i = 0; i < l.size(); i++)
			add(l.entry(i), l.labelEntry(i));
	}
}

istream&
operator>>(istream& i, Collection& l) {

	MRSDIAG("Collection.cpp - operator>>");
	string str0, str1, str2;
	string fileEntry;
	string fname;

	while (getline(i, fileEntry)) {
		if (fileEntry.empty()) {
			; // do nothing
		} else {
			char c = fileEntry[0];
			istringstream iss(fileEntry);
			iss >> fname;
			string::size_type loc = fileEntry.find("\t", 0);
			if (loc != string::npos) {
				if (c != '#') {
					l.collectionList_.push_back(fileEntry.substr(0, loc));
					string label = fileEntry.substr(loc + 1, fileEntry.size());
					l.labelList_.push_back(label);

					vector<string>::const_iterator it;
					it = find(l.labelNames_.begin(), l.labelNames_.end(),
							label);
					if (it == l.labelNames_.end()) // label not found add it to list
						l.labelNames_.push_back(label);

					l.hasLabels_ = true;
				}

			} else {
				if (c != '#') {
					l.collectionList_.push_back(fileEntry);
				}
			}
		}
	}

	return i;
}

