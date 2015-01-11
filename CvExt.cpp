#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <iostream>
#include "CvExt.hpp"

using namespace cv;

void CvExt::Niblack(cv::Mat &in, cv::Mat &out, double k, int b){
    Mat gray;
    cvtColor(in, gray, CV_RGB2GRAY);
    Mat filter = Mat::ones(2*b+1,2*b+1,CV_32F);
    filter = filter.mul(filter, 1.0/(2.0*b+1.0)/(2.0*b+1.0));


    Mat h = Mat::ones(2*b+1,2*b+1,CV_8U);
    CvExt::matInfo(in);
    filter2D(gray, out, in.depth(), filter);
    // Canny(in, out, 50, 100, 3, true);
    CvExt::matInfo(in);

}


void CvExt::matInfo(cv::Mat &m1){
    std::cout << "rows(height):" << m1.rows <<std::endl;
    std::cout << "cols(width):" << m1.cols << std::endl;
    std::cout << "channels:" << m1.channels() << std::endl;
    std::cout << "dims:" << m1.dims << std::endl;
    std::cout << "depth (ID):" << m1.depth() << "(=" << CV_64F << ")" << std::endl;
}

void CvExt::makeMat(double *in, cv::Mat &out)
{


}
