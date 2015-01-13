function det = detText(im,model,prms)
%   functions to find text locations
%   input: grayscale image
%   output: rectangles of textlines, words, and characters
%   note that: lines and words are in the format of [x, y, width, height],
%              characters are [x,y,x+width,y+height];
%   
addpath ('./swt/');
gray = double(rgb2gray(im));
[H W] = size(gray);

% detect characters and group them into line
[lines0 chars0 CC0 idx0] = detLines(gray,0,model,prms);
[lines1 chars1 CC1 idx1] = detLines(gray,1,model,prms);

% cat both color
chars = [chars0;chars1];
lines = [lines0;lines1];
CC.PixelIdxList = [CC0.PixelIdxList, CC1.PixelIdxList];
CC.NumObjects   = numel(CC.PixelIdxList);
if ~isempty(find(idx0~=-1, 1)) % re-assign label of line
    idx1(idx1~=-1) = idx1(idx1~=-1)+max(idx0);
end
idx = [idx0;idx1];

% check overlap of two patterns
isgood = checkoverlap(lines);
% separate line into word
words = sepLine(lines,chars,isgood,idx,gray,prms);
chars(:,3:4) = chars(:,3:4) - chars(:,1:2) + 1;

det = struct('lines',lines,'words',words,'chars',chars,'CC',CC,'idx',idx);



function [lines chars CC idx] = detLines(gray,isdark,model,prms)
%SEPLINE Find characters and group them into textlines.
%  gray   : grayscale image
%  isdark : character color Bright on Dark (0) or Dark on Bright (1)
%  model  : model of character classifier

[chars CC] = detChars_mser(gray,isdark,model);
lines = []; idx = [];
if ~isempty(chars)
    [idx lines] = textline(chars(:,1:8), prms.sw_ratio, prms.distance_ratio);
end



function words = sepLine(lines,chars,isgood,idx,gray,prms)
%SEPLINE Separate textline into words.
%  lines  : bounding box of textline
%  isgood : overlap check
%  gray   : grayscale image

words = [];
for i = 1:size(lines,1)
    if isgood(i)~=1 && prms.overlap; continue; end;
    rect = lines(i,1:4);

    if prms.colorbin
        patch = im(rect(2):rect(2)+rect(4)-1,rect(1):rect(1)+rect(3)-1,:);
        bw = otsubin_color(patch);
    else
        patch = gray(rect(2):rect(2)+rect(4)-1,rect(1):rect(1)+rect(3)-1);
        bw = otsubin(patch);
    end
    word = wordsep(rect,chars(idx == i,:),bw); % pass linerect, charrect in line
    words = [words;word];
end
