#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <vector>
#include <utility>
#include <iostream>

using namespace std;

class CC 
{
    public:
        int connectivity;
        pair<int,int> image_size;
        int num_objects;
        vector<vector<int> > pixel_idx_list;
        vector<cv::Rect> bb_list;
        
        static CC* extractCC(cv::Mat &img);
        void outputInfo();

        void labelingBW(cv::Mat &img, cv::Mat_<int> &labels);
        bool isNeighbor(int label, double pixel, double mypixel);
        void calcBoundingBox();
};

CC* CC::extractCC(cv::Mat &img)
{
    CC* c = new CC();
    c->connectivity = 8;
    c->image_size = make_pair(img.rows, img.cols);
    cv::Mat_<int> labels(img.size(), CV_32S);
    labels[1][2] = 10;
    c->labelingBW(img, labels);
    return c;
}

void CC::outputInfo()
{
    cout << "Connectivity : " << connectivity << endl;
    cout << "ImageSize    : " << image_size.first << " x " << image_size.second << endl;
    cout << "NumObjects   : " << num_objects << endl;
}

void CC::labelingBW(cv::Mat &img, cv::Mat_<int> &labels)
{
    int rows = img.rows;
    int cols = img.cols;
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            labels[i][j] = 0;
            // label[i + j*rows] = 0;
        }
    }
    vector<vector<int> > pix_list;
    pix_list.push_back(vector<int>(0));
    int L = 0;
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            double p = img.at<double>(i,j);
            if (p == 0) continue; 
            int nei_v1 = 0, nei_v2 = 0;
            int p2;
            if (i!=0 && j!=0){
                p2 = labels[i-1][j-1];
                if(isNeighbor(p2,img.at<double>(i-1,j-1),p)){
                    nei_v1 = p2;
                }
            }
            if (i!=0 && j!=cols-1){
                p2 = labels[i-1][j+1];
                if(isNeighbor(p2,img.at<double>(i-1,j+1),p)){
                    if (nei_v1 == 0){
                        nei_v1 = p2;
                    } else if (nei_v1 != p2){
                        nei_v2 = p2;
                    }
                }
            }
            if (i!=0){
                p2 = labels[i-1][j];
                if(isNeighbor(p2,img.at<double>(i-1,j),p)){
                    if (nei_v1 == 0){
                        nei_v1 = p2;
                    } else if (nei_v1 != p2){
                        nei_v2 = p2;
                    }
                }
            }
            if (j!=0){
                p2 = labels[i][j-1];
                if(isNeighbor(p2,img.at<double>(i,j-1),p)){
                    if (nei_v1 == 0){
                        nei_v1 = p2;
                    } else if (nei_v1 != p2){
                        nei_v2 = p2;
                    }
                }
            }

            if (nei_v1 == 0 && nei_v2 == 0){
                L += 1;
                labels[i][j] = L;
                pix_list.push_back(vector<int>(1,i+rows*j));
            }else if (nei_v1 > 0 && nei_v2 == 0){
                labels[i][j] = nei_v1;
                pix_list[nei_v1].push_back(i+rows*j);
            }else if (nei_v1 > 0 && nei_v2 > 0){
                int nei_big = int(nei_v1 > nei_v2 ? nei_v1 : nei_v2);
                int nei_small = int(nei_v2 > nei_v1 ? nei_v1 : nei_v2);
                labels[i][j] = nei_small;

                pix_list[nei_small].push_back(i+rows*j);
                for (vector<int>::iterator it=pix_list[nei_big].begin(); 
                        it!=pix_list[nei_big].end(); ++it){
                    labels[*it%rows][*it/rows] = nei_small;
                    pix_list[nei_small].push_back(*it);
                }
                pix_list[nei_big].clear();
            }else{
                printf("ERROR!!!\n");
            }
        }
    }
    pixel_idx_list = pix_list;
    pixel_idx_list.clear();
    for (int i=1; i<=L; i++){
        if (pix_list[i].size() > 0){
            pixel_idx_list.push_back(pix_list[i]);
        }
    }
    num_objects = pixel_idx_list.size();

}

bool CC::isNeighbor(int label, double pixel, double mypixel)
{
    return pixel>0 && mypixel>0;
}

void CC::calcBoundingBox()
{
    bb_list.clear();
    for (int i=0; i<num_objects; i++){
        vector<int>::iterator end = pixel_idx_list[i].end();
        int xmin=100000, xmax=0, ymin=100000, ymax=0;
        for (vector<int>::iterator it=pixel_idx_list[i].begin(); it!=end; ++it){
            int y=*it % image_size.first;
            int x=*it / image_size.first;
            if (xmin > x) xmin = x;
            if (xmax < x) xmax = x;
            if (ymin > y) ymin = y;
            if (ymax < y) ymax = y;
        }
        // cout << xmin << " " << xmax << " " << ymin << " " << ymax << endl;
        bb_list.push_back(cv::Rect(xmin+1,ymin+1,xmax-xmin+1,ymax-ymin+1));
    }

}
