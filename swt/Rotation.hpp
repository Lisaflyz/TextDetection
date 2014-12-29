#include <opencv2/core/core.hpp>

class Rotation {
    private:
        double d_x;
        double d_y;
	
    public:
        Rotation();
        void exec(cv::Point p, double dx, double dy, double angle);
        double getDx();
        double getDy();
};
