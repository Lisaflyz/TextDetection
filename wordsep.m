function words = wordsep(tline, chars, bw)
% ライン全体に大津の二値化を掛けて、CC同士の間隔が広いところで区切る

words = [];
% Num of component <= 3 -> word
if size(chars,1) <= 3, words=tline; return; end;

line = sum(bw); % number of foreground pixel for each x

% Pre process : if (rate of fore < 5%) then (fore->back) => im2
line(line<0.05*size(bw,1)) = 0; 
im2 = ones(size(bw));
im2(:,line==0) = 0;

%% FOR DEBUG
% if mean(chars(:,2)) < 50
%     imshow(bw);
% end

% find bbs of connect componets
CC = bwconncomp(im2);
if CC.NumObjects == 0, return; end
bbs  = regionprops(CC, 'BoundingBox');
bbs  = round(cat(1,bbs.BoundingBox));
chars = bbs(bbs(:,4)>=3 & bbs(:,3)>=3, 1:4);
chars(:,3:4) = chars(:,1:2)+chars(:,3:4) - 1;

% calc length of interval of CC
ccnum = size(chars,1);
lens = zeros(ccnum-1,1); % length of interval
sortrows(chars,1);
for j = 2:ccnum
    lens(j-1) = max(1,(chars(j,1))-(chars(j-1,3)));
end
lens = sort(lens);


counts = histc(lens,1:max(lens)); % make histgram 1~max(lens)
T = find(cumsum(counts) > 0.65 * numel(lens),1,'first'); % 最大間隔*65%を初めて超えるときの間隔
T = 1.5*T+3;
startpos = tline(1);
for j = 2:ccnum
    len = max(1,(chars(j,1))-(chars(j-1,3)));
    % j-1までのCCを１つのwordとして区切る
    if len >= T % separate if interval > threshold
        endpos = chars(j-1,3)+tline(1);
        rect = [startpos, tline(2), endpos, tline(4)];
        words = [words;rect];
        startpos = chars(j,1)+tline(1);
    end
end
rect = [startpos, tline(2), tline(1)+tline(3),tline(4)];
words = [words;rect];
words(:,3) = words(:,3)-words(:,1);
% words(:,4) = words(:,4)-words(:,2);
output = words;

end
