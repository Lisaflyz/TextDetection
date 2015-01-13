function CC = filterCC(CC,idx)

CC.PixelIdxList = CC.PixelIdxList(idx);
CC.NumObjects = numel(CC.PixelIdxList);
