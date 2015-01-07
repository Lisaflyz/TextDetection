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

    [lines words chars] = detText(double(rgb2gray(I)), model, prms);
    recall = calcDetScore(dsinfo(k).bbs,lines);
    recall2 = calcDetScore(dsinfo(k).bbs,words);
    for i = 1:numel(dsinfo(k).tag)
        % fprintf('%2d : %5.1f%%  %s\n',i,recall(i)*100,dsinfo(k).tag{i});
        if recall(i) == recall2(i)
            fprintf('%2d : %5.1f%%  %s\n',i,recall(i)*100,dsinfo(k).tag{i});
        elseif recall(i) < recall2(i)
            fprintf('%2d : %5.1f%% (\033[32m%5.1f%%\033[39m)  %s\n',i,recall(i)*100,recall2(i)*100,dsinfo(k).tag{i});
        else
            fprintf('%2d : %5.1f%% (\033[31m%5.1f%%\033[39m)  %s\n',i,recall(i)*100,recall2(i)*100,dsinfo(k).tag{i});
        end
    end
    fprintf('Recall : %.4f\n', sum(recall)/numel(recall));
    fprintf('Char :%4d, Word :%3d, Line :%3d\n',size(chars,1),size(words,1),size(lines,1));

    res = lines;
    res(:,3:4)=res(:,1:2)+res(:,3:4)-1;

    dlmwrite(sprintf('res/res_img_%d.txt',k), res(:,1:4), 'newline', 'pc');
    if prms.saveimg==1
        rectimg = myrectangle(I,lines);
        imwrite(uint8(rectimg),sprintf('images/save/det_%d.jpg',k));
    end

    detail.lines = lines;
    detail.words = words;
    detail.chars = chars;
    expResult.details  = [expResult.details; detail];
end
end
