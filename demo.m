function demo(n)
%clear all;
addpath('liblinear-1.94/matlab/');


k = 1;
% file = '../images/detection/icdar2003/SceneTrialTest/ryoungt_05.08.2002/aPICT0034.JPG';
file = sprintf('../images/detection/icdar2013/test/img_%d.jpg',n);
%file = strcat('/home/naoki/images/ICDAR2013/scene_images/test_set_images/img_',num2str(k),'.jpg');
%file = sprintf('/home/matlab/hinami/WordRecognition/testimage/%d.png',n);

im = double(rgb2gray(imread(file)));

% im = imresize(im,0.5);

load data/model_deteng
[lines, words, chars] = detText(im,model,prms);
lines
words
% [lines, words, chars] = textdetect(im);
% save('data/tmp_det.mat','lines','words','chars');

%%
f = figure('Visible','on');imshow(uint8(im));
hold on;
% for i = 1:size(chars,1)
%     rect = chars(i,1:4);
%     rect(:,3) = rect(:,3) - rect(:,1);
%     rect(:,4) = rect(:,4) - rect(:,2);
%     rectangle('Position',rect,'EdgeColor','y');
% end
% for i = 1:size(words,1)
%     rect = words(i,1:4);     
%     rectangle('Position',rect,'EdgeColor','b');
% end
for i = 1:size(chars,1)
    rect = chars(i,1:4);
    rect(3) = rect(3) - rect(1);
    rect(4) = rect(4) - rect(2);
    rectangle('Position',rect,'EdgeColor','g');
end
for i = 1:size(lines,1)
    rect = lines(i,1:4);
    rectangle('Position',rect,'EdgeColor','r');
end
for i = 1:size(words,1)
   rect = words(i,1:4);
   rectangle('Position',rect,'EdgeColor','b');
end
%  outfile = strcat('/home/matlab/naoki/comics/',num2str(k),'.png');
%  print( f,'-dpng',outfile ); 
%%
%  rectimg = myrectangle(img,lines);
%  imwrite(uint8(rectimg),strcat('/home/matlab/naoki/comics/',num2str(k),'.png'));
