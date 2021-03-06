function [rect_chars, BW, bbs_old, CC] = detChars_swt(I, isdark, model)

[H W] = size(I);
minv = max([H/100,W/100,8]);

addpath('swt/');
SWT = swt(I,isdark);
BW = SWT; % いらない
CC = swt2cc(SWT);
% imshow(SWT);

%% 以下detCharsと同じ
bbs = regionprops(CC,'BoundingBox');
bbs = cat(1,bbs.BoundingBox);

%filter out too small or too large bb
idx = bbs(:,4) >= minv  & bbs(:,3) >= 5 & bbs(:,3) < W/2;% & bbs(:,4)< H/2 & bbs(:,3) < W/2;
% idx = bbs(:,4) >= 1;
CC.PixelIdxList = CC.PixelIdxList(idx);
CC.NumObjects = numel(CC.PixelIdxList);
bbs = bbs(idx,:);

meancolor = regionprops(CC,I,'MeanIntensity');
meancolor = cat(1,meancolor.MeanIntensity);
centroids = regionprops(CC,'centroid');
centroids = cat(1,centroids.Centroid);

mgn = 2; % margin for enlarge bb
tmp = double([max(1,bbs(:,1)-mgn) max(1,bbs(:,2)-mgn) ...
    min(W,bbs(:,1)+bbs(:,3)-1+mgn) min(H,bbs(:,2)+bbs(:,4)-1+mgn)]);
bbs = [tmp(:,1:2) tmp(:,3)-tmp(:,1)+1 tmp(:,4)-tmp(:,2)+1];
bbs_old = tmp;

% determine if char or not for each CC
rect_chars = []; idx = [];
for i = 1:size(bbs,1)
    rect = round(bbs(i,:));
    patch = imcrop(I, rect);
    
    [label,prob] = hogClf(patch,model);
    %1 is alpha and number, 2 is non-text
    if(label == 1)
        idx = [idx; i];
        rect_chars = [rect_chars; [round(bbs_old(i,1:4)) prob(label) label meancolor(i,1)]];
    end
end
CC.PixelIdxList = CC.PixelIdxList(idx);
CC.NumObjects = numel(CC.PixelIdxList);
end


function [label,prob] = hogClf(patch,model)
label = 1; prob = 1; return;
    label = 0; prob = 0;

    [hp wp] = size(patch);
    aspect = hp / wp;
    if aspect > 6.5 || 1/aspect > 4 return;end;

    % compute feature, may contain multipe characters for wide image patch
    feature = computefeature(patch);
    % predict by trained classfier;
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
end
