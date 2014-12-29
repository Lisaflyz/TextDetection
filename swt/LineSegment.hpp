#include <opencv2/core/core.hpp>
class LineSegment 
{
public:
	cv::Point s;	// Start point
	cv::Point e;	// End point
	int w;		// width
	
	LineSegment();
	LineSegment(cv::Point st, cv::Point ed);
    bool operator<(const LineSegment& right) const;
};
