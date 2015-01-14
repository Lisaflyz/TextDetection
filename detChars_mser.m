function [rect_chars CC] = detChars_mser(I, isdark, model, prms)
%function to extract text characters from image

%% Find CC and BB
CC = MSER(uint8(I),isdark);
bbs = regionprops(CC,'BoundingBox'); bbs = cat(1,bbs.BoundingBox);

%% Filter out small characters 
[H W] = size(I);
minv = max([H/100,W/100,8]);
idx = bbs(:,4) >= minv & bbs(:,3) >= 8;
CC = filterCC(CC,idx);
bbs = bbs(idx,:);

color = regionprops(CC,I,'MeanIntensity'); color = cat(1,color.MeanIntensity);
sw    = CCStrokeWidth(CC);

mgn = 2; % margin for enlarge bb
tmp = double([max(1,bbs(:,1)-mgn) max(1,bbs(:,2)-mgn) ...
    min(W,bbs(:,1)+bbs(:,3)-1+mgn) min(H,bbs(:,2)+bbs(:,4)-1+mgn)]);
bbs = [tmp(:,1:2) tmp(:,3)-tmp(:,1)+1 tmp(:,4)-tmp(:,2)+1];
% bbs_old = tmp;

%% Filter out characters by HOG+SVM classifier
rect_chars = double.empty(0,8); idx = [];
for i = 1:size(bbs,1)
    patch = imcrop(I, round(bbs(i,:)));
    [label,prob] = hogClf(patch,model,prms);
    if(label == 1) % 1 is alpha and number, 2 is non-text
        idx = [idx; i];
        chr = [round(bbs(i,1:4)) prob(label) label sw(i) color(i,1)];
        rect_chars = [rect_chars; chr];
    end
end
CC = filterCC(CC,idx);



function sw = CCStrokeWidth(CC)
%CCSTROKEWIDTH  Compute stroke width by CC list
%  sw : list of stroke width
sw = [];
binImage = regionprops(CC, 'Image');
for i=1:numel(binImage)
    bin = binImage(i).Image;
    bin = [zeros(5,size(bin,2)+10);zeros(size(bin,1),5),bin,zeros(size(bin,1),5);zeros(5,size(bin,2)+10)];
    sw(i) = strokeWidth(bin*255, 0);
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
feature = computefeature(patch);
% predict by trained classfier;
[label,acc,prob] = predict(zeros(size(feature,1),1),sparse(feature),model,'-b 1 -q 1');


if size(label,1) > 1
    idx = find(label == 1);
    mprob = mean(prob(:,1));
    % if mprob < 0.3 % non-text
    if mprob < 0.1 % non-text
        label = 2;
        prob = 1;
    else % text
        mprob = mean(prob(:,1));
        label = 1;
        prob = mean(prob(idx,1));
    end
else
    if prob > prms.clfthr
        label = 1;
    end
end
if isnan(prob), prob = 0; end;
