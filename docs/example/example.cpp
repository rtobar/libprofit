/* copyright statement, etc */

#include <cmath>
#include "example.h"
#include "profit/model.h"

namespace profit {

ExampleProfile::ExampleProfile(const Model &model, const std::string &name) :
    Profile(model, name),
	 param1(1.),
	 param2(2.),
	 param3(3)
{
	// no-op
}

bool ExampleProfile::parameter(const std::string &name, double value) {

	if( Profile::parameter(name, value) ) {
		return true;
	}

	if( name == "param1" )      { param1 = value; }
	else if( name == "param2" ) { param2 = value; }
	else {
		return false;
	}

	return true;
}

bool ExampleProfile::parameter(const std::string &name, unsigned int value) {
	if( Profile::parameter(name, value) ) {
		return true;
	}

	if( name == "param3" ) { param3 = value; }
	else {
		return false;
	}

	return true;
}

void ExampleProfile::validate() {

	if ( this->param1 < 0 ) {
		throw invalid_parameter("param1 is negative");
	}
	if ( this->param1 < 0 ) {
		throw invalid_parameter("param2 is negative");
	}
	if ( this->param3 < 0 ) {
		throw invalid_parameter("param3 is negative");
	}

	/*
	if ( this->model->width < 20 || this->model->height < 20 ) {
		throw invalid_parameter("can't apply example profile to images less than 20x20");
	}
	*/
}

void ExampleProfile::evaluate(std::vector<double> &image) {

	Model *model = this->model;
	double x, y;
	unsigned int i, j;
	double half_xbin = model->scale_x/2.;
	double half_ybin = model->scale_y/2.;

	x = 0;
	for (i=0; i < model->width; i++) {
		x += half_xbin;

		y = 0;
		for (j=0; j < model->height; j++) {
			y += half_ybin;

			if ( !model->calcmask || model->calcmask[i + j*model->width] ) {
				double val = std::abs( (this->param1 - this->param2) * this->param3 * (x - y) );
				image[i + j*model->width] = val;
			}

			y += half_ybin;
		}
		x += half_xbin;
	}
}

} /* namespace profit */
