#include <stdlib.h>
#include <math.h>
#include <mex.h>
#include <vector>
#include <utility>
#include <opencv2/core/core.hpp>
#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/highgui/highgui.hpp>
#include "CC.hpp"

using namespace std;

// double sw_thr = 3.0;
//
// bool isok(double label, double sw1, double sw2) {
//     if (label==0) return false;
//     if (sw1<0 || sw2<0) return false;
//     double ratio = double(sw1)/double(sw2);
//     if (ratio>sw_thr || ratio<1/sw_thr) return false;
//
//     return true;
// }


void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs==0) mexErrMsgTxt("No Image");
    if (mxGetClassID(prhs[0])!=6) mexErrMsgTxt("Matrix is not of type double");
    if (nlhs>1)  mexErrMsgTxt("Too many output arguments");

    double *pixels = mxGetPr(prhs[0]);

    int rows = mxGetM(prhs[0]);   // height
    int cols  = mxGetN(prhs[0]);  // width

    cv::Mat img_t(cols, rows, CV_64F, pixels);
    CV_Assert(img_t.refcount==NULL);
    cv::Mat img = img_t.t();
    CC* c = CC::extractCC(img);
    c->calcBoundingBox();





    const char **fnames;
    int nfields = 5;
    fnames = (const char **)mxCalloc(nfields, sizeof(*fnames));
    fnames[0] = "Connectivity";
    fnames[1] = "ImageSize";
    fnames[2] = "NumObjects";
    fnames[3] = "PixelIdxList";
    fnames[4] = "BoundingBoxes";
    plhs[0] = mxCreateStructMatrix(1, 1, nfields, fnames);

    mxArray *fout;

    // Connectivity
    mxSetFieldByNumber(plhs[0], 0, 0, mxCreateDoubleScalar(c->connectivity));

    // ImageSize
    fout = mxCreateDoubleMatrix(1,2,mxREAL);
    double *pdata=(double *)mxGetData(fout);
    pdata[0] = c->image_size.first;
    pdata[1] = c->image_size.second;
    mxSetFieldByNumber(plhs[0], 0, 1, fout);

    // PixelIdxList
    int ndim = 2;
    int dims[2];
    dims[0] = 1;
    dims[1] = c->num_objects;
    fout = mxCreateCellArray(ndim, dims);

    int count = 0;
    for (int i=0; i<c->num_objects; i++){
        if (c->pixel_idx_list[i].size() > 0){
            mxArray *list = mxCreateDoubleMatrix(c->pixel_idx_list[i].size(),1,mxREAL);
            double *pdata2=(double *)mxGetData(list);
            int count2 = 0;
            for (vector<int>::iterator it=c->pixel_idx_list[i].begin(); 
                    it!=c->pixel_idx_list[i].end(); ++it){
                pdata2[count2++] = *it + 1; // 1から始まるindexに
            }
            mxSetCell(fout, count++, list);
        }
    }
    mxSetFieldByNumber(plhs[0], 0, 3, fout);

    // NumObjects
    mxSetFieldByNumber(plhs[0], 0, 2, mxCreateDoubleScalar(c->num_objects));

    // BoundingBox
    fout = mxCreateDoubleMatrix(c->num_objects,4,mxREAL);
    pdata = (double *)mxGetData(fout);
    
    count = 0;
    for (vector<cv::Rect>::iterator it=c->bb_list.begin();
            it!=c->bb_list.end(); ++it){
        pdata[c->num_objects*0 + count] = it->x;
        pdata[c->num_objects*1 + count] = it->y;
        pdata[c->num_objects*2 + count] = it->width;
        pdata[c->num_objects*3 + count] = it->height;
        count++;
    }
    // pdata[0] = c->image_size.first;
    // pdata[1] = c->image_size.second;
    mxSetFieldByNumber(plhs[0], 0, 4, fout);
}




