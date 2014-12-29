function [rect_chars, BW, boundingbox] = character(im, isdark)

%function to extract text characters from image

%load the pretrained SVM model

load model_0528.mat


[H W] = size(im);

binIm = bt_niblackbin(im);

%get the binary image;
BW = binIm == isdark;

% h = fspecial('average', 3);
% bw1 = filter2(h, double(BW));
% 
% BW = (bw1 >0.40);

%find connect componets
CC = bwconncomp(BW);
%find the bounding box;
bbs  = regionprops(CC, 'BoundingBox');
bbs  = cat(1,bbs.BoundingBox);

meancolor = regionprops(CC,im,'MeanIntensity');
meancolor = cat(1,meancolor.MeanIntensity);
%filter out too small or too large bb
ind = bbs(:,4) >= 8  & bbs(:,3) >= 8 & bbs(:,3) < W/2;% & bbs(:,4)< H/2 & bbs(:,3) < W/2;
bbsfiltered = bbs(ind,:);
meancolor = meancolor(ind,:);


s  = regionprops(CC, 'centroid');
centroids = cat(1, s.Centroid);
centroids = centroids(ind,:);


x1 = bbsfiltered(:,1);
x2 = bbsfiltered(:,1) + bbsfiltered(:,3);
y1 = bbsfiltered(:,2);
y2 = bbsfiltered(:,2) + bbsfiltered(:,4);

% enlarge the bb a bit
x1 = max(1,x1-2);
x2 = min(W,x2+2);
y1 = max(1,y1-2);
y2 = min(H,y2+2);
rect_chars = [];

boundingbox =double([x1 y1 x2 y2]);
for i = 1:size(boundingbox,1)
    rect = round(boundingbox(i,:));
    patch = im(rect(2):rect(4),rect(1):rect(3));
    [hp wp] = size(patch);
    aspect = hp / wp;
    if aspect > 6.5 || 1/aspect > 4 continue;end;

    %compute feature, may contain multipe characters for wide image patch
    feature = computefeature(patch);

    %predict by trained classfier;
    [label, accuracy, prob] = predict(zeros(size(feature,1),1),sparse(feature),model,'-b 1 -q 1');

    if size(label,1) > 1
        idx = find(label == 1);
        mprob = mean(prob(:,1));
        if mprob < 0.3
            label = 2;
            prob = 1;
        else
            mprob = mean(prob(:,1));
            label = 1;
            prob = mean(prob(idx,1));
        end
    end
    
    
    %1 is alpha and number, 2 is non-text
    if(label < 2)
        recta = [rect(1) rect(2) rect(3) rect(4)];
        rect_chars = [rect_chars; [recta prob(label) label meancolor(i,1)]];
        
    end
    
end
end
