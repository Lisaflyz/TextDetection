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
    
    /**
     * calc wrap rect
     */
    Rect operator|(const Rect &rhs)
    {
        Rect r;
        r.x = std::min(this->x,rhs.x);
        r.y = std::min(this->y,rhs.y);
        r.width = std::max(this->x+this->width,rhs.x+rhs.width) - r.x;
        r.height = std::max(this->y+this->height, rhs.y+rhs.height) - r.y;
        return r;
    }

    /**
     * calc intersection rect
     */
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
    
    int getArea()
    {
        return this->width * this->height;
    }
};

void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    double *input = mxGetPr(prhs[0]);
    int cols  = mxGetM(prhs[0]);
    int rows  = mxGetN(prhs[0]);
    vector<Rect> gtrects;
    for(int i = 0; i < cols; i++){
        int x1 = (int) input[i+0*cols];
        int y1 = (int) input[i+1*cols];
        int x2 = (int) input[i+2*cols];
        int y2 = (int) input[i+3*cols];
        double prob = (double) input[i+4*cols];
        Rect rect = {x1,y1,x2,y2};
        gtrects.push_back(rect);
    }
    double *input2 = mxGetPr(prhs[1]);
    cols = mxGetM(prhs[1]);
    rows = mxGetN(prhs[1]);
    vector<Rect> detrects;
    for(int i = 0; i < cols; i++){
        int x1 = (int) input2[i+0*cols];
        int y1 = (int) input2[i+1*cols];
        int x2 = (int) input2[i+2*cols];
        int y2 = (int) input2[i+3*cols];
        double prob = (double) input2[i+4*cols];
        Rect rect = {x1,y1,x2,y2};
        detrects.push_back(rect);
    }
    
    // calc match scores for recall
    vector<double> match_recall(gtrects.size(),1);
    for(int i = 0; i < gtrects.size(); i++){
        double max_score = 0.0;
        for(int j = 0; j < detrects.size();j++){
            Rect wrap = gtrects[i] | detrects[j];
            Rect isec = gtrects[i] & detrects[j];
            double overlap = (double)isec.getArea()/(double)wrap.getArea();
            if (max_score < overlap){
                max_score = overlap;
            }
        }
        match_recall[i] = max_score;
    }
    
    plhs[0] = mxCreateDoubleMatrix(gtrects.size(),1, mxREAL); /* Create the output matrix */
    double *output = mxGetPr(plhs[0]);
    for(int i = 0; i < gtrects.size(); i++){
        output[i] = match_recall[i];
    }
}
