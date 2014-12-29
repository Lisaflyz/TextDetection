function [ bw ] = otsubin( im ,rev)
%UNTITLED5 Summary of this function goes here
%   Detailed explanation goes here
[H W] = size(im);
% if H > 40
%     im(:,:) = medfilt2(im(:,:), [4 4], 'symmetric');
% end 

im = uint8(im);
level = graythresh(im);
bw = im2bw(im,level);

count = sum(bw(1,:))+sum(bw(H,:))+sum(bw(:,1))+sum(bw(:,W));

if(count > 0.6 * 2*(H+W))
    bw = ~bw;
end;

if(nargin==2 && rev==1)
    bw = ~bw;
end


end

