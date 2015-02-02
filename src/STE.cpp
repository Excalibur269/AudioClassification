/*
 ** Author:Li Zhonghua
 ** Date:4/05/2008
 */
#include "STE.h"

using namespace std;

ShortTimeEnergy::ShortTimeEnergy() {
	//threshold_ = 0.03;

}

ShortTimeEnergy::~ShortTimeEnergy() {

}

void ShortTimeEnergy::computeSTE(realvec& in, realvec& out) {

	double cnt = 0.0;
	double val = 0.0;
	double ste = 0.0;
	int inObservations_ = in.getRows();
	int inSamples_ = in.getCols();

	// compute the energy
	for (int o = 0; o < inObservations_; o++) {
		for (int t = 0; t < inSamples_; t++) {
			val = in(o, t);
			ste = ste + val * val;
		}
		if (ste != 0) {
			ste = ste / inSamples_;
			ste = sqrt(ste);
		}

		out(o, 0) = ste;

//		ste = 0.0;

	}
}
