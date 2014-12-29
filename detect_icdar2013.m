%
% This script detects text in ICDAR 2013 scene images.
%

clear all;

for k = 1:233
    imgfile = strcat('../images/detection/icdar2013/test/img_',num2str(k),'.jpg');
    img = double(rgb2gray(imread(imgfile)));
    [lines, words, chars] = textdetect(img);
    if isempty(words)
        res = words;
    else
        res = words(:,1:4);
        res(:,3) = res(:,1) + res(:,3) - 1;
        res(:,4) = res(:,2) + res(:,4) - 1;
    end
    
    outfile = strcat('res/res_img_',num2str(k),'.txt');
    dlmwrite(outfile,res);
    fprintf('image #####:%d\n',k);
end
