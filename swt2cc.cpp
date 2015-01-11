#include <stdlib.h>
#include <math.h>
#include <mex.h>
#include <vector>
#include <utility>

using namespace std;

double sw_thr = 3.0;

bool isok(double label, double sw1, double sw2) {
    if (label==0) return false;
    if (sw1<0 || sw2<0) return false;
    double ratio = double(sw1)/double(sw2);
    if (ratio>sw_thr || ratio<1/sw_thr) return false;

    return true;
}


/**
 * @arg swt_image
 * @return label_image
 */
void mexFunction(int nlhs, mxArray *plhs[], int nrhs, const mxArray *prhs[]) {
    if (nrhs==0) mexErrMsgTxt("No Image");
    if (mxGetClassID(prhs[0])!=6) mexErrMsgTxt("Matrix is not of type double");
    if (nlhs>1)  mexErrMsgTxt("Too many output arguments");

    double *pixels = mxGetPr(prhs[0]); // input image

    int rows = mxGetM(prhs[0]);   // height
    int cols  = mxGetN(prhs[0]);  // width

    vector<vector<int> > pixlist; // pixel index list
    pixlist.push_back(vector<int>(0));


    mxArray *ptmp = mxCreateDoubleMatrix(rows,cols,mxREAL);
    double *label=(double *)mxGetData(ptmp);
    // double* label = mxGetPr(plhs[0]);
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            // label[j*rows+i] = 0;
            label[i + j*rows] = 0;
        }
    }
    int L = 0;
    for (int i = 0; i < rows; i++){
        for (int j = 0; j < cols; j++){
            double p = pixels[i+j*rows];
            if (p < 0){
                continue;
            }
            double nei_v1 = 0, nei_v2 = 0;
            double p2;
            if (i!=0 && j!=0){
                p2 = label[(i-1)+rows*(j-1)];
                if(isok(p2,pixels[(i-1)+rows*(j-1)],p)){
                    nei_v1 = p2;
                }
            }
            if (i!=0 && j!=cols-1){
                p2 = label[(i-1)+rows*(j+1)];
                if(isok(p2,pixels[(i-1)+rows*(j+1)],p)){
                    if (int(nei_v1) == 0){
                        nei_v1 = p2;
                    } else if (nei_v1 != p2){
                        nei_v2 = p2;
                    }
                }
            }
            if (i!=0){
                p2 = label[(i-1)+rows*(j)];
                if(isok(p2,pixels[(i-1)+rows*(j)],p)){
                    if (int(nei_v1) == 0){
                        nei_v1 = p2;
                    } else if (nei_v1 != p2){
                        nei_v2 = p2;
                    }
                }
            }
            if (j!=0){
                p2 = label[(i)+rows*(j-1)];
                if(isok(p2,pixels[(i)+rows*(j-1)],p)){
                    if (int(nei_v1) == 0){
                        nei_v1 = p2;
                    } else if (nei_v1 != p2){
                        nei_v2 = p2;
                    }
                }
            }

            if (nei_v1 == 0 && nei_v2 == 0){
                L += 1;
                label[i+rows*j] = L;
                pixlist.push_back(vector<int>(1,i+rows*j));
            }else if (nei_v1 > 0 && nei_v2 == 0){
                label[i+rows*j] = nei_v1;
                pixlist[int(nei_v1)].push_back(i+rows*j);
            }else if (nei_v1 > 0 && nei_v2 > 0){
                int nei_big = int(nei_v1 > nei_v2 ? nei_v1 : nei_v2);
                int nei_small = int(nei_v2 > nei_v1 ? nei_v1 : nei_v2);
                label[i+rows*j] = nei_small;

                pixlist[nei_small].push_back(i+rows*j);
                for (vector<int>::iterator it=pixlist[nei_big].begin(); 
                        it!=pixlist[nei_big].end(); ++it){
                    label[*it] = nei_small;
                    pixlist[nei_small].push_back(*it);
                }
                pixlist[nei_big].clear();

                // bool flag = false;
                // for (int i2 = 0; i2 < rows; i2++){
                //     for (int j2 = 0; j2 < cols; j2++){
                //         if (i2 >= i && j2 >= j) {
                //             flag = true;
                //             break;
                //         }
                //         if (int(label[i2+rows*j2])==nei_big){
                //             // printf("%d,%d %f \n",i2,j2,nei_big);
                //             label[i2+rows*j2]=double(nei_small);
                //         }
                //     }
                //     if (flag == true) break;
                // }
            }else{
                printf("ERROR!!!\n");
            }
        }
    }

    const char **fnames;
    int nfields = 4;
    fnames = (const char **)mxCalloc(nfields, sizeof(*fnames));
    fnames[0] = "Connectivity";
    fnames[1] = "ImageSize";
    fnames[2] = "NumObjects";
    fnames[3] = "PixelIdxList";
    plhs[0] = mxCreateStructMatrix(1, 1, nfields, fnames);

    mxArray *fout;

    // Connectivity
    mxSetFieldByNumber(plhs[0], 0, 0, mxCreateDoubleScalar(8));

    // ImageSize
    fout = mxCreateDoubleMatrix(1,2,mxREAL);
    double *pdata=(double *)mxGetData(fout);
    pdata[0] = rows;
    pdata[1] = cols;
    mxSetFieldByNumber(plhs[0], 0, 1, fout);

    // PixelIdxList
    int n = 0;
    for (int i=1; i<=L; i++){
        if (pixlist[i].size() > 0){
            n++;
        }
    }
    int ndim = 2;
    int dims[2];
    dims[0] = 1;
    dims[1] = n;
    fout = mxCreateCellArray(ndim, dims);

    int count = 0;
    for (int i=1; i<=L; i++){
        if (pixlist[i].size() > 0){
            mxArray *list = mxCreateDoubleMatrix(pixlist[i].size(),1,mxREAL);
            double *pdata2=(double *)mxGetData(list);
            int count2 = 0;
            for (vector<int>::iterator it=pixlist[i].begin(); 
                    it!=pixlist[i].end(); ++it){
                pdata2[count2++] = *it + 1; // 1から始まるindexに
            }
            mxSetCell(fout, count++, list);
        }
    }
    mxSetFieldByNumber(plhs[0], 0, 3, fout);

    // NumObjects
    mxSetFieldByNumber(plhs[0], 0, 2, mxCreateDoubleScalar(n));
}




