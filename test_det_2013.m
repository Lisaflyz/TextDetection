function test_det_2013
%
% This script detects text in ICDAR 2013 scene images.
%
clear all;
prms = getParams;
prms.saveimg = 1;
prms.date = datestr(now,'yymmdd_HHMM');

writeLog(sprintf('Experiment started : %s',prms.date));
model = trainDetClf(1);

expResult = execTextDet('icdar_2013_test', model, prms);

save(sprintf('data/result/%s.mat',date),'expResult');
writeLog(sprintf('Experiment ended : %s',prms.date));
% evaldet

end


function expResult = execTextDet(dataset, model, prms)

expResult.prms = prms;
expResult.details = [];

dsinfo = loadDetDataset(dataset,1);

for k = 1:numel(dsinfo)
    fprintf('\n# image %d %s\n', k, prms.date);
    I = imread(dsinfo(k).filename);

    [lines words chars] = detText(double(rgb2gray(I)), model);
    recall = calcDetScore(dsinfo(k).bbs,lines);
    for i = 1:numel(dsinfo(k).tag)
        fprintf('%2d : %5.1f%%  %s\n',i,recall(i)*100,dsinfo(k).tag{i});
    end
    fprintf('Recall : %.4f\n', sum(recall)/numel(recall));

    res = lines;
    res(:,3:4)=res(:,1:2)+res(:,3:4)-1;

    dlmwrite(sprintf('res/res_img_%d.txt',k), res(:,1:4), 'newline', 'pc');
    if prms.saveimg==1
        rectimg = myrectangle(I,words);
        imwrite(uint8(rectimg),sprintf('images/save/det_%d.jpg',k));
    end

    detail.lines = lines;
    detail.words = words;
    detail.chars = chars;
    expResult.details  = [expResult.details; detail];
end
end
