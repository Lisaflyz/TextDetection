function swtimg = tmp

% img = imread('image/test.png');
img = imread('image/656.png');
swtimg = swt(double(rgb2gray(img)),1);
imshow(swtimg)
