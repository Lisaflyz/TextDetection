run liblinear-1.94/matlab/make
mex textline.cpp;
mex checkoverlap.cpp;
mex HOG.cpp;
mex swt2cc.cpp;
run swt/buildSwt

mex bw2cc.cpp -lopencv_core -lopencv_imgproc -lopencv_highgui
