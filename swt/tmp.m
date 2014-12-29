img = imread('image/test.png');
[avg, var] = swt(double(rgb2gray(img)),1)
