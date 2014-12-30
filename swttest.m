function swtimg = swttest(n,isdark)

% img = imread('image/test.png');
% img = imread('image/656.png');
% swtimg = swt(double(rgb2gray(img)),1);
% imshow(swtimg)


addpath swt
img = imread(sprintf('../images/detection/icdar2013/test/img_%d.jpg',n));
if nargin==1
    isdark = 1;
end
swtimg = swt(double(rgb2gray(img)),isdark);
imshow(swtimg);
