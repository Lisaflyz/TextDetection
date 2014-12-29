#include <math.h>
#include <opencv2/core/core.hpp>
#include "Rotation.hpp"


	
Rotation::Rotation(){
	d_x = 0.0;
	d_y = 0.0;
}

void Rotation::exec(cv::Point p, double dx, double dy, double angle){
	double s = sin(angle), c = cos(angle);
	double dx2 = dx * dx;
	double dy2 = dy * dy;
	double mag = sqrt(dx2 + dy2);
	dx /= mag; 
	dy /= mag;
	
	d_x = c * dx - s * dy;
	d_y = s * dx + c * dy;
}

double Rotation::getDx(){
	return d_x;
}

double Rotation::getDy(){
	return d_y;
}
