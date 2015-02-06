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

#include "Rms.h"

using namespace std;

Rms::Rms() {
	//type_ = "Rms";
	//name_ = name;  
}

Rms::~Rms() {
}

double Rms::computeRms(realvec& in) {
	double val;
	double rmsEnergy = 0.0;
	double rms = 0;

	int inSamples_ = in.getRows();
	for (int t = 0; t < inSamples_; t++) {
		val = in(t, 0);
		rmsEnergy += (val * val);
	}
	if (rmsEnergy != 0.0) {
		rmsEnergy /= inSamples_;
		rmsEnergy = sqrt(rmsEnergy);
	}

	rms = rmsEnergy;
	return rms;

}

