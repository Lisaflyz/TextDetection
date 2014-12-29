#include <stdlib.h>
#include <math.h>
#include <mex.h>
#include <vector>
#include <utility>      // std::pair, std::make_pair

using namespace std;
struct Rect {
    int x;
    int y;
    int width;
    int height;
    
    bool operator==(const Rect& rhs)
    {
        return this->x == rhs.x && this->y == rhs.y && this->width == rhs.width && this->height == rhs.height;
    }
    
    Rect operator|(const Rect &rhs)
    {
        Rect r;
        r.x = std::min(this->x,rhs.x);
        r.y = std::min(this->y,rhs.y);
        r.width = std::max(this->x+this->width,rhs.x+rhs.width) - r.x;
        r.height = std::max(this->y+this->height, rhs.y+rhs.height) - r.y;
        return r;
    }
    Rect operator&(const Rect &rhs)
    {
        Rect r;
        r.x = std::max(this->x,rhs.x);
        r.y = std::max(this->y,rhs.y);
        r.width = std::min(this->x+this->width,rhs.x+rhs.width) - r.x;
        r.height = std::min(this->y+this->height, rhs.y+rhs.height) - r.y;
        
        if(r.width <= 0 || r.height <= 0){
            r.x = 0;r.y = 0; r.width = 0;r.height = 0;
        }
        return r;
    }
    
};

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    double *input = mxGetPr(prhs[0]);
    int cols  = mxGetM(prhs[0]);
    int rows  = mxGetN(prhs[0]);
    vector<Rect> rects;
    vector<double> scores;
    for(int i = 0; i < cols; i++){
        int x1 = (int) input[i+0*cols];
        int y1 = (int) input[i+1*cols];
        int x2 = (int) input[i+2*cols];
        int y2 = (int) input[i+3*cols];
        double prob = (double) input[i+4*cols];
        Rect rect = {x1,y1,x2,y2};
        rects.push_back(rect);
        scores.push_back(prob);
        
    }
    
    vector<int> isgood(scores.size(),1);
    for(int i = 0; i < rects.size(); i++){
        for(int j = i+1; j < rects.size();j++){
            Rect r = rects[i] & rects[j];

            double overlap = (double) (r.width * r.height) / std::min(rects[i].width*rects[i].height,rects[j].width*rects[j].height);
            if(overlap > 0.5){
                int k = scores[i] > scores[j] ? j : i;
                isgood[k] = 0;
            }
        }
    }
    
    plhs[0] = mxCreateDoubleMatrix(rects.size(),1, mxREAL); /* Create the output matrix */
    double *output = mxGetPr(plhs[0]);
    cols = rects.size();
    for(int i = 0; i < rects.size(); i++){
                
        output[i+0*cols] = (double) isgood[i];
       
    }
    
}