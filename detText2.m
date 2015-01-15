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
isgood = filterLine(lines,chars,idx,prms);
% isgood = isgood & checkoverlap(lines);
% separate line into word
words = sepLine(lines,chars,isgood,idx,gray,model,prms);
% chars(:,3:4) = chars(:,3:4) - chars(:,1:2) + 1;

det = struct('lines',lines,'words',words,'chars',chars,'CC',CC,'idx',idx);

function isgood = filterLine(lines,chars,idx,prms)
%FILTERLINE Filter out line that don't have char with high score

isgood = zeros(size(lines,1),1);

% for i=1:size(lines,1)
%     tmpidx = find(idx==i);
%     if max(chars(tmpidx,5)) > prms.maxthr
%         isgood(i) = 1;
%     end
% end
for i=1:size(lines,1)
    tmpidx = find(idx==i);
    sortc = sort(chars(tmpidx,5),'descend');
    if numel(sortc)>=1 && sortc(1) > prms.maxthr
        isgood(i) = 1;
    elseif numel(sortc)>=2 && mean(sortc(1:2)) > prms.maxthr2
        isgood(i) = 1;
    elseif numel(sortc)>=3 && mean(sortc(1:3)) > prms.maxthr3
        isgood(i) = 1;
    end
end
isgood = isgood & checkoverlap(lines);

% isgood2 = checkoverlap(lines(find(isgood==1),:));
% count = 1;
% for i=1:numel(isgood)
%     if isgood(i) == 0, continue; end
%     isgood(i)=isgood2(count);
%     count = count + 1;
% end



function [lines chars CC idx] = detLines(gray,isdark,model,prms)
%SEPLINE Find characters and group them into textlines.
%  gray   : grayscale image
%  isdark : character color Bright on Dark (0) or Dark on Bright (1)
%  model  : model of character classifier

[chars CC] = detChars_mser(gray,isdark,model,prms);
lines = []; idx = [];
if ~isempty(chars)
    [idx lines] = textline(chars(:,1:8), prms.sw_ratio, prms.distance_ratio,prms.color_diff);
end



function words = sepLine(lines,chars,isgood,idx,gray,model,prms)
%SEPLINE Separate textline into words.
%  lines  : bounding box of textline
%  isgood : overlap check
%  gray   : grayscale image

words = [];
for i = 1:size(lines,1)
    if isgood(i)~=1 && prms.overlap; continue; end;
    rect = lines(i,1:4);

    if prms.colorbin
        patch = imcrop(im,rect(1:4));
        bw = otsubin_color(patch);
    else
        patch = imcrop(gray,rect(1:4));
        bw = otsubin(patch);
    end
    word = wordsep(rect,chars(idx == i,:),bw,model,prms); % pass linerect, charrect in line
    words = [words;word];
end
