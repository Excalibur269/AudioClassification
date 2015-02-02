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

#include "SVMClassifier.h"

using namespace std;

SVMClassifier::SVMClassifier() {
	prev_mode_ = "predict";
	cnt = 0;
	dims = 0;
	svm_param_.svm_type = C_SVC;
	svm_param_.kernel_type = LINEAR;
	svm_param_.degree = 3;
	svm_param_.gamma = 4;        // 1/k
	svm_param_.coef0 = 0;
	svm_param_.nu = 0.5;
	svm_param_.cache_size = 100;
	svm_param_.C = 1.0;
	svm_param_.eps = 1e-3;
	svm_param_.p = 0.1;
	svm_param_.shrinking = 1;
	svm_param_.probability = 0;
	svm_param_.nr_weight = 0;
	svm_param_.weight_label = NULL;
	svm_param_.weight = NULL;
}

SVMClassifier::SVMClassifier(const SVMClassifier& a) {
	prev_mode_ = "predict";
	cnt = 0;
	dims = 0;
//  ctrl_filename_ = getctrl("mrs_string/filename");
//  ctrl_maxminfile_ = getctrl("mrs_string/maxminfile");
//  ctrl_kernelType_ = getctrl("int/kernelType");

}

SVMClassifier::~SVMClassifier() {
}

//void
//SVMClassifier::myUpdate(MarControlPtr sender)
//{
//	(void) sender;
//	MRSDIAG("SVMClassifier.cpp - SVMClassifier:myUpdate");
//	ctrl_onSamples_->setValue(ctrl_inSamples_, NOUPDATE);
//	setctrl("int/onObservations", 2);
//
//	mode_ = getctrl("mrs_string/mode")->to<mrs_string>();
//	filename_=ctrl_filename_->to<mrs_string>();
//	maxminfile_=ctrl_maxminfile_->to<mrs_string>();
//	kernelType = ctrl_kernelType_->to<int>();
//
//
//	// default values

//
//		switch(kernelType)
//		{
//		case 0:
//			svm_param_.kernel_type = LINEAR;break;
//		case 1:
//			svm_param_.kernel_type = RBF; break;
//		case 2:
//			svm_param_.kernel_type = POLY;break;
//		case 3:
//			svm_param_.kernel_type = SIGMOID;break;
//		default:
//			{cout <<"Not supported kernel type!"<<endl;
//			cout<<"default:LINEAR"<<endl;
//			svm_param_.kernel_type = LINEAR;break;
//			}
//		}
//
//}

void SVMClassifier::svmProcess(realvec& in, realvec& out, string modelfile, string maxminfile) {
	filename_ = modelfile;
	maxminfile_ = maxminfile;
	dims = 0;
	if (mode_ == "train") {
		if (prev_mode_ == "predict") {
			dims = in.getCols();
			instances_.Create(dims);
		}

		instances_.Append(in);
//		out(0, 0) = in(inObservations_ - 1, 0);
//		out(1, 0) = in(inObservations_ - 1, 0);
	}

	if ((prev_mode_ == "train") && (mode_ == "predict")) {

		instances_.NormMaxMin(maxminfile_);

		int nInstances = instances_.getRows();
		svm_prob_.l = nInstances;
		svm_prob_.y = new double[svm_prob_.l];
		svm_prob_.x = new svm_node*[nInstances];
		for (int i = 0; i < nInstances; i++)
			svm_prob_.y[i] = instances_.GetClass(i);

		for (int i = 0; i < nInstances; i++) {
			svm_prob_.x[i] = new svm_node[dims];
			for (int j = 0; j < dims; j++) {
				if (j < dims - 1) {
					svm_prob_.x[i][j].index = j + 1;
					svm_prob_.x[i][j].value = instances_.at(i)->at(j);
				} else {
					svm_prob_.x[i][j].index = -1;
					svm_prob_.x[i][j].value = 0.0;
				}
			}

		}

		const char *error_msg;
		error_msg = svm_check_parameter(&svm_prob_, &svm_param_);
		if (error_msg) {
			fprintf(stderr, "Error: %s\n", error_msg);
			exit(1);
		}

		svm_model_ = svm_train(&svm_prob_, &svm_param_);
//		const struct svm_model  *svm_model_const= svm_model_;
		svm_save_model(filename_.c_str(), svm_model_);
		svm_destroy_model(svm_model_);

	}

	if (mode_ == "predict") {
//      int nAttributes = getctrl("int/inObservations")->to<int>();
		int nAttributes = in.getCols() + 1;
		struct svm_node* xv = new svm_node[nAttributes];
		/*  cout <<"we are in svmclassifier;mode= predict"<<endl;*/

		instances_.NormMaxMinRow(in, maxminfile_);

		for (int j = 0; j < nAttributes; j++) {
			if (j < nAttributes - 1) {
				xv[j].index = j + 1;
				xv[j].value = in(j, 0);
			} else {
				xv[j].index = -1;
				xv[j].value = 0.0;
			}
		}

		if ((cnt != 0) && (cnt != 1))
			cnt = 0;
		if (cnt == 0) {
			newmodel = svm_load_model(filename_.c_str());
			cnt = 1;
		}

		//  double prediction = svm_predict(svm_model_, xv);
		double prediction = svm_predict(newmodel, xv);

//		int label = (int) in(nAttributes - 1, 0);
		out(0, 0) = (double) prediction;
//		out(1, 0) = (double) label;

	}

	prev_mode_ = mode_;

}

