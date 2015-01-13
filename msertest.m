function CC = msertest(n,isdark)
  

if numel(n)>1
    I = n;
    fname = n;
else
    fname = sprintf('../images/detection/icdar2013/test/img_%d.jpg',n);
    I = imread(fname);
end
[H W ch] = size(I);
% for i=1:H, for j=1:W
%     I(i,j,:) = I(i,j,:) - min(I(i,j,:));
% end,end
% I = adjustImage(I,0.2);
I = uint8(rgb2gray(I));

CC = MSER(I,isdark);
CC
bb = regionprops(CC,'BoundingBox');

drawBBs2(fname, cat(1,bb.BoundingBox));
