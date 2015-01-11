function test_det_2013
%
% This script detects text in ICDAR 2013 scene images.
%
clear all;
prms = getParams;
prms.date = datestr(now,'yymmdd_HHMM');
prms.useswt = 0;

writeLog(sprintf('Experiment started : %s',prms.date));
model = trainDetClf(1);

expResult = execTextDet('icdar_2013_test', model, prms);


save(sprintf('data/result/%s.mat',prms.date),'expResult');
analyzeResult(expResult);
writeLog(sprintf('Experiment ended : %s',prms.date));
% evaldet

end


function expResult = execTextDet(testset, model, prms)

addpath DetEval;

prms.testset = testset;
expResult.prms = prms;
expResult.details = [];

dsinfo = loadDetDataset(testset,1);

for k = 1:numel(dsinfo)
    fprintf('\n# image %d %s\n', k, prms.date);
    I = imread(dsinfo(k).filename);

    [lines words chars CC idx] = detText(double(rgb2gray(I)), model, prms);
    dets = words;
    recall = calcDetScore(dsinfo(k).bbs,dets);
    for i = 1:numel(dsinfo(k).tag)
        fprintf('%2d : %5.1f%%  %s\n',i,recall(i)*100,dsinfo(k).tag{i});
    end
    result = DetEval(dets, dsinfo(k).bbs); % [r p h(2003) r p h(deteval)]
    fprintf('Result : \033[32m%.4f\033[39m\n', result(4));
    fprintf('Char :%4d, Word :%3d, Line :%3d\n',size(chars,1),size(words,1),size(lines,1));

    detail = struct('lines',lines,'words',words,'chars',chars, ...
        'dets',dets,'result',result,'CC',CC,'idx',idx);
    expResult.details  = [expResult.details; detail];
end
end



