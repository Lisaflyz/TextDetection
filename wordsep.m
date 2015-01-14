function words = wordsep(tline, chars, bw, model, prms)
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
        %% NEW VERSION
        endpos = chars(j-1,3)+tline(1);
        bwword = bw(:,startpos-tline(1)+1:endpos-tline(1)+1);
        [y x] = find(bwword(:,:)==1);
        rect = [startpos, tline(2)+min(y)-1, endpos, max(y)-min(y)+1];
        % rect = [startpos, tline(2), endpos, tline(4)]; %% OLD VERSION
        if finalValidate(bwword,model,prms)
            words = [words;rect];
        end
        startpos = chars(j,1)+tline(1);
    end
end
bwword = bw(:,startpos-tline(1)+1:tline(3)-1);
[y x] = find(bwword(:,:)==1);
rect = [startpos, tline(2)+min(y)-1, tline(1)+tline(3), max(y)-min(y)+1];
% rect = [startpos, tline(2), tline(1)+tline(3),tline(4)]; %% OLD VERSION
if finalValidate(bwword,model,prms)
    words = [words;rect];
end
if numel(words)>0
    words(:,3) = words(:,3)-words(:,1);
end
% words(:,4) = words(:,4)-words(:,2);
output = words;



function flag = finalValidate(bw,model,prms)
CC = bw2cc(double(bw));
bbs = CC.BoundingBoxes;
probs = [];
for i=1:size(bbs,1)
    [~,prob] = hogClf(imcrop(bw,bbs(i,1:4)),model,prms);
    if numel(prob)>0
        probs = [probs; prob];
    end
end
if max(probs) > prms.finalthr
    flag = 1;
else
    flag = 0;
end


function [label,prob] = hogClf(patch,model,prms)
%HOGCLF  HOG+SVM character classifier
%  patch : character image 
%  model : model of character classifier (SVM)

% label = 1; prob = 1; return;
label = 0; prob = 0;

[hp wp] = size(patch);
aspect = hp / wp;
% if aspect > 6.5 || 1/aspect > 4 return;end;
if aspect > 10 || 1/aspect > 10 return;end;

% compute feature, may contain multipe characters for wide image patch
feature = computefeature(double(patch));
% predict by trained classfier;
[label,acc,prob] = predict(zeros(size(feature,1),1),sparse(feature),model,'-b 1 -q 1');

if numel(prob)==0, return; end

if size(label,1) > 1
    idx = find(label == 1);
    if numel(idx)>0
        prob = mean(prob(idx,1));
    else
        prob = mean(prob(:,1));
    end
else
    prob = prob(1);
    if prob > prms.clfthr
        label = 1;
    end
end
if isnan(prob), prob = 0; end;
