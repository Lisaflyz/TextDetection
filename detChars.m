function [rect_chars, BW, bbs_old] = detChars(I, isdark, model)
%function to extract text characters from image

[H W] = size(I);

binIm = bt_niblackbin(I);

%get the binary image;
BW = binIm == isdark;

% h = fspecial('average', 3);
% bw1 = filter2(h, double(BW));
% 
% BW = (bw1 >0.40);

%find CCs and their bbs
CC = bwconncomp(BW);
bbs = regionprops(CC, 'BoundingBox');
bbs = cat(1,bbs.BoundingBox);

meancolor = regionprops(CC,I,'MeanIntensity');
meancolor = cat(1,meancolor.MeanIntensity);

%filter out too small or too large bb
ind = bbs(:,4) >= 8  & bbs(:,3) >= 8 & bbs(:,3) < W/2;% & bbs(:,4)< H/2 & bbs(:,3) < W/2;
bbs = bbs(ind,:);
meancolor = meancolor(ind,:);

s  = regionprops(CC, 'centroid');
centroids = cat(1, s.Centroid);
centroids = centroids(ind,:);

mgn = 2; % margin for enlarge bb
tmp = double([max(1,bbs(:,1)-mgn) max(1,bbs(:,2)-mgn) ...
    min(W,bbs(:,1)+bbs(:,3)-1+mgn) min(H,bbs(:,2)+bbs(:,4)-1+mgn)]);
bbs = [tmp(:,1:2) tmp(:,3)-tmp(:,1)+1 tmp(:,4)-tmp(:,2)+1];
bbs_old = tmp;

rect_chars = [];
size(bbs,1)
for i = 1:size(bbs,1)
    rect = round(bbs(i,:));
    patch = imcrop(I, rect);
    [hp wp] = size(patch);
    aspect = hp / wp;
    if aspect > 6.5 || 1/aspect > 4 continue;end;

    %compute feature, may contain multipe characters for wide image patch
    feature = computefeature(patch);
    %predict by trained classfier;
    [label,acc,prob] = predict(zeros(size(feature,1),1),sparse(feature),model,'-b 1 -q 1');

    if size(label,1) > 1
        idx = find(label == 1);
        mprob = mean(prob(:,1));
        if mprob < 0.3 % non-text
            label = 2;
            prob = 1;
        else % text
            mprob = mean(prob(:,1));
            label = 1;
            prob = mean(prob(idx,1));
        end
    end
    
    %1 is alpha and number, 2 is non-text
    if(label == 1)
        % recta = [rect(1) rect(2) rect(3) rect(4)];
        rect_chars = [rect_chars; [round(bbs_old(i,1:4)) prob(label) label meancolor(i,1)]];
    end
    
end
end
