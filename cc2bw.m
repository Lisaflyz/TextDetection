function BW = cc2bw(CC)
imsize = CC.ImageSize; % [H W]
imsize

BW = zeros(imsize);
for i=1:CC.NumObjects
    [y x] = ind2sub(imsize,CC.PixelIdxList{i});
    for j=1:numel(y)
        idx = CC.PixelIdxList{i}(j);
        BW(y(j),x(j)) = 1;
    end
end

