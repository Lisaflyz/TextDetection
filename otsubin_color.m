function [ bw ] = otsubin_color( im ,rev)
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
[H W ch] = size(im);
% if H > 40
%     im(:,:) = medfilt2(im(:,:), [4 4], 'symmetric');
% end 

im = uint8(im);
gray = uint8(rgb2gray(im));
[level(1) em(1)] = graythresh(gray);
[level(2) em(2)] = graythresh(im(:,:,1));
[level(3) em(3)] = graythresh(im(:,:,2));
[level(4) em(4)] = graythresh(im(:,:,3));
[~, ind] = max(em);
thr = level(ind);

if     ind==2, gray = im(:,:,1);
elseif ind==3, gray = im(:,:,2);
elseif ind==4, gray = im(:,:,3);
end

bw = im2bw(gray,thr);

count = sum(bw(1,:))+sum(bw(H,:))+sum(bw(:,1))+sum(bw(:,W));

if(count > 0.6 * 2*(H+W))
    bw = ~bw;
end;

if(nargin==2 && rev==1)
    bw = ~bw;
end


end

