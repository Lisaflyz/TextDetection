function [trainingX, trainingY] = extractICDAR2013()



features = [];
labels = [];
for k = 100:328
%     k = 102;
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
    
    s = cell2mat(C{10});
    s = s(:,2);
    ind =  isstrprop(s,'alphanum');
    s = s(ind,:);
    s = map(s);
    
    x1 = max(0,x1-2);
    x2 = min(W-1,x2+2);
    y1 = max(0,y1-2);
    y2 = min(H-1,y2+2);
    
    gt_rects =double([x1 y1 x2 y2])+1;
    gt_rects = gt_rects(ind,:);
    
    for i =1:size(gt_rects,1)
        rect = gt_rects(i,:);
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
        features = [features;hogfeat];
        labels = [labels; s(i)];
        %     patch2 = imresize(patch,[32 32],'cubic');
        %     imwrite(uint8(patch),strcat(num2str(i),'.png'));
        %     imwrite(uint8(patch2),strcat(num2str(i),'_32.png'));
        
    end
    fprintf('image:%d,num of positive features:%d\n',k,size(features,1));
end
trainingX = features;
trainingY = labels;
% patch = img(rect(2):rect(4),rect(1):rect(3));
% imshow(patch,[]);
% patch2 = imresize(patch,[32 32],'cubic');
% imshow(patch2,[]);


end
