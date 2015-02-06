#include "WekaData.h"

using namespace std;

//This class represents a collection of row data read from a weka arff file.
//It is organized as a vector collection of vector pointers.
//It is optimized for fast sorting and shuffling of the data. It is not intended
//that the data change once it is loaded.
//
//It is also assumed that the last column of each row is the class attribute.
//All data items are double, including the class attribute, however the class
//attribute should be interpreted as an int.
WekaData::WekaData():cols_(0),rows_(0)
{
}

WekaData::~WekaData()
{
}

//create the table. Will clear contents first and fix the number of columns.
void WekaData::Create(int cols)
{
	assert(cols>=0);
	this->Clear();
	cols_ = cols;
	rows_ = 0;
	flag = 0;
	
}


//clear all data from the table
//Requires that the vector rows be freed
void WekaData::Clear()
{
  
	for(vector<vector<double>*>::iterator iter = this->begin(); iter!=this->end(); iter++)
	{
		delete (*iter);
	}
	this->clear();
}//Clear


void 
WekaData::NormMaxMinRow(realvec& in,string MaxMinFilename)
{
  int ii;

  int size = 0;
  char s[20]="";
  char ch_maxminfile[50]="";

  strcpy(ch_maxminfile, MaxMinFilename.c_str());

  if((flag != 0)&&(flag !=1))
	flag = 0;  

  if(flag ==0)
  {

	FILE *fp = fopen(ch_maxminfile,"r");
	if (fp==NULL)
	{
		cout<<"can not open file maxmin."<<endl;
		exit(0);
	}

	fscanf(fp,"%s",s);
	fscanf(fp,"%s %d",&s,&size);

	minimums_.create(size);
	maximums_.create(size);
	maximums_.setval(DBL_MIN);
	minimums_.setval(DBL_MAX);


	for(ii=0;ii<size;ii++)
		fscanf(fp,"%lf",&maximums_(ii));

	fscanf(fp,"%s",s);
	fscanf(fp,"%s %d",&s,&size);

	for(ii=0;ii<size;ii++)
		fscanf(fp,"%lf",&minimums_(ii));

	fclose(fp);

	flag = 1;

  }	

	  for(ii=0; ii<(int)in.getSize()-1; ii++)
	  {
		  in(ii) =  (in(ii) - minimums_(ii)) / (maximums_(ii) - minimums_(ii));
	  }
 

}

void 
WekaData::NormMaxMin(string MaxMinFilename)
{
  minimums_.create(cols_-1);
  maximums_.create(cols_-1);
  maximums_.setval(DBL_MIN);
  minimums_.setval(DBL_MAX);
  
  // find minimums_ and maximums_ 
  for(vector<vector<double>*>::const_iterator citer = this->begin(); citer!=this->end(); citer++)
    {
      const vector<double> *row = (*citer);
      int ii;
      for(ii=0; ii<(int)row->size()-1; ii++)
	{
	  if (row->at(ii) > maximums_(ii))
	    maximums_(ii) = row->at(ii);
	  if (row->at(ii) < minimums_(ii))
	    minimums_(ii) = row->at(ii);
	}

    }
//   cout <<"maximums_"<<maximums_<<endl;
//   cout <<"minimums_"<<minimums_<<endl;


  
/*cout <<"ininininininin"<<endl;*/

  // normalize 
  for(vector<vector<double>*>::const_iterator citer = this->begin(); citer!=this->end(); citer++)
    {
      vector<double> *row = (*citer);
      int ii;
      for(ii=0; ii<(int)row->size()-1; ii++)
	{
	  row->at(ii) =  ((row->at(ii) - minimums_(ii)) / (maximums_(ii) - minimums_(ii)));
	}
    }

  char ch_maxminfile[50]="";

  strcpy(ch_maxminfile, MaxMinFilename.c_str());	

  FILE *fp=fopen(ch_maxminfile,"w");
  if(fp==NULL)
  {
	cout<<"can not open file maxmin."<<endl;
	exit(0);
  }

  fprintf(fp,"maximums\nsize: %d\n",maximums_.getSize());

  for(int jj=0;jj<maximums_.getSize();jj ++)
	  fprintf(fp,"%lf  ",maximums_(jj));

  fprintf(fp,"\nminimums\nsize: %d\n",minimums_.getSize());

  for(int jj=0;jj<minimums_.getSize();jj ++)
	  fprintf(fp,"%lf  ",minimums_(jj));

  fclose(fp);

}


//randomly shuffle the data in the table
//Need only to swap the pointers to row data, nice and fast!
void WekaData::Shuffle()
{
	int size = this->size()-1;
	for (int ii=0; ii<size; ii++)
	{
		int rind = (int)(((double)rand() / (double)(RAND_MAX))*size);
		//swap row ii with row rind
		swapRows(ii, rind);
	}//for ii
}//Shuffle

//SwapRows will exchange one row for another.
//Just need to swap the 2 vector pointers.
void WekaData::swapRows(int l, int r)
{
	vector<double> *temp = this->at(l);
	this->at(l) = this->at(r);
	this->at(r) = temp;
}

int WekaData::partition(int attIndex, int l, int r)
{
	double pivot = this->at((l+r)/2)->at(attIndex);
	while (l < r)
	{
		while ((this->at(l)->at(attIndex) < pivot) && (l < r))
		{
			l++;
		}//while

		while ((this->at(r)->at(attIndex) > pivot) && (l < r))
		{
			r--;
		}//while

		if (l < r)
		{
			swapRows(l, r);
			l++;
			r--;
		}//if
    }
	if ((l == r) && (this->at(r)->at(attIndex) > pivot))
	{
		r--;
	} //if

	return r;
}//partition

  /**
   * Implements quicksort according to Manber's "Introduction to
   * Algorithms".
   *
   * @param attIndex the attribute's index
   * @param left the first index of the subset to be sorted
   * @param right the last index of the subset to be sorted
   */
  //@ requires 0 <= attIndex && attIndex < numAttributes();
  //@ requires 0 <= first && first <= right && right < numInstances();
  //Shamelessly ripped off from the weka library of code. - dale
void WekaData::quickSort(int attIndex, int left, int right)
{
	if (left < right)
	{
		int middle = partition(attIndex, left, right);
		quickSort(attIndex, left, middle);
		quickSort(attIndex, middle + 1, right);
	}//if
}//quicksort

//Sort the instances dataset based on the column attr
//Note that the entire table must be sorted on the attribute,
//not just the attribute itself.
void WekaData::Sort(int attr)
{
	assert(attr>=0&&attr<cols_);
	quickSort(attr, 0, this->size()-1);
}

//add rows of data to the table
void WekaData::Append(const realvec& in)
{
  assert(in.getRows()==cols_);
  vector<double> *data = new vector<double>(cols_);
  for(int ii=0; ii<in.getRows(); ii++)
    {
      data->at(ii) = in(ii, 0);
    }
  Append(data);
}//Append

//add rows of data to the table
void WekaData::Append(vector<double> *data)
{
  rows_++;
	assert(data!=NULL&&data->size()==cols_);
	this->push_back(data);
}//Append

//get the class attribute for a row and convert to a int
//class attribute is last column of row
int WekaData::GetClass(int row) const
{
	return (int)this->at(row)->at(cols_-1);
}

//debug helper funtion to dump table to an ascii file
void WekaData::Dump(const string& filename, const vector<string>& classNames) const
{
	char buffer[32];

	ofstream *mis = new ofstream;

	mis->open(filename.c_str(), ios_base::out | ios_base::trunc );
	assert( mis->is_open() );

	for(vector<vector<double>*>::const_iterator citer = this->begin(); citer!=this->end(); citer++)
	{
		bool first = true;
		const vector<double> *row = (*citer);
		int ii;
		for(ii=0; ii<(int)row->size()-1; ii++)
		{
			if(!first)
				mis->write(", ", 2);
			first = false;

			sprintf(buffer, "%09.4f", row->at(ii));
			mis->write(buffer, strlen(buffer));
		}
		mis->write(", ", 2);
		int classIndex = (int)row->at(ii);
		mis->write(classNames[classIndex].c_str(), strlen(classNames[classIndex].c_str()));
		mis->write("\n", 1);
	}

	mis->close();
}//Dump
