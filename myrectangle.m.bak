function [ im ] = myrectangle( im , rects )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
color = [0 0 255];
for i =1:size(rects,1)
    x = rects(i,1);
    y = rects(i,2);
    width = rects(i,3);
    height = rects(i,4);
    for k = 0:3
    im(y:y+height,x-k,1) = color(1);
    im(y:y+height,x-k,2) = color(2);
    im(y:y+height,x-k,3) = color(3);
    
    im(y:y+height,x+width+k,1) = color(1);
    im(y:y+height,x+width+k,2) = color(2);
    im(y:y+height,x+width+k,3) = color(3);
    
    im(y-k,x:x+width,1) = color(1);
    im(y-k,x:x+width,2) = color(2);
    im(y-k,x:x+width,3) = color(3);
    
    im(y+height+k,x:x+width,1) = color(1);
    im(y+height+k,x:x+width,2) = color(2);
    im(y+height+k,x:x+width,3) = color(3);
    end
end

end

