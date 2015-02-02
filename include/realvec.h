/*
** Copyright (C) 1998-2004 George Tzanetakis <gtzan@cs.uvic.ca>
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

#ifndef MARSYAS_REALVEC_H
#define MARSYAS_REALVEC_H 

#include <assert.h>
#include <cmath>
#include <cfloat>
#include <string>
#include <iostream>
#include <fstream>
#include "common.h"

//#include "MrsLog.h"
//#include "Communicator.h"


/** 
    \class realvec
	\ingroup NotmarCore
    \brief Vector of double values

    Array (vector in the numerical sense) of double values. Basic
arithmetic operations and statistics are supported. 


\todo Crash-proof certain functions in realvec (like calling median() on
an empty realvec)

\todo document realvec functions.  In detail.  Using all the doxygen
tricks.  For something as basic as this, it's worth it.
*/


class realvec 
{
protected:
	/// total number of values in data_
  int size_;
	/// total memory allocated for data_
  int allocatedSize_;
	/// number of rows in array; for a one-dimensional array, this is 1.
  int rows_;
	/// number of columns in array.
  int cols_;

	/// the actual array
  double *data_;
  
public:
  realvec();
  virtual ~realvec();
  realvec(int size);
  realvec(int rows, int cols);
  realvec(const realvec& a);
  
  realvec& operator=(const realvec& a);

	/** \name Memory allocation */
	//@{
  void allocate(int size);
  void allocate(int rows, int cols);

	///allocate(size) + fill with zeros
  void create(int size);
	///allocate(rows,cols) + fill with zeros
  void create(int rows, int cols);
	///allocate(rows,cols) + fill with val
  void create(double val, int rows, int cols);

	/// allocate(size) + keep old vals.  May also be used to shrink realvec.
  void stretch(int rows, int cols);
	/// allocate(size) + keep old vals.  May also be used to shrink realvec.
  void stretch(int size);

  	/// write to array, stretching the array if necessary
  void stretchWrite(const int pos, const double val);
  	/// write to array, stretching the array if necessary
  void stretchWrite(const int r, const int c, const double val);
	//@}
  
	/** \name Modify values in array */
	//@{
	/// set all entries to val 
  void setval(int start, int end, double val);
	/// set all entries to val 
  void setval(double val);
	/// appends values from the newValues realvec.  Resizes the original
	/// realvec to make space for the new values.
  void appendRealvec(const realvec newValues);
	//@}
  
	/** \name other */
	//@{
	/// apply a given function to all the elements
  void apply(double (*func) (double));
	//@}

	/** \name Getting information */
	//@{
  int getSize() const;
  int getCols() const;
  int getRows() const;
	/// extracts a subset of a realvec.  One-dimensional realvecs only.
  realvec getSubVector(int startPos, int length) const;
	/// dirty for easy integration 
  double *getData() const;
	//@}


  void shuffle();
  
	/** \name Vector Operations */
	//@{
  realvec& operator+=(const realvec& vec);
  realvec& operator-=(const realvec& vec);
  realvec& operator*=(const realvec& vec);
  realvec& operator/=(const realvec& vec);
  realvec& operator*=(const double val);
  realvec& operator/=(const double val);
  realvec& operator+=(const double val);
  realvec& operator-=(const double val);

  friend realvec operator+(const realvec& vec1, const realvec& vec2);
  friend realvec operator-(const realvec& vec1, const realvec& vec2);
  friend realvec operator*(const realvec& vec1, const realvec& vec2);
  friend realvec operator/(const realvec& vec1, const realvec& vec2);
  friend bool operator!=(const realvec& v1, const realvec& v2);
	//@}
   
	/** \name Item access */
	//@{
  double& operator()(const int i);
  double operator()(const int i) const;
  double& operator()(const int r, const int c);
  double operator()(const int r, const int c) const;
	//@}


	/** \name Indexing*/
	//@{
	/// matlab-like
  realvec operator()(std::string r, std::string c);
	/// matlab-like
  realvec operator()(std::string r);
	/// vector indexing
  void getRow(const int r, realvec& res) const;
	/// vector indexing
  void getCol(const int c, realvec& res) const;
	//@}

	/** \name Output functions */
	//@{
  void debug_info();
  void dump();
  bool write(std::string filename) const;
  bool read(std::string filename);
  friend std::ostream& operator<<(std::ostream&, const realvec&);
  friend std::istream& operator>>(std::istream&, realvec&);
	/// input function for line-separated text files
  bool readText(std::string filename);
	/// output function for line-separated text files
  bool writeText(std::string filename);
	//@}

	/** \name Observations statics */
	//@{
  void meanObs(realvec& res) const;
  void stdObs(realvec& res) const;
  void varObs(realvec& res) const;
  void normObs();
  void normSpl(int=0);
  void normObsMinMax();
  void normSplMinMax(int=0);
	//@}

	/** \name Vector/Matrix Algebra and Statistics */
	//@{
  double maxval(int *index=NULL) const;
  double minval() const;
  double mean() const;
  double median() const;
  double sum() const;
  double std() const;
  double var() const;
  void sort();//assumes one dimensional vector only!
  void abs();
  void sqr();
  void sqroot();
  void norm();
  void normMaxMin();
		
		
  void norm(double mean, double std);
  void renorm(double old_mean, double old_std, double new_mean, double new_std);
  int invert(realvec& res);//lmartins: this seems to be both inplace and returning the inverse matrix in "res"... why both?!? [!][?]
  void transpose();
  void covariance(realvec& res) const; //Typical covariance calculation, as performed by MATLAB cov(). 
  void covariance2(realvec& res) const;//Marsyas0.1 covariance calculation (assumes standardized data, and uses biased estimation) 
  void correlation(realvec& res) const; 
  double trace() const;
  double det() const;
	//@}

	/** \name Communications */
	//@{
//  void send(Communicator *com);
  int search(double val);
	//@}
};


inline 
realvec&
realvec::operator/=(const double val)
{
  for (int i=0; i<size_; i++)
    data_[i] /= val;
  return *this;
}


inline 
realvec&
realvec::operator*=(const double val)
{
  for (int i=0; i<size_; i++)
    data_[i] *= val;
  return *this;
}

inline 
realvec&
realvec::operator-=(const double val)
{
  for (int i=0; i<size_; i++)
    data_[i] -= val;
  return *this;
}

inline 
realvec&
realvec::operator+=(const double val)
{
  for (int i=0; i<size_; i++)
    data_[i] += val;
  return *this;
}
  
inline
realvec& 
realvec::operator+=(const realvec& vec)
{
  for (int i=0; i<size_; i++)
    data_[i] += vec.data_[i];
  return *this;
}

inline
realvec& 
realvec::operator-=(const realvec& vec)
{
  for (int i=0; i<size_; i++)
    data_[i] -= vec.data_[i];
  return *this;
}

inline
realvec& 
realvec::operator*=(const realvec& vec)
{
  for (int i=0; i<size_; i++)
    data_[i] *= vec.data_[i];
  return *this;
}

inline
realvec& 
realvec::operator/=(const realvec& vec)
{
  for (int i=0; i<size_; i++)
    data_[i] /= vec.data_[i];
  return *this;
}
 
inline 
double realvec::operator()(const int r, const int c) const
{
  assert(r < rows_);
  assert(c < cols_);

  return data_[c * rows_ + r];
  // return data_[r * cols_ + c];
}

inline 
double& realvec::operator()(const int r, const int c)
{
  assert(r < rows_);
  assert(c < cols_);

  return data_[c * rows_ + r];  
}

inline 
double realvec::operator()(const int i) const
{
  assert(i < size_);
  return data_[i];
}

inline 
double& realvec::operator()(const int i)
{
  
  assert(i < size_);
  
  return data_[i];
}




#endif /* !MARSYAS_REALVEC_H */

