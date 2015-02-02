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

#ifndef MARSYAS_SPECTRUM_H
#define MARSYAS_SPECTRUM_H

#include "fft.h"

/** 
    \class Spectrum
		\ingroup Analysis
    \brief Compute the complex spectrum of input window 

    Computes the complex spectrum (N/2+1 points) of the input window 
    using the Fast Fourier Transform (FFT). The output is 
		a N-sized column vector (where N is the size of the input
		audio vector), using the following format:

		[Re(0), Re(N/2), Re(1), Im(1), Re(2), Im(2), ..., Re(N/2-1), Im(N/2-1)]

		Note that the DC and Nyquist frequencies only have real part,
		and are output as the two first coefficients in the vector. 
		Overall, the output spectrum has N/2+1 unique points, corresponding
		to the positive half of the complex spectrum. 
*/

class Spectrum
{
private:
  fft myfft_;
  
  double cutoff_;
  double lowcutoff_;
  double re_,im_;
  int ponObservations_;

  void addControls();

 
public:
  Spectrum();
  Spectrum(const Spectrum&);
  ~Spectrum();

  void computeSpectrum(realvec& in, realvec& out);
};

}//namespace Marsyas

#endif

	
