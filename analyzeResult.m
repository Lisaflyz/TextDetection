function analyzeResult(expResult)

dsinfo = loadDetDataset(expResult.prms.testset,1);
N = numel(dsinfo);
if N ~= numel(expResult.details); fprintf('ERROR\n'); end

% result
nums   = zeros(N,2);
result = zeros(N,6);
for i = 1:N
    d = expResult.details(i);
    nums(i,1) = size(dsinfo(i).bbs,1);
    nums(i,2) = size(d.dets,1);
    result(i,1:6) = d.result(1:6);
end

t03r = sum(result(:,1) .* nums(:,1)) / sum(nums(:,1));
t03p = sum(result(:,2) .* nums(:,2)) / sum(nums(:,2));
t03h = 2.0*t03p*t03r/(t03p+t03r);

t13r = sum(result(:,4) .* nums(:,1)) / sum(nums(:,1));
t13p = sum(result(:,5) .* nums(:,2)) / sum(nums(:,2));
t13h = 2.0*t13p*t13r/(t13p+t13r);

% fprintf('Evaluation 03: precision:%f,recall:%f,hmean:%f\n',t03p*100,t03r*100,t03h*100);
% fprintf('Evaluation 13: precision:%f,recall:%f,hmean:%f\n',t13p*100,t13r*100,t13h*100);
fprintf('Recall : \33[32m%2.2f%%\33[39m Precision:%2.2f%% Hmean:%2.2f%%\n\n',t13r*100,t13p*100,t13h*100);
writeLog(sprintf('%s Recall : \33[32m%2.2f%%\33[39m Precision:%2.2f%% Hmean:%2.2f%%' ...
        ,expResult.prms.date,t13r*100,t13p*100,t13h*100));


% output result for icdar2013, image
for i = 1:N
    d = expResult.details(i);

    res = zeros(1,4);
    if numel(d.dets)>0
        res = d.dets;
        res(:,3:4)=res(:,1:2)+res(:,3:4)-1;
    end
    dlmwrite(sprintf('res/res_img_%d.txt',i), res(:,1:4), 'newline', 'pc');

end

if 1
mkdir(fullfile('images',expResult.prms.date));
% output image
dir1 = fullfile('images',expResult.prms.date,'1');
dir2 = fullfile('images',expResult.prms.date,'2');
mkdir(dir1);
mkdir(dir2);
for i = 1:N
    CC = expResult.details(i).CC;
    idx = expResult.details(i).idx;
    % image1
    BW = zeros(CC.ImageSize);
    for j = 1:numel(CC.PixelIdxList)
        % ind2subで0に
        [y x] = ind2sub(CC.ImageSize, CC.PixelIdxList{j});
        for k = 1:numel(y)
            BW(y(k),x(k)) = 1;
        end
    end
    imwrite(BW,sprintf('%s/img_%d.jpg',dir1,i));
    % color
    color = getColor;
    I = ones([CC.ImageSize,3])*200;
    for j = 1:numel(CC.PixelIdxList)
        if idx(j) == -1
            c = [255 255 255];
        else
            c = color(mod(idx(j),18)+1,:);
        end
        [y x] = ind2sub(CC.ImageSize, CC.PixelIdxList{j});
        for k = 1:numel(y)
            I(y(k),x(k),1:3) = c(1:3);
        end
    end
    imwrite(uint8(I),sprintf('%s/img_%d.jpg',dir2,i));
end
end
end
    % if expResult.prms.saveimg==1
    %     I = imread(dsinfo(i).filename);
    %     rectimg = myrectangle(I,d.dets);
    %     imwrite(uint8(rectimg),sprintf('images/save/det_%d.jpg',i));
    % end
function color = getColor
color = ...
[255 255 100; 255 100 255; 255 100 100; 100 255 255; 100 255 100; 100 100 255; ...
255 255 0; 255 0 255; 255 0 0; 0 255 255; 0 255 0; 0 0 255; ...
150 150 0; 150 0 150; 150 0 0; 0 150 150; 0 150 0; 0 0 150];

end

