function [ rect_lines, rect_words, rect_chars_all ] = textdetect( im )
%   functions to find text locations
%   input: grayscale image
%   output: rectangles of textlines, words, and characters
%   note that: lines and words are in the format of [x, y, width, height],
%              characters are [x,y,x+width,y+height];
%   
[H W] = size(im);

% find characters
[rect_chars0,bw0,rect_all0] = character(im,0);
rect_lines0 = [];
idx0 = [];
if ~isempty(rect_chars0)
    [idx0,rect_lines0] = textline(rect_chars0(:,1:7));
end

% find characters in opposite pattern
[rect_chars1, bw1, rect_all1] = character(im,1);


% group characters into textlines
rect_lines1 = [];
idx1 = [];
if ~isempty(rect_chars1)
    [idx1,rect_lines1] = textline(rect_chars1(:,1:7));
end

rect_chars_all = [rect_chars0;rect_chars1];
rect_lines_all = [rect_lines0;rect_lines1];
if isempty(find(idx0~=-1, 1))
    idx1(idx1~=-1)= idx1(idx1~=-1);
else
    idx1(idx1~=-1)= idx1(idx1~=-1)+max(idx0);
end
idx_all = [idx0;idx1];

%check overlap of two patterns
isgood = checkoverlap(rect_lines_all);
rect_lines = rect_lines_all(isgood==1,:);

%separate textlines into words
rect_words = [];
for i = 1:size(rect_lines_all,1)
    if isgood(i) ~= 1; continue; end;
    rect = rect_lines_all(i,1:4);
    if i < max(idx0)
        patch = im(rect(2):rect(2)+rect(4),rect(1):rect(1)+rect(3));
        bw = otsubin(patch);
        word = wordsep(rect,rect_chars_all(idx_all == i,:),bw);
        rect_words = [rect_words;word];
    else
        patch = im(rect(2):rect(2)+rect(4),rect(1):rect(1)+rect(3));
        bw = otsubin(patch);
        word = wordsep(rect,rect_chars_all(idx_all == i,:),bw);
        rect_words = [rect_words;word];
    end
end

end
