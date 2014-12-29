run liblinear-1.94/matlab/make
mex textline.cpp;
mex checkoverlap.cpp;
mex HOG.cpp;
run swt/buildSwt
