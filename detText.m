function [rect_lines,rect_words,rect_chars,CC, idx_all] = detText(im,model,prms)
%   functions to find text locations
%   input: grayscale image
%   output: rectangles of textlines, words, and characters
%   note that: lines and words are in the format of [x, y, width, height],
%              characters are [x,y,x+width,y+height];
%   
gray = double(rgb2gray(im));
[H W] = size(gray);
if ~prms.useswt
    binIm = bt_niblackbin(gray);
end

% find characters
if prms.useswt
    [rect_chars0,bw0,rect_all0,CC0] = detChars_swt(gray,0,model);
elseif prms.usemser
    [rect_chars0,bw0,rect_all0,CC0] = detChars_mser(gray,0,model);
else
    [rect_chars0,bw0,rect_all0,CC0] = detChars(gray,0,model,binIm);
end
% group characters into textlines
rect_lines0 = []; idx0 = [];
if ~isempty(rect_chars0)
    [idx0,rect_lines0] = textline(rect_chars0(:,1:7));
end

% find characters in opposite pattern
if prms.useswt
    [rect_chars1, bw1, rect_all1,CC1] = detChars_swt(gray,1,model);
elseif prms.usemser
    [rect_chars1, bw1, rect_all1,CC1] = detChars_mser(gray,1,model);
else
    [rect_chars1, bw1, rect_all1,CC1] = detChars(gray,1,model,binIm);
end
% group characters into textlines
rect_lines1 = []; idx1 = [];
if ~isempty(rect_chars1)
    [idx1,rect_lines1] = textline(rect_chars1(:,1:7));
end

rect_chars = [rect_chars0;rect_chars1];
rect_lines = [rect_lines0;rect_lines1];
if isempty(find(idx0~=-1, 1)) % char in line does not exist
    idx1(idx1~=-1) = idx1(idx1~=-1);
else % re-assign label of line
    idx1(idx1~=-1) = idx1(idx1~=-1)+max(idx0);
end
idx_all = [idx0;idx1];
CC = CC0;
CC.PixelIdxList = [CC0.PixelIdxList, CC1.PixelIdxList];
% CC.PixelIdxList = CC.PixelIdxList(idx_all~=-1);
% CC.NumObjects = numel(CC.PixelIdxList);



%check overlap of two patterns
% isgood = checkoverlap(rect_lines);
% rect_lines = rect_lines(isgood==1,:);

%separate textlines into words
rect_words = [];
for i = 1:size(rect_lines,1)
    % if isgood(i) ~= 1; continue; end;
    rect = rect_lines(i,1:4);

    if prms.colorbin
        patch = im(rect(2):rect(2)+rect(4)-1,rect(1):rect(1)+rect(3)-1,:);
        bw = otsubin_color(patch);
    else
        patch = gray(rect(2):rect(2)+rect(4)-1,rect(1):rect(1)+rect(3)-1);
        bw = otsubin(patch);
    end
    word = wordsep(rect,rect_chars(idx_all == i,:),bw); % pass linerect, charrect in line
    rect_words = [rect_words;word];
end
rect_chars(:,3:4) = rect_chars(:,3:4) - rect_chars(:,1:2) + 1;

end
