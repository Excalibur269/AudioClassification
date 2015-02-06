#ifndef MARSYAS_WEKADATA_H
#define MARSYAS_WEKADATA_H 

#include <assert.h>
#include <list>
#include <vector>
#include <cfloat> 
#include <iostream>
#include <stdlib.h>
#include <cstring>

#include "realvec.h"
//This class represents a collection of row data read from a weka arff file.
//It is organized as a vector collection of vector pointers.
//It is optimized for fast sorting and shuffling of the data. It is not intended
//that the data change once it is loaded.
//
//It is also assumed that the last column of each row is the class attribute.
//All data items are mrs_real, including the class attribute, however the class
//attribute should be interpreted as an int.
class WekaData : public std::vector<std::vector<double>*>
{
public:
	WekaData();
	virtual ~WekaData();

	//create the table. Will clear contents first and fix the number of columns.
	void Create(int cols);

	//clear all data from the table
	void Clear();

	//randomly shuffle the data in the table
	void Shuffle();

	// NormMaxMin normalize the data with minimums and maximums
	void NormMaxMin(std::string MaxMinFilename);

	void NormMaxMinRow(realvec& in,std::string MaxMinFilename);
	
	//sort the table based on an attribute. sorts in ascending order.
	void Sort(int attIndex);

	//add rows of data to the table
	void Append(const realvec& in);
	void Append(std::vector<double> *);

	//return the number of columns(including the class attribute)
	inline int getCols()const{return cols_;}
	inline int getRows()const{return rows_;}

	//get the class attribute for a row and convert to a int
	int GetClass(int row)const;

	//debug helper funtion to dump table to an ascii file
	void Dump(const std::string& filename, const std::vector<std::string>& classNames)const;


private:
	int cols_;
	int rows_;
	realvec minimums_;
	realvec maximums_;
	int flag;
	
	//some sorting private functions
	void quickSort(int attIndex, int left, int right);
	void swapRows(int l, int r);
	int partition(int attIndex, int l, int r);
};//class WekaData

#endif
