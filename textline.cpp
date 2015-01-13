#include <stdlib.h>
#include <math.h>
#include <mex.h>
#include <vector>
#include <utility>      // std::pair, std::make_pair
#include <iostream>

using namespace std;

double distance_ratio = 2.9;
double height_ratio = 1.7;
double intersect_ratio = 1.3;
double sw_ratio = 2.0;
double color_diff = 0.2;

struct Rect {
    int x;
    int y;
    int width;
    int height;
    double sw;
    double color;
    
    bool operator==(const Rect& rhs) {
        return this->x == rhs.x && this->y == rhs.y && this->width == rhs.width && this->height == rhs.height;
    }
    
    Rect operator|(const Rect& rhs) {
        Rect r;
        r.x = std::min(this->x, rhs.x);
        r.y = std::min(this->y, rhs.y);
        r.width = std::max(this->x+this->width, rhs.x+rhs.width) - r.x;
        r.height = std::max(this->y+this->height, rhs.y+rhs.height) - r.y;
        return r;
    }
    
};

bool is_pair(Rect r1, Rect r2){
    int dx = r1.x - r2.x + (r1.width - r2.width) / 2;
    int dy = r1.y - r2.y + (r1.height - r2.height) / 2;
    // character spaceing validation
    if (abs(dx) > distance_ratio * max(r1.width, r2.width) || abs(dx) < 0.25 * min(r1.width, r2.width)) return false;
    
    // character height validation
    double ratio = (double) r1.height / r2.height;
    if (ratio > height_ratio || ratio < 1.0 / height_ratio) return false;
    
    int oy = min(r1.y + r1.height, r2.y + r2.height) -  max(r1.y, r2.y);
    if (oy * intersect_ratio < min(r1.height, r2.height)) return false;

    // stroke width validation
    ratio = r1.sw / r2.sw;
    if (ratio > sw_ratio || ratio < 1.0/sw_ratio) return false;
    // color validation
    ratio = r1.color / r2.color;
    if (abs((int)(r1.color-r2.color))/255.0 > color_diff) return false;
    
    return true;
    
}

void make_pairs(vector<Rect> &rects, vector<pair<Rect, Rect> > &pairs){
    for(size_t i = 0; i< rects.size() - 1; i++){
        Rect r1 = rects[i];
        for(size_t j = i+1; j < rects.size(); j++){
            Rect r2 = rects[j];
            if(is_pair(r1, r2)){
                
                pair<Rect, Rect> apair(r1, r2);
                pairs.push_back(apair);
            }
        }
    }
}

bool is_in_textline(pair<Rect, Rect> pair1, pair<Rect, Rect> pair2) {
    Rect r1 = pair1.first;
    Rect r2 = pair1.second;
    int dx1 = r1.x - r2.x + (r1.width - r2.width) / 2;
    int dy1 = r1.y - r2.y + (r1.height - r2.height) / 2;
    
    r1 = pair2.first;
    r2 = pair2.second;
    int dx2 = r1.x - r2.x + (r1.width - r2.width) / 2;
    int dy2 = r1.y - r2.y + (r1.height - r2.height) / 2;
    
    if (pair1.first == pair2.first || pair1.second == pair2.second) {
        
        int tn = dy1 * dx2 - dx1 * dy2;
        int td = dx1 * dx2 + dy1 * dy2;
        // share the same end, opposite direction
        if (tn * 7 < -td * 4 && tn * 7 > td * 4)
            return true;
    } else if (pair1.first == pair2.second || pair1.second == pair2.first) {
        int tn = dy1 * dx2 - dx1 * dy2;
        int td = dx1 * dx2 + dy1 * dy2;
        // share the other end, same direction
        if (tn * 7 < td * 4 && tn * 7 > -td * 4)
            return true;
    }
    return false;
}


typedef struct ccv_ptree_node_t {
    struct ccv_ptree_node_t* parent;
    pair<Rect, Rect> *element;
    int rank;
} ccv_ptree_node_t;


/* the code for grouping array is adopted from OpenCV's cvSeqPartition func, it is essentially a find-union algorithm */
int pair_grouping(vector<pair<Rect, Rect> > &pairs, vector<int> &index) {
    index.resize(pairs.size(), 0);
    int i, j;
    vector<ccv_ptree_node_t> node;
    //ptree_node_t* node = (ptree_node_t*)malloc(pairs.size() * sizeof(ptree_node_t));
    for (i = 0; i < pairs.size(); i++) {
        ccv_ptree_node_t pnode;
        pnode.parent = 0;
        pnode.element = &pairs[i];
        pnode.rank = 0;
        node.push_back(pnode);
    }
    for (i = 0; i < pairs.size(); i++) {
        if (!node[i].element)
            continue;
        ccv_ptree_node_t *root = &node[i];
        while (root->parent)
            root = root->parent;
        for (j = 0; j < pairs.size(); j++) {
            if( i != j && node[j].element && is_in_textline(*node[i].element, *node[j].element)) {
                ccv_ptree_node_t* root2 = &node[j];
                
                while(root2->parent)
                    root2 = root2->parent;
                
                if(root2 != root) {
                    if(root->rank > root2->rank)
                        root2->parent = root;
                    else {
                        root->parent = root2;
                        root2->rank += root->rank == root2->rank;
                        root = root2;
                    }
                    
                    /* compress path from node2 to the root: */
                    ccv_ptree_node_t* node2 = &node[j];// + j;
                    while(node2->parent) {
                        ccv_ptree_node_t* temp = node2;
                        node2 = node2->parent;
                        temp->parent = root;
                    }
                    
                    /* compress path from node to the root: */
                    node2 = &node[i];// + i;
                    while(node2->parent) {
                        ccv_ptree_node_t* temp = node2;
                        node2 = node2->parent;
                        temp->parent = root;
                    }
                }
            }
        }
    }
    
    
    int class_idx = 0;
    for(i = 0; i < pairs.size(); i++) {
        j = -1;
        ccv_ptree_node_t* node1 = &node[i];
        if(node1->element) {
            while(node1->parent)
                node1 = node1->parent;
            if(node1->rank >= 0)
                node1->rank = ~class_idx++;
            j = ~node1->rank;
        }
        index[i] = j;
    }
    return class_idx;
}
void add_rect_to_line(vector<Rect> &rects, Rect rect){
    if(rects.size() == 0) {
        rects.push_back(rect);
        return;
    }
    for(int i = 0; i < rects.size(); i++){
        if(rects[i] == rect) return;
    }
    rects.push_back(rect);
}


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nlhs>3)  mexErrMsgTxt("Too many output arguments");
    if (nrhs==0) mexErrMsgTxt("No Image");
    double *rects = mxGetPr(prhs[0]);
    int cols  = mxGetM(prhs[0]);
    int rows  = mxGetN(prhs[0]);
    if (nrhs>=2) sw_ratio = mxGetScalar(prhs[1]);
    if (nrhs>=3) distance_ratio = mxGetScalar(prhs[2]);
    // std::cout << nlhs << "sw:" << sw_ratio << "dis:" << distance_ratio << std::endl;

    vector<Rect> gtrects;
    vector<double> scores;
    for(int i = 0; i < cols; i++){
        int x1       = (int)    rects[i+0*cols];
        int y1       = (int)    rects[i+1*cols];
        int x2       = (int)    rects[i+2*cols];
        int y2       = (int)    rects[i+3*cols];
        double prob  = (double) rects[i+4*cols];
        int label    = (int)    rects[i+5*cols];
        double sw    = (double) rects[i+6*cols];
        double color = (double) rects[i+7*cols];
        Rect rect = {x1, y1, x2-x1, y2-y1, sw, color};
        gtrects.push_back(rect);
        scores.push_back(prob);
        
    }
    vector<pair<Rect, Rect> > pairs;
    make_pairs(gtrects, pairs);
    vector<int> index;
    int nchains = pair_grouping(pairs, index);


    vector<vector<Rect> > urects;
    urects.resize(nchains);
    for(int k = 0; k < nchains; k++){
        for(int i = 0; i < index.size(); i++){
            if(index[i] == k){
                add_rect_to_line(urects[k], pairs[i].first);
                add_rect_to_line(urects[k], pairs[i].second);
            }
        }
    }
    vector<Rect> lines;
    vector<double> probabilities;
    int num = 1;
    plhs[0] = mxCreateDoubleMatrix(cols, 1, mxREAL); /* Create the output matrix */
    double *ind = mxGetPr(plhs[0]);
    for(int i = 0; i < cols; i++)
        ind[i] = -1;
    for(int i = 0; i < urects.size(); i++){
        if(urects[i].size() <= 1) { // XXX
            continue;
        }
        double prob = 0;
        Rect r = urects[i][0];
        for(int j = 0; j < urects[i].size(); j++){
            r = r | urects[i][j];
            for(int k = 0; k < cols; k++){
                if(urects[i][j] == gtrects[k]){
                    ind[k] = (double) num;
                    prob += scores[k];
                    break;
                }
            }
        }
        num++;
        lines.push_back(r);
        probabilities.push_back(prob);
    }
    
    // Output
    plhs[1] = mxCreateDoubleMatrix(lines.size(), 5, mxREAL); /* Create the output matrix */
    double *output = mxGetPr(plhs[1]);
    cols = lines.size();
    for(int i = 0; i < cols; i++){
        output[i+0*cols] = (double) lines[i].x;
        output[i+1*cols] = (double) lines[i].y;
        output[i+2*cols] = (double) lines[i].width;
        output[i+3*cols] = (double) lines[i].height;
        output[i+4*cols] = (double) probabilities[i];
    }
}
