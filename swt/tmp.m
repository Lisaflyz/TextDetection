function swtimg = tmp

% img = imread('image/test.png');
% img = imread('image/656.png');
% swtimg = swt(double(rgb2gray(img)),1);
% imshow(swtimg)


img = imread('../../images/detection/icdar2013/test/img_7.jpg');
swtimg = swt(double(rgb2gray(img)),1);
