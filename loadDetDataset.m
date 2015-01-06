function data = loadDetDataset(dataset, usemat)
if usemat
    matpath = sprintf('data/loadDetDataset_%s.mat',dataset);
    if exist(matpath,'file')
        load(matpath);
        return;
    end
end

switch dataset
    case 'icdar_2013_test'
        data = icdar2013Test;
    otherwise
        return;
end
save(sprintf('data/loadDetDataset_%s.mat',dataset), 'data');

end


function data = icdar2013Test
path = '../images/detection/icdar2013/';

data = [];
for i=1:233
    imgpath = fullfile(path,sprintf('test/img_%d.jpg',i));
    gtpath  = fullfile(path,sprintf('test_gt/gt_img_%d.txt',i));
    fid = fopen(gtpath,'r');
    scanned = textscan(fid,'%[^,], %[^,], %[^,], %[^,], "%[^"]"');
    sbbs = [scanned{1}, scanned{2}, scanned{3}, scanned{4}]; % [x1 y1 x2 y2]
    gts = scanned{5};
    fclose(fid);
    bbs = [];
    for j=1:numel(gts)
        bb = [str2num(sbbs{j,1}), str2num(sbbs{j,2}), str2num(sbbs{j,3}), str2num(sbbs{j,4})];
        bb(:,3:4) = bb(:,3:4) - bb(:,1:2) + 1;
        bbs = [bbs; bb];
    end
    img.imgID    = i;
    img.tag      = gts;
    img.bbs      = bbs; % [x y w h]
    img.filename = imgpath;
    data = [data; img];
end

end


