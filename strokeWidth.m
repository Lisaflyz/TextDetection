function med = strokeWidth(img,isdark)

% addpath('./swt/');
[~,med] = swt(double(img),isdark);
