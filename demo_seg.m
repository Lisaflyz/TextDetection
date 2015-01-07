function demo(n)
%clear all;
addpath('liblinear-1.94/matlab/');


% file = '../images/detection/icdar2003/SceneTrialTest/ryoungt_05.08.2002/aPICT0034.JPG';
file = strcat('../images/detection/icdar2013/test/img_',num2str(n),'.jpg');
%file = sprintf('/home/matlab/work/WordRecognition/testimage/%d.png',n);

im = double(rgb2gray(imread(file)));

% im = imresize(im,0.5);

load data/model_deteng
[lines, words, chars, CC] = detText(im,model,prms);
BW = cc2bw(CC);

f = figure('Visible','on');
imshow(BW);
hold on;

% for i = 1:size(chars,1)
%     rect = chars(i,1:4);
%     rect(3) = rect(3) - rect(1);
%     rect(4) = rect(4) - rect(2);
%     rectangle('Position',rect,'EdgeColor','g');
% end
% for i = 1:size(lines,1)
%     rect = lines(i,1:4);
%     rectangle('Position',rect,'EdgeColor','b');
% end
