function getResultImage(expResult)

dsinfo = loadDetDataset(expResult.prms.testset,1);

mkdir(fullfile('images',expResult.prms.date));
% output image
dir1 = fullfile('images',expResult.prms.date,'1');
dir2 = fullfile('images',expResult.prms.date,'2');
dir3 = fullfile('images',expResult.prms.date,'rect');
mkdir(dir1);
mkdir(dir2);
mkdir(dir3);
for i = 1:numel(dsinfo)
    I_base = imread(dsinfo(i).filename);
    imgsize = size(I_base);

    CC = expResult.details(i).CC;
    CC.ImageSize = imgsize(1:2);
    idx = expResult.details(i).idx;
    % BW image
    BW = zeros(CC.ImageSize);
    for j = 1:numel(CC.PixelIdxList)
        % ind2subで0に
        [y x] = ind2sub(CC.ImageSize, CC.PixelIdxList{j});
        for k = 1:numel(y)
            BW(y(k),x(k)) = 1;
        end
    end
    imwrite(BW,sprintf('%s/img_%d.jpg',dir1,i));
    % color CC
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

    d = expResult.details(i);
    I = myrectangle(I_base,d.chars,[255 0 0]);
    I = myrectangle(I,d.words,[0 255 0]);
    imwrite(uint8(I),sprintf('%s/img_%d.jpg',dir3,i));
end
end



function color = getColor
color = ...
[255 255 100; 255 100 255; 255 100 100; 100 255 255; 100 255 100; 100 100 255; ...
255 255 0; 255 0 255; 255 0 0; 0 255 255; 0 255 0; 0 0 255; ...
150 150 0; 150 0 150; 150 0 0; 0 150 150; 0 150 0; 0 0 150];

end

