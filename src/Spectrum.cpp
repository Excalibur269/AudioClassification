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

#include "Spectrum.h"

using namespace std;

Spectrum::Spectrum() {
	ponObservations_ = 0;

	cutoff_ = 1.0;
	lowcutoff_ = 0.0;
	re_ = 0.0;
	im_ = 0.0;
}

Spectrum::~Spectrum() {
}

Spectrum::Spectrum(const Spectrum& a) {

}


void Spectrum::computeSpectrum(realvec& in, realvec& out) {
	// copy to output to perform inplace fft 
	// notice transposition of matrix 
	// from row to column 
	int inSamples_ = 512;//?
	for (int t = 0; t < inSamples_; t++) {
		out(t, 0) = in(0, t);
	}

	double *tmp = out.getData();
	myfft_.rfft(tmp, inSamples_ / 2, FFT_FORWARD);

	if (cutoff_ != 1.0) {
		for (int t = (int)((cutoff_ * inSamples_) / 2); t < inSamples_ / 2;
				t++) {
			out(2 * t) = 0;
			out(2 * t + 1) = 0;
		}
	}

	if (lowcutoff_ != 0.0) {
		for (int t = 0; t < (int)((lowcutoff_ * inSamples_) / 2); t++) {
			out(2 * t) = 0;
			out(2 * t + 1) = 0;
		}
	}

	//compare with matlab fft   
	//	 MATLAB_PUT(in, "vec");
	//	 MATLAB_EVAL("out=fft(vec);");
	//   MATLAB_GET("vec", out);

	return;
}

