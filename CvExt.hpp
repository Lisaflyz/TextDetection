#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>

class CvExt{
    public:
    static void Niblack(cv::Mat &in, cv::Mat &out, double k, int b);
    static void matInfo(cv::Mat &img);
};
