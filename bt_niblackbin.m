function binIm = bt_niblackbin(Im,b,k)
% BT_NIBLACKBIN  image binarization using Niblack method
%   BINIM = BT_NIBLACKBIN(IM) converts input image IM into binary image 
%   BINIM based on a local thresholding estimated using Niblack formula 
%   [1]. Input image IM can be grayscale or RGB image, which converted into 
%   grayscale. Output image BINIM contains pixel with values 1 (lighter
%   objects), 0 (darker objects) and 0.5 (background).
% 
%   BINIM = BT_NIBLACKBIN(IM,B) specifies window radius B for local mean 
%   and standart deviation in Niblack's formula
% 
%   BINIM = BT_NIBLACKBIN(IM,B,K) specifies paremeter k and window radius B
%   for local mean and standart deviation in Niblack's formula
% 
% Reference:
%   [1] W. Niblack. An introduction to digital image processing. 1986.

if (ndims(Im) > 3)
    error('Input image should be grayscale or RGB');
end

if (ndims(Im) == 3)
    grayIm = rgb2gray(Im);
else
    grayIm = Im;
end

if nargin == 1
    k = 0.4; 
    b = 15;
elseif nargin == 2
    k = 0.4;
end

grayIm = double(grayIm);
h = fspecial('average',2*b+1);
mean_val = imfilter(grayIm,h);
std_val = stdfilt(grayIm,ones(2*b+1));

binIm = 0.5 * ones(size(grayIm));
thr = mean_val + k * std_val;
binIm(grayIm > thr) = 1;
thr_ = mean_val - k * std_val;
binIm(grayIm < thr_) = 0;