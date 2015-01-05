run liblinear-1.94/matlab/make
mex textline.cpp;
mex checkoverlap.cpp;
mex HOG.cpp;
mex swt2cc.cpp;
run swt/buildSwt
