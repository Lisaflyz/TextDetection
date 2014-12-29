function output = wordsep(textline, chars, bw)


n = size(chars,1);
if n <= 3
    output = textline;
    return
end;

line = sum(bw);

line(line<0.05*size(bw,1)) = 0;
im2 = ones(size(bw));
im2(:,line==0) = 0;


%find connect componets
CC = bwconncomp(im2);
%find the bounding box;
bbs  = regionprops(CC, 'BoundingBox');
bbs  = cat(1,bbs.BoundingBox);
bbs = round(bbs);
ind = bbs(:,4)>=3  & bbs(:,3)>=3;
bbs = bbs(ind,:);
chars = bbs;
chars(:,3) = chars(:,1)+chars(:,3);
chars(:,4) = chars(:,2)+chars(:,4);
n = size(chars,1);
words = [];
sortrows(chars,1);
lens = zeros(n-1,1);
for j = 2:size(chars,1)
    lens(j-1) = max(1,(chars(j,1))-(chars(j-1,3)));
end
lens = sort(lens);



counts = histc(lens,1:max(lens));
T = find(cumsum(counts) > 0.65 * numel(lens),1,'first');
T2 = 1.5*T+3;
startpos = textline(1);
for j = 2:size(chars,1)
    len = max(1,(chars(j,1))-(chars(j-1,3)));
    if len >= T2
        endpos = chars(j-1,3)+textline(1);
        rect = [startpos, textline(2), endpos,textline(4)];
        words = [words;rect];
        startpos = chars(j,1)+textline(1);
    end
end
rect = [startpos, textline(2), textline(1)+textline(3),textline(4)];
words = [words;rect];
words(:,3) = words(:,3)-words(:,1);
% words(:,4) = words(:,4)-words(:,2);
output = words;

end
