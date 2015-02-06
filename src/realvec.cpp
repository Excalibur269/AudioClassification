/*
** Copyright (C) 1998-2006 George Tzanetakis <gtzan@cs.uvic.ca>
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

#include "realvec.h"
//#include "NumericLib.h"
#include <algorithm>
#include <limits>


using namespace std;


/// constructor
realvec::realvec()
{
	size_ = 0;
	allocatedSize_ = 0;
	data_ = NULL;
	rows_ = 0;  // [!! 0]
	cols_ = size_;
}

realvec::~realvec()
{
	if (size_ == 0)
		data_ = NULL;

	delete [] data_;
	data_ = NULL;
}

realvec::realvec(int size)
{
	data_ = NULL;
	size_ = size;
	allocatedSize_ = size;
	if (size_ > 0) 
		data_ = new double[size_];
	rows_ = 1;
	cols_ = size_;
}

realvec::realvec(int rows, int cols)
{
	data_ = NULL;
	size_ = rows * cols;
	allocatedSize_ = size_;
	if (size_ > 0) 
		data_ = new double[size_];
	rows_ = rows;
	cols_ = cols;
}

realvec::realvec(const realvec& a) : size_(a.size_),allocatedSize_(a.size_), 
rows_(a.rows_), cols_(a.cols_)
{
	data_ = NULL; 
	if( a.size_ > 0 )
		data_ = new double[a.size_];
	for (int i=0; i<size_; i++)
		data_[i] = a.data_[i];

}

realvec& 
realvec::operator=(const realvec& a)
{
	if (this != &a)
	{
		//check if we need to allocate more memory
		if (allocatedSize_ < a.size_)
		{
			//if data_ is not NULL, delete it
			delete [] data_;
			data_ = NULL;

			allocatedSize_ = 0;
			size_ = 0;

			//allocate memory, if size > 0
			if(a.size_ > 0)
			{
				data_ = new double[a.size_];
				allocatedSize_ = a.size_; //"a" may have more allocated memory than its current size!
				size_ = a.size_;
			}
		}

		//copy data
		for (int i=0; i < a.size_; i++)
			data_[i] = a.data_[i];
		
		//update internal parameters
		size_ = a.size_;
		//allocatedSize_ = a.allocatedSize_; //!!
		rows_ = a.rows_;
		cols_ = a.cols_;
	}

	return *this;
}

double *
realvec::getData() const
{
	return data_;
}

void
realvec::appendRealvec(const realvec newValues)
{
	int origSize = size_;

  stretch(origSize + newValues.getSize());

  for (int i=0; i<newValues.getSize(); i++)
		data_[origSize + i] = newValues.data_[i];
}

realvec
realvec::getSubVector(int startPos, int length) const
{
	realvec subVector(length);
	for (int i=0; i<length; i++)
		subVector.data_[i] = data_[startPos + i];
	return subVector;
}

void
realvec::transpose()
{
	double *tmp_ = new double[size_];

	for (int i=0; i < rows_; i++)
		for (int j=0; j < cols_; j++)
			tmp_[i * cols_ + j] = data_[j * rows_ + i];

	int tmp = rows_;
	rows_ = cols_;
	cols_ = tmp;

	delete [] data_;
	data_ = tmp_;
}

double
realvec::median() const
{
	realvec tmp(*this);
	double *tmpData = tmp.data_;
	std::sort(tmpData, tmpData+size_);
	return tmpData[size_/2];
}

double
realvec::mean() const
{
	double sum = 0.0;
	for (int i=0; i < size_; i++)
	{
		sum += data_[i];
	}
	if (sum != 0.0) sum /= size_;
	return sum;
}

void
realvec::sort() //assumes one dimensional vector only!
{
	std::sort(data_, data_+size_);
}

double
realvec::sum() const
{
	double sum = 0.0;
	for (int i=0; i < size_; i++)
	{
		sum += data_[i];
	}
	return sum;
}


double
realvec::var() const
{
	double sum = 0.0;
	double sum_sq = 0.0;
	double val;
	double var;

	for (int i=0; i < size_; i++)
	{
		val = data_[i];
		sum += val;
		sum_sq += (val * val);
	}
	if (sum != 0.0) sum /= size_;
	if (sum_sq != 0.0) sum_sq /= size_;
	
	var = sum_sq - sum * sum;

	if (var < 0.0) 
	  var = 0.0;
	return var;
}

double
realvec::std() const
{
  double vr = var();
  if (vr != 0) 
    return sqrt(vr);
  else 
    return 0.0;
}

int
realvec::getRows() const
{
	return rows_;
}

int
realvec::getCols() const
{
	return cols_;
}

int
realvec::getSize() const
{
	return size_;
}

void 
realvec::debug_info()
{
//	MRSERR("realvec information");
//	MRSERR("size = " + size_);

}

/* keep the old data and possibly extend */ 
void 
realvec::stretch(int size)
{
	if (size_ == size) 
		return; //no need for more memory allocation

	if(size < allocatedSize_)
	{
		size_ = size;
		rows_ = 1;
		cols_ = size_;
		return; //no need for more memory allocation
	}

	//we need more memory allocation!
	double* ndata = NULL;
	if (size > 0) 
		ndata = new double[size];

	// zero new data, but keep existing data 
	for (int i=0; i < size; i++)
	{
		if (i < size_) 
			ndata[i] = data_[i]; //copy existing data
		else 
			ndata[i] = 0.0; //zero new data
	}
	
	//deallocate existing memory...
	delete [] data_;
	//...and point to new data memory (if any - it can be NULL, when size == 0)
	data_ = ndata;
	
	//update internal parameters
	size_ = size;
	allocatedSize_ = size;
	rows_ = 1;
	cols_ = size;
}

/* keep the old data and possibly extend */ 
void 
realvec::stretch(int rows, int cols)
{
	int size = rows * cols;

	if ((rows == rows_)&&(cols == cols_))
		return;

	/*if(size < size_)  [!] should be improved as the simpler stretch function
	{
	size_ = size;
	rows_ = rows;
	cols_ = cols;
	return;
	}*/

	double *ndata = NULL;

	if (size > 0) 
		ndata = new double[size];

	// copy and zero new data 
	for (int r=0; r < rows; r++)
		for (int c = 0; c < cols; c++)
		{
			if ((r < rows_)&&(c < cols_)) 
				ndata[c * rows + r] = data_[c * rows_ + r];
			else 
				ndata[c * rows + r] = 0.0;
		}
		if (data_) 
		{
			delete [] data_;
			data_ = NULL;
		}
		data_ = ndata;
		size_ = size;
		allocatedSize_ = size;
		rows_ = rows;
		cols_ = cols;
}

void
realvec::stretchWrite(const int pos, const double val)
{
	// don't forget the zero-indexing position!
	//   i.e.  pos=0  is the first value to store
	int wantSize = pos+1;
	if (wantSize > size_)
		if ( wantSize < 2*size_ )
			// grow exponentially with sequential access
			stretch( 2*size_ );
		else
			// if we have a sudden large value, don't double it
			stretch( wantSize );
	// FIXME: add a assert here for debugging.
	// cout<<"List stretched to "<<size_<<endl;
	data_[pos] = val;
}

void
realvec::stretchWrite(const int r, const int c, const double val)
{
	int nextR = rows_;
	int nextC = cols_;
	int wantR = r+1;
	int wantC = c+1;
	if ( (wantR >= rows_) || (wantC >= cols_) ) {
		if ( wantR >= rows_ )
			if ( wantR < 2*rows_ )
				nextR = 2*rows_;
			else
				nextR = wantR;

		if ( wantC >= cols_ )
			if ( wantC < 2*cols_ )
				nextC = 2*cols_;
			else
				nextC = wantC;

		stretch( nextR, nextC );
		// FIXME: add a assert here for debugging.
		// cout<<"List stretched to "<<rows_<<", "<<cols_<<endl;
	}
	data_[c * rows_ + r] = val;
}

void 
realvec::create(int size)
{
	delete [] data_;
	data_ = NULL;
	size_ = size;
	allocatedSize_ = size;
	rows_ = 1;
	cols_ = size;
	if (size_ > 0) 
		data_ = new double[size_];
	for (int i=0; i<size_; i++)
		data_[i] = 0.0;
}

void 
realvec::create(int rows, int cols)
{
	delete [] data_;
	data_ = NULL;
	size_ = rows * cols;
	rows_ = rows;
	cols_ = cols;
	allocatedSize_ = size_;
	if (size_ > 0) 
		data_ = new double[size_];
	for (int i=0; i<size_; i++)
		data_[i] = 0.0;
}

void 
realvec::create(double val, int rows, int cols)
{
	size_ = rows * cols;
	rows_ = rows;
	cols_ = cols;
	delete [] data_;
	data_ = NULL;
	if (size_ > 0) 
		data_ = new double[size_];
	for (int i=0; i<size_; i++)
		data_[i] = val;
	allocatedSize_ = size_;
}

void 
realvec::allocate(int size)
{
	delete [] data_;
	data_ = NULL;
	size_ = size;
	cols_ = size_;
	rows_ = 1;
	allocatedSize_ = size;
	if (size_ > 0) 
		data_ = new double[size_];
}

void
realvec::allocate(int rows, int cols)
{
	delete [] data_;
	data_ = NULL;
	size_ = rows*cols;
	cols_ = cols;
	rows_ = rows;
	allocatedSize_ = size_;
	if (size_ > 0) 
		data_ = new double[size_];
}

void 
realvec::setval(int start, int end, double val)
{
	assert(start <= size_);
	assert(end <= size_);

	for (int i=start; i<end; i++)
	{
		data_[i] = val;
	}
}

void
realvec::apply(double (*func) (double))
{
	for (int i=0; i<size_; i++)
	{
		data_[i] = func(data_[i]);
	}
}

void 
realvec::setval(double val)
{
	for (int i=0; i<size_; i++)
	{
		data_[i] = val;
	}
}

void 
realvec::abs()
{
	for (int i=0; i<size_; i++)
	{
		data_[i] = fabs(data_[i]);
	}
}

void
realvec::norm()
{
	double mean = this->mean();
	double std = this->std();
	for(int i=0; i < size_; i++)
	{
		data_[i] = (data_[i] - mean) / std;
	}
}


void 
realvec::normMaxMin() 
{
	double max = DBL_MIN;
	double min = DBL_MAX;
	
	for (int i=0; i < size_; i++)
	{
		if (data_[i] > max)
			max = data_[i];
		if (data_[i] < min) 
			min = data_[i];
	}
	

	for (int i=0; i < size_; i++)
	{	
		data_[i] = (data_[i] - min) / (max - min);
	}
	
	
}


void
realvec::norm(double mean, double std)
{
	for(int i=0; i < size_; i++)
	{
		data_[i] = (data_[i] - mean) / std;
	}
}

void 
realvec::renorm(double old_mean, double old_std, double new_mean, double new_std)
{
	int i;
	for(i=0; i < size_; i++)
	{
		data_[i] = (data_[i] - old_mean) / old_std;
		data_[i] *= new_std;
		data_[i] += new_mean;
	}
}

int
realvec::invert(realvec& res)
{
	if(rows_ != cols_)
	{
//		MRSERR("realvec::invert() - matrix should be square!");
		res.create(0);
		return -1;
	}
	if (this != &res)
	{
		int rank;
		int r,c,i;
		double temp;

		res.create(rows_, cols_);

		rank = 0;
		for (r = 0; r < rows_; r++)
			for (c=0; c < cols_; c++)
			{
				if (r == c) 
					res(r,c) = 1.0;
				else 
					res(r,c) = 0.0;
			}
			for (i = 0; i < rows_; i++)
			{
				if ((*this)(i,i) == 0)
				{
					for (r = i; r < rows_; r++)
						for (c = 0; c < cols_; c++)
						{
							(*this)(i,c) += (*this)(r,c);
							res(i,c) += res(r,c);
						}
				}
				for (r = i; r < rows_; r++) 
				{
					temp = (*this)(r,i);
					if (temp != 0) 
						for (c =0; c < cols_; c++)
						{
							(*this)(r,c) /= temp;
							res(r,c) /= temp;
						}
				}
				if (i != rows_-1)
				{
					for (r = i+1; r < rows_; r++)
					{
						temp = (*this)(r,i);
						if (temp != 0.0) 
							for (c=0; c < cols_; c++)
							{
								(*this)(r,c) -= (*this)(i,c);
								res(r,c) -= res(i,c);
							}
					}
				}
			}
			for (i=1; i < rows_; i++)
				for (r=0; r < i; r++)
				{
					temp = (*this)(r,i);
					for (c=0; c < cols_; c++)
					{
						(*this)(r,c) -= (temp * (*this)(i,c));
						res(r,c) -= (temp * res(i,c));
					}
				}
				for (r =0; r < rows_; r++)
					for (c=0; c < cols_; c++)
						(*this)(r,c) = res(r,c);
				return rank;
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::invert() - inPlace operation not supported - returning empty result vector!");
		return -1;
	}
}

void
realvec::sqr()
{
	for (int i=0; i<size_; i++)
	{
		data_[i] *= data_[i];
	}
}

int
realvec::search(double val)
{
	double minDiff = MAXREAL;
	int index=-1;
	for (int i=0; i<size_; i++)
		if (fabs(data_[i]-val)< minDiff)
		{
			minDiff = fabs(data_[i]-val);
			index=i;
		}
		return index;
}

void
realvec::sqroot()
{
	for (int i=0; i<size_; i++)
	{
		data_[i] = sqrt(data_[i]);
	}
}

realvec 
operator+(const realvec& vec1, const realvec& vec2)
{
	int size;
	int i;
	if (vec1.size_ != vec2.size_)
//		MRSERR("Size of realvecs does not match");
	if (vec1.size_ >= vec2.size_)
		size = vec1.size_;
	else 
		size = vec2.size_;
	realvec sum;
	sum.create(size);    

	for (i=0; i<vec1.size_; i++)
	{
		sum.data_[i] = vec1.data_[i];
	}
	for (i=0; i<vec2.size_; i++)
	{
		sum.data_[i] += vec2.data_[i];
	}

	return sum;
}

realvec 
operator-(const realvec& vec1, const realvec& vec2)
{
	int size;
	int i;
	if (vec1.size_ != vec2.size_)
//		MRSERR("Size of realvecs does not match");
	if (vec1.size_ >= vec2.size_)
		size = vec1.size_;
	else 
		size = vec2.size_;
	realvec diff;
	diff.create(size);    

	for (i=0; i<vec1.size_; i++)
	{
		diff.data_[i] = vec1.data_[i];
	}
	for (i=0; i<vec2.size_; i++)
	{
		diff.data_[i] -= vec2.data_[i];
	}

	return diff;
}

bool
operator!=(const realvec& v1, const realvec& v2)
{
	//if vectors have the same dimensions, compare all their values
	if(v1.cols_ == v2.cols_ && v1.rows_ == v2.rows_)
	{
		for(int r = 0; r < v1.rows_; ++r)
			for (int c = 0; c < v1.cols_; ++c)
				if(v1(r,c) != v2(r,c))
					return true; //at least one value is diferent!

		//all values are equal!
		return false;
	}
	//if their dimensions are different... that basically means they are diferent! ;-)
	else 
		return true;
}

//void
//realvec::send(Communicator *com)
//{
//	int i;
//	static char *buf = new char[256];
//	string message;
//	sprintf(buf, "%i\n", (int)size_);
//	message = buf;
//	com->send_message(message);
//	for (i=0; i<size_; i++)
//	{
//		sprintf(buf, "%f\n", data_[i]);
//		message = buf;
//		com->send_message(message);
//	}
////	MRSERR("realvec::send numbers were sent to the client");
//
//}

bool
realvec::read(string filename)
{   
	ifstream from(filename.c_str());
	if(from.is_open())
	{
		from >> (*this);
		return true;
	}
	else
	{
//		MRSERR("realvec::read: failed to open file: " + filename);
		return false;
	}
}

void 
realvec::shuffle() 
{
	int size = cols_;
	int rind;
	double temp;

	for (int c=0;  c < cols_; c++)
	{
		rind = (unsigned int)(((double)rand() / (double)(RAND_MAX))*size);
		for (int r=0; r < rows_; r++) 
		{
			temp = data_[c * rows_ + r];
			data_[c * rows_ + r] = data_[rind * rows_ + r];
			data_[rind * rows_ + r] = temp;
		}
	}  
}

bool 
realvec::write(string filename) const
{
	ofstream os(filename.c_str());
	if(os.is_open())
	{
		os << (*this) << endl;
		return true;
	}
	else
	{
//		MRSERR("realvec::write: failed to open file to write: filename");
		return false;
	}
}

void 
realvec::dump()
{
	for(int i =0 ; i< size_ ; i++)
		cout << data_[i] << " " ;
	cout << endl;
}

bool
realvec::readText(string filename)
{
	ifstream infile(filename.c_str());
	if(infile.is_open())
	{
		int i = 0;
		if (size_ == 0)
			allocate(1);
		double value;
		while (infile >> value) 
		{
			// slower but safer
			stretchWrite(i,value);
			i++;
		}
		stretch(i-1);
		infile.close();
		return true;
	}
	else
	{
//		MRSERR("realvec::readText: failed to open file: " + filename);
		return false;
	}
}

bool
realvec::writeText(string filename)
{
	if (size_ == 0)
		return true; //[?]

	ofstream outfile(filename.c_str());
	if(outfile.is_open())
	{
		for (int i=0; i<size_; i++) {
			outfile << data_[i] <<endl;
		}
		outfile.close();
		return true;
	}
	else
	{
//		MRSERR("realvec::writeText: failed to open file: " + filename);
		return false;
	}
}

ostream& 
operator<< (ostream& o, const realvec& vec)
{
	o << "# MARSYAS mrs_realvec" << endl;
	o << "# Size = " << vec.size_ << endl << endl;
	o << endl;

	o << "# type: matrix" << endl;
	o << "# rows: " << vec.rows_ << endl;
	o << "# columns: " << vec.cols_ << endl;

	for (int r=0; r < vec.rows_; r++)
	{
		for (int c=0; c < vec.cols_; c++)
		  o << vec.data_[c * vec.rows_ + r] << " " ;
		o << endl;
	}

	// for (int i=0; i<vec.size_; i++)
	// o << vec.data_[i] << endl;
	o << endl;
	o << "# Size = " << vec.size_ << endl;
	o << "# MARSYAS mrs_realvec" << endl;  
	return o;
}

istream& 
operator>>(istream& is, realvec& vec)
{   
	// [WTF] ... is this necessary?  doesn't allocate() delete the data array, and reallocate? (Jen)
	//  if (vec.size_ != 0)
	//    {
	//      MRSERR("realvec::operator>>: realvec already allocated cannot read from istream");
	//      MRSERR("vec.size_ = " + vec.size_);
	//      
	//      return is;
	//    }
	string str0,str1,str2;
	int size;
	int i;

	is >> str0;
	is >> str1;
	is >> str2;


	if ((str0 != "#") || (str1 != "MARSYAS") || (str2 != "mrs_realvec"))
	{
//		MRSERR("realvec::operator>>: Problem1 reading realvec object from istream");
//		MRSERR("-str0 = " << str0 << endl);
//		MRSERR("-str1 = " << str1 << endl);
//		MRSERR("-str2 = " << str2 << endl);
		return is;
	}
	is >> str0;				
	is >> str1;
	is >> str2;


	if ((str0 != "#") || (str1 != "Size") || (str2 != "="))
	{
//		MRSERR("realvec::operator>>: Problem2 reading realvec object from istream");
		return is;
	}
	is >> size;

	vec.allocate(size);  
	for (i=0; i<3; i++)
	{
		is >> str0;
	}
	is >> str0 >> str1 >> vec.rows_;
	is >> str0 >> str1 >> vec.cols_;
	

	for (int r = 0; r < vec.rows_; r++)
		for (int c = 0; c < vec.cols_; c++)
		{
		  is >> vec.data_[c * vec.rows_ + r];
		}

	is >> str0;				
	is >> str1;
	is >> str2;
	if ((str0 != "#") || (str1 != "Size") || (str2 != "="))
	{
//		MRSERR("realvec::operator>>: Problem3 reading realvec object from istream");
		is >> str0;				
		is >> str1;
		is >> str2;
		return is;
	}
	is >> size;
	is >> str0;
	is >> str1;
	is >> str2;

	if ((str0 != "#") || (str1 != "MARSYAS") || (str2 != "mrs_realvec"))
	{
//		MRSERR("realvec::operator>>: Problem4 reading realvec object from istream");
		return is;
	}
	return is;
}

realvec
realvec::operator()(std::string r, std::string c)
{
	unsigned int r_l = r.length();
	unsigned int c_l = c.length();

	unsigned int r_c = r.find(":");
	unsigned int c_c = c.find(":");

	unsigned int r_a;
	unsigned int r_b;

	unsigned int c_a;
	unsigned int c_b;

	char *endptr;

	assert( (r_c == 0 && r_l == 1) || (r_c == string::npos) || (r_c>0 && r_l-r_c>1) );
	assert( (c_c == 0 && c_l == 1) || (c_c == string::npos) || (c_c>0 && c_l-c_c>1) );

	if( r_c != string::npos && r_l > 1 )
	{
		r_a = (int)strtol( r.substr(0,r_c).c_str() , &endptr , 10  );
		assert( *endptr == '\0' );
		r_b = (int)strtol( r.substr(r_c+1,r_l-r_c-1).c_str() , &endptr , 10  );
		assert( *endptr == '\0' );
	}
	else if( r_c == string::npos )
	{
		r_a = r_b = (int)strtol( r.c_str() , &endptr , 10 );
		assert( *endptr == '\0' );
	}
	else
	{
		r_a = 0;
		r_b = rows_-1;
	}

	assert( r_a >= 0 && r_b < rows_ );

	if( c_c != string::npos && c_l > 1 )
	{
		c_a = (int)strtol( c.substr(0,c_c).c_str() , &endptr , 10  );
		assert( *endptr == '\0' );
		c_b = (int)strtol( c.substr(c_c+1,c_l-c_c-1).c_str() , &endptr , 10 );
		assert( *endptr == '\0' );
	}
	else if( c_c == string::npos )
	{
		c_a = c_b = (int)strtol( c.c_str() , &endptr , 10 );
		assert( *endptr == '\0' );
	}
	else
	{
		c_a = 0;
		c_b = cols_-1;
	}

	assert( c_a >= 0 && c_b < cols_ );

	r_l = r_b - r_a + 1;
	c_l = c_b - c_a + 1;

	realvec matrix;

	matrix.create( r_l , c_l );

	for( c_c = c_a ; c_c <= c_b ; c_c++ )
	{
		for( r_c = r_a ; r_c <= r_b ; r_c++ )
		{
			matrix.data_[(c_c-c_a) * r_l + (r_c-r_a)] = data_[c_c * rows_ + r_c];
		}
	}       

	return matrix;
}

realvec
realvec::operator()(std::string c)
{
	unsigned int c_l = c.length();
	unsigned int c_c = c.find(":");
	unsigned int c_a;
	unsigned int c_b;
	char *endptr;

	assert( (c_c == 0 && c_l == 1) || (c_c == string::npos) || (c_c>0 && c_l-c_c>1) );

	if( c_c != string::npos && c_l > 1 )
	{
		c_a = (int)strtol( c.substr(0,c_c).c_str() , &endptr , 10  );
		assert( *endptr == '\0' );
		c_b = (int)strtol( c.substr(c_c+1,c_l-c_c-1).c_str() , &endptr , 10  );
		assert( *endptr == '\0' );
	}
	else if( c_c == string::npos )
	{
		c_a = c_b = (int)strtol( c.c_str() , &endptr , 10 );
		assert( *endptr == '\0' );
	}
	else
	{
		c_a = 0;
		c_b = (rows_*cols_)-1;
	}

	assert( c_a >= 0 && c_b < rows_*cols_ );
	c_l = c_b - c_a + 1;

	realvec matrix;
	matrix.create( c_l );

	for( c_c = c_a ; c_c <= c_b ; c_c++ )
	{
		matrix.data_[(c_c-c_a)] = data_[c_c];
	}
	return matrix;
}

void
realvec::getRow(const int r, realvec& res) const
{
	if (this != &res)
	{
		if(r >= rows_ )
		{
//			MRSERR("realvec::getRow() - row index greater than realvec number of rows! Returning empty result vector.");
			res.create(0);
			return;
		}
		res.stretch(cols_);
		for (int c=0; c < cols_; c++)
		{
			res(c) = (*this)(r,c);
		}
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::getRow() - inPlace operation not supported - returning empty result vector!");
	}
}

void
realvec::getCol(const int c, realvec& res) const
{
	if (this != &res)
	{
		if(c >= cols_)
		{
//			MRSERR("realvec::getCol() - row index greater than realvec number of rows! Returning empty result vector.");
			res.create(0);
			return;
		}
		res.stretch(rows_,1);
		for (int r=0; r < rows_; r++)
		{
			res(r) = (*this)(r,c);
		}
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::getCol() - inPlace operation not supported - returning empty result vector!");
	}
}

double
realvec::maxval(int* index) const
{
	double max = numeric_limits<double>::max() * -1.0;
	int ind = 0;
	for (int i=0; i < size_; i++)
	{
		if(data_[i] > max)
		{
			max = data_[i];
			ind = i;
		}
	}
	if(index)
		*index = ind;
	return max;
}

double
realvec::minval() const
{
	double min = numeric_limits<double>::max();
	for (int i=0; i < size_; i++)
	{
		if(data_[i] < min)
			min = data_[i];
	}
	return min;
}

void
realvec::meanObs(realvec& res) const
{
	if (this != &res)
	{
		realvec obsrow(cols_); //row vector //[TODO]
		res.stretch(rows_, 1); //column vector

		for (int r=0; r<rows_; r++)
		{
			//obsrow = getRow(r);
			for (int c=0; c<cols_; c++)
			{
				obsrow(c) = (*this)(r,c); //get observation row
			}
			res(r,0) = obsrow.mean();
		}
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::meanObs() - inPlace operation not supported - returning empty result vector!");
	}
}

void
realvec::varObs(realvec& res) const
{
	if (this != &res)
	{
		res.create(rows_, 1); //column vector
		realvec obsrow(cols_); //row vector //[TODO]

		for (int r=0; r<rows_; r++)
		{
			//obsrow = getRow(r);
			for (int c=0; c<cols_; c++)
			{
				obsrow(c) = (*this)(r,c); //get observation row
			}
			res(r,0) = obsrow.var();
		}
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::varObs() - inPlace operation not supported - returning empty result vector!");
	}
}

void
realvec::stdObs(realvec& res) const
{
	if (this != &res)
	{
		realvec obsrow(cols_); //row vector //[TODO]
		res.stretch(rows_, 1); //column vector
		for (int r=0; r<rows_; r++)
		{
			//obsrow = getRow(r);
			for (int c=0; c < cols_; c++)
			{
				obsrow(c) = (*this)(r,c); //get observation row
			}
			res(r,0) = obsrow.std();
		}
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::stdObs() - inPlace operation not supported - returning empty result vector!");
	}
}

void
realvec::normObs()
{
	//normalize (aka standardize) matrix
	//using observations means and standard deviations 
	realvec obsrow(cols_); //[TODO]
	double mean, std;

	for (int r=0; r < rows_; r++)
	{
		for (int c=0; c < cols_; c++)
		{
			obsrow(c) = (*this)(r,c); //get observation row
		}
		//obsrow = getRow(r);
		mean = obsrow.mean();
		std = obsrow.std();
		for (int c=0; c < cols_; c++)
		{
			(*this)(r,c) -= mean;
			(*this)(r,c) /= std;
		}
	}
}

void
realvec::normObsMinMax()
{
	//normalize (aka standardize) matrix
	//using observations min and max values 

	realvec obsrow(cols_);
	double min, max, dif;

	for (int r=0; r < rows_; r++)
	{
		//for (int c=0; c < cols_; c++)
		//{
		//	obsrow(c) = (*this)(r,c); //get observation row
		//}
		getRow(r, obsrow); //[TODO]
		min = obsrow.minval();
		max = obsrow.maxval();
		dif = max-min;
		if(dif ==0)
			dif=1.0;
		for (int c=0; c < cols_; c++)
		{
			(*this)(r,c) -= min;
			(*this)(r,c) /= dif;
		}
	}
}

void
realvec::normSpl(int index)
{
	//normalize (aka standardize) matrix
	//using ???		[?] [TODO]
	double mean;
	double std;

	realvec colVec;//[TODO]
	
	if(!index)
		index=cols_;
	for (int r=0; r < index; r++)
	{
		//for (int c=0; c < cols_; c++)
		//{
		//	obsrow(c) = (*this)(r,c); //get observation row
		//}

		getCol(r, colVec);//[TODO]
		mean = colVec.mean(); 
		std = colVec.std();   
		
		if(std)
			for (int c=0; c < rows_; c++)
			{
				(*this)(c, r) -= mean;
				(*this)(c, r) /= std;
			}
	}
}

void
realvec::normSplMinMax(int index)
{
	//normalize (aka standardize) matrix
	//using ???		[?] [TODO]
	double min;
	double max, dif;
	realvec colVec; //[TODO]
	
	if(!index)
		index=cols_;
	for (int r=0; r < index; r++)
	{
		//for (int c=0; c < cols_; c++)
		//{
		//	obsrow(c) = (*this)(r,c); //get observation row
		//}
		getCol(r, colVec);
		min = colVec.minval(); //[TODO]
		max = colVec.maxval(); //[TODO]
		dif = max-min;
		if(dif ==0)
			dif=1.0;
		if(max)
			for (int c=0; c < rows_; c++)
			{
				(*this)(c, r) -= min;
				(*this)(c, r) /= dif;
			}
	}
}

void
realvec::correlation(realvec& res) const
{
	//correlation as computed in Marsyas0.1
	//computes the correlation between observations
	//Assumes data points (i.e. examples) in columns and features (i.e. observations) in rows (as usual in Marsyas0.2).
	if(size_ == 0)
	{
//		MRSERR("realvec::correlation() : empty input matrix! returning empty correlation matrix!");
		res.create(0);
		return;
	}
	if (this != &res)
	{
		res.stretch(rows_, rows_);//correlation matrix
		// normalize observations (i.e subtract obs. mean, divide by obs. standard dev)
		realvec temp = (*this); //[TODO]
		temp.normObs(); 

		double sum = 0.0;
		for (int r1=0; r1< rows_; r1++)
		{
			for (int r2=0; r2 < rows_; r2++)
			{
				sum = 0.0;

				for (int c=0; c < cols_; c++)
					sum += (temp(r1,c) * temp(r2,c));

				sum /= cols_;
				res(r1,r2) = sum;
			}
		}
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::correlation() - inPlace operation not supported - returning empty result vector!");
	}
}

void 
realvec::covariance(realvec& res) const
{
	//computes the (unbiased estimate) covariance between observations (as in MATLAB cov()).
	//Assumes data points (i.e. examples) in columns and features (i.e. observations) in rows (as usual in Marsyas0.2).
	//This method assumes non-standardized data (typical covariance calculation).
	if(size_ == 0)
	{
//		MRSERR("realvec::covariance(): empty input matrix! returning empty covariance matrix!");
		res.create(0);
		return;
	}

	if (this != &res)
	{
		res.stretch(rows_, rows_); //covariance matrix
		//check if there are sufficient data points for a good covariance estimation...
		if(cols_ < (rows_ + 1))
		{
//			MRSWARN("realvec::covariance() : nr. data points < nr. observations + 1 => covariance matrix is SINGULAR!");
		}
		if( (double)cols_ < ((double)rows_*(double)(rows_-1.0)/2.0))
		{
//			MRSWARN("realvec::covariance() : too few data points => ill-calculation of covariance matrix!");
		}

		realvec meanobs;
		this->meanObs(meanobs);//observation means //[TODO]

		double sum = 0.0;
		for (int r1=0; r1< rows_; r1++)
		{
			for (int r2=0; r2 < rows_; r2++)
			{
				sum = 0.0;
				for (int c=0; c < cols_; c++)
					sum += ((data_[c * rows_ + r1] - meanobs(r1)) * (data_[c * rows_ + r2]- meanobs(r2)));

				if(cols_ > 1)
					sum /= (cols_ - 1);//unbiased covariance estimate
				else
					sum /= cols_; //biased covariance estimate

				res(r1,r2) = sum;
			}
		}
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::covariance() - inPlace operation not supported - returning empty result vector!");
	}
}

void 
realvec::covariance2(realvec& res) const
{
	//covariance matrix as computed in Marsyas0.1
	//computes the covariance between observations.
	//Assumes data points (i.e. examples) in columns and features (i.e. observations) in rows (as usual in Marsyas0.2).
	//This calculation assumes standardized data at its input...
	if(size_ == 0)
	{
//		MRSERR("realvec::covariance() : empty input matrix! returning empty and invalid covariance matrix!");
		res.create(0);
		return;
	}
	if (this != &res)
	{
		res.stretch(rows_, rows_); //covariance matrix
		//check if there are sufficient data points for a good covariance estimation...
		if(cols_ < (rows_ + 1))
		{
//			MRSWARN("realvec::covariance() : nr. data points < nr. observations + 1 => covariance matrix is SINGULAR!");
		}
		if( (double)cols_ < ((double)rows_*(double)(rows_-1.0)/2.0))
		{
//			MRSWARN("realvec::covariance() : too few data points => ill-calculation of covariance matrix!");
		}

		for (int r1=0; r1< rows_; r1++)
		{
			for (int r2=0; r2 < rows_; r2++)
			{
				double sum = 0.0;

				for (int c=0; c < cols_; c++)
					sum += (data_[c * rows_ + r1] * data_[c * rows_ + r2]);

				sum /= cols_;
				res(r1,r2) = sum;
			}
		}
	}
	else
	{
		res.create(0);
//		MRSERR("realvec::covariance2() - inPlace operation not supported - returning empty result vector!");
	}
}

double
realvec::trace() const
{
	if (cols_ != rows_)
	{
//		MRSWARN("realvec::trace() - matrix is not square!");
	}

	double res = 0.0;
	for(int i = 0; i < size_; )
	{
		res += data_[i];
		i += cols_+1;
	}

	return res;
}

double
realvec::det() const
{
//	NumericLib numlib;
//	return numlib.determinant(*this);
}


