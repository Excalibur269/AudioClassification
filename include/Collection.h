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

#ifndef MARSYAS_COLLECTION_H
#define MARSYAS_COLLECTION_H

#include <string> 
#include <vector>
#include <iostream>
#include <fstream>
#include <sstream>
#include "common.h"

/**
 \class Collection
 \ingroup NotmarCore
 \brief List of files

 List of files used for batch processing.
 */

class Collection {
private:
	std::vector<std::string> collectionList_;
	std::vector<std::string> labelList_;
	std::vector<std::string> labelNames_;
	std::string name_;
	bool hasLabels_;
public:
	Collection();
	~Collection();
	int size();
	void setName(std::string name);
	std::string entry(unsigned int i);
	std::string labelEntry(unsigned int i);
	int getSize();
	int getNumLabels();
	std::string getLabelNames();
	int labelNum(std::string label);
	std::string labelName(int i);
	bool hasLabels();
	void add(std::string entry);
	void add(std::string entry, std::string label);
	std::string name();
	void shuffle();
	void read(std::string filename);
	void write(std::string filename);
	void labelAll(std::string label);
	std::string toLongString();

	void concatenate(std::vector<Collection> cls);

	friend std::ostream& operator<<(std::ostream&, const Collection&);
	friend std::istream& operator>>(std::istream&, Collection&);
};

#endif
