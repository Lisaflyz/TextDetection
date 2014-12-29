%
% This script detects text in ICDAR 2013 scene images.
%

clear all;

model = trainDetClf(1);

for k = 1:233
    fname = strcat('../images/detection/icdar2013/test/img_',num2str(k),'.jpg');
    I = double(rgb2gray(imread(fname)));
    [~, words, ~] = detText(I,model);
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

evaldet
