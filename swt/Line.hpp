#include <opencv2/core/core.hpp>

class Line {
	public:
		Line();
		static void draw(cv::Mat image, cv::Point p0, cv::Point p1, cv::Scalar value);
		static void draw(cv::Mat image, cv::Point p0, double n_dx, double n_dy, cv::Scalar value);
};
