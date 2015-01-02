function [ im ] = myrectangle( im , rects )
%UNTITLED2 Summary of this function goes here
%   Detailed explanation goes here
[H W ch] = size(im);
color = [0 255 0];
rects = int16(rects);
for i =1:size(rects,1)
    x = rects(i,1);
    y = rects(i,2);
    width = rects(i,3);
    height = rects(i,4);
    for k = 0:1
        if x-k > 0
            im(y:y+height,x-k,1) = color(1);
            im(y:y+height,x-k,2) = color(2);
            im(y:y+height,x-k,3) = color(3);
        end
        if 1
            im(y:y+height,x+width+k,1) = color(1);
            im(y:y+height,x+width+k,2) = color(2);
            im(y:y+height,x+width+k,3) = color(3);
        end
        if y-k > 0
            im(y-k,x:x+width,1) = color(1);
            im(y-k,x:x+width,2) = color(2);
            im(y-k,x:x+width,3) = color(3);
        end
        if 1
            im(y+height+k,x:x+width,1) = color(1);
            im(y+height+k,x:x+width,2) = color(2);
            im(y+height+k,x:x+width,3) = color(3);
        end
    end
end

end


