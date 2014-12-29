#include <math.h>
#include <opencv2/core/core.hpp>
#include "LineSegment.hpp"

LineSegment::LineSegment(){
}

LineSegment::LineSegment(cv::Point st, cv::Point ed){
	s = st;
	e = ed;
	int dx = (int)st.x - (int)ed.x;
	int dy = (int)st.y - (int)ed.y;
	
	if(dx == 0 && dy == 0){
		w = 0;
	}else{
		w = (int)sqrt(dx*dx + dy*dy);
	}
}

bool LineSegment::operator<(const LineSegment& right) const{ 
    return w < right.w;
}
