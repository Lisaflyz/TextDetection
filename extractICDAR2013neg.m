function [features_neg, trainingY] = extractICDAR2013neg()

features_neg = [];
for k = 100:328
    file = strcat('../images/detection/icdar2013/train_gt_seg/',num2str(k),'_GT.txt');
    fileID = fopen(file);
    imgfile = strcat('../images/detection/icdar2013/train/',num2str(k),'.jpg');
    img = double(rgb2gray(imread(imgfile)));
    C = textscan(fileID,'%d %d %d %d %d %d %d %d %d %s','commentStyle','#');
    [H W] = size(img);
    x1 = C{6};
    y1 = C{7};
    x2 = C{8};
    y2 = C{9};
    
    x1 = max(0,x1-1);
    x2 = min(W-1,x2+1);
    y1 = max(0,y1-1);
    y2 = min(H-1,y2+1);
    
    gt_rects =double([x1 y1 x2 y2])+1;
    
    
    [H W] = size(img);
    
    binIm = bt_niblackbin(img);
    
    %get the binary image;
    BW = binIm == 0;
    
    %find connect componets
    CC = bwconncomp(BW);
    %find the bounding box;
    bbs  = regionprops(CC, 'BoundingBox');
    bbs  = cat(1,bbs.BoundingBox);
    
    %filter out too small or too large bb
    ind = bbs(:,4)>8  & bbs(:,3)>8 & bbs(:,4)< H/2 & bbs(:,3) < W/2;
    bbs1 = bbs(ind,:);
    
    %get the binary image;
    BW = binIm == 1;
    
    %find connect componets
    CC = bwconncomp(BW);
    %find the bounding box;
    bbs  = regionprops(CC, 'BoundingBox');
    bbs  = cat(1,bbs.BoundingBox);
    
    %filter out too small or too large bb
    ind = bbs(:,4)>8  & bbs(:,3)>8 & bbs(:,4)< H/2 & bbs(:,3) < W/2;
    bbs2 = bbs(ind,:);
    
    bbs = round([bbs1;bbs2]);
    
    
    bbs(:,3) = bbs(:,1) + bbs(:,3) - 1;
    bbs(:,4) = bbs(:,2) + bbs(:,4) - 1;
    
    
    bbsfilt = [];
    for i = 1:size(bbs,1)
        rect = bbs(i,1:4);
        match_score = matchscore(rect,gt_rects);
        if match_score < 0.01
            bbsfilt = [bbsfilt;rect];
            
        end
    end
    a = 1;
    b = size(bbsfilt,1);
    limit = 150;
    if b < limit
        ind = 1:b;
    else
        ind = randperm(b);
        ind = ind(1:limit);
    end
    
    bbox = bbsfilt(ind,:);
    features = zeros(size(bbox,1),325);
    for i = 1:size(bbox,1)
        rect = bbox(i,:);
        patch = img(rect(2):rect(4),rect(1):rect(3));
        [hp wp] = size(patch);
        aspect = hp / wp;
        if hp <= 24
            patch2 = imresize(patch,[16 16],'cubic');
            params = [9 4 2 0 0.2];
            hogfeat = HOG(patch2,params);
        elseif hp > 24 && hp <= 48
            patch2 = imresize(patch,[32 32],'cubic');
            params = [9 8 2 0 0.2];
            hogfeat = HOG(patch2,params);
        else
            patch2 = imresize(patch,[64 64],'cubic');
            params = [9 16 2 0 0.2];
            hogfeat = HOG(patch2,params);
        end
        %         fprintf('hp:%d,size:%d\n',hp,size(hogfeat,1));
        hogfeat = hogfeat';
        hogfeat = [hogfeat,aspect];
        features(i,:) = hogfeat;
    end
    features_neg = [features_neg;features];
    fprintf('image:%d,num of negative features:%d\n',k,size(features_neg,1));
end
trainingY = ones(size(features_neg,1),1)*2;
end
% for i = 1:1%size(gt_rects,1)
%     rect = gt_rects(i,1:4);
%     ind = bbs(:,3) < rect(1) | bbs(:,1) > rect(2) | bbs(:,4) < bbs(:,2) | bbs(:,2) > bbs(:,4);
%     tmp = bbs(ind,:);
% end
% patches = zeros(size(bbs,1),32);
%%
% imshow(img,[]);
% hold on;
% for i = 1:size(bbsfilt,1)
%     rect = bbsfilt(i,:);
%     patch = img(rect(2):rect(4),rect(1):rect(3));
%     rect(3) = rect(3) - rect(1);
%     rect(4) = rect(4) - rect(2);
%     rect = max(1,rect);
%     rectangle('Position',rect,'EdgeColor','b');
%
% end
% for i = 1:size(gt_rects,1)
%     rect = gt_rects(i,:);
%     rectangle('Position',[rect(1),rect(2),rect(3)-rect(1),rect(4)-rect(2)],'EdgeColor','y');
% end
% % for h = 1:32:H-step+1
% %     for w = 1:32:W-step+1
% %         if i > 120 break;end
% %         rect = [w,h,w+step-1,h+step-1];
% %         subimage = img(h:h+step-1,w:w+step-1);
% %         overlap_ratio = check_overlap(rect,gt_rects);
% %         if overlap_ratio < 0.01
% %             %imwrite(uint8(subimage),strcat('sub',num2str(i),'.png'));
% %             %             patches(i,:) = subimage(:)';
% %             %                 subimages(:,:,i) = subimage;
% %             patches =[patches; subimage(:)'];
% %
% %
% %             i = i+1;
% %
% %         end
% %
% %     end
% % end
% fprintf('Image No:%d, patches no.:%d\n',k,i);

