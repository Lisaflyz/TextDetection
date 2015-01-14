function test_det_2013
%
% This script detects text in ICDAR 2013 scene images.
%
clear all;
prms = getParams;

model = trainDetClf(1);
% for i=[2.9 2 2.5 3.5 4 1.5], for j=1.5:0.5:2.5, for k=0.05:0.05:0.25
for i=0.1:0.05:0.5
    prms.clfthr = i;
    % prms.distance_ratio = i;
    % prms.sw_ratio = j;
    % prms.color_diff = k;

    prms.date = datestr(now,'yymmdd_HHMM');
    writeLog(sprintf('Experiment started : %s %f',prms.date,prms.clfthr));
    expResult = execTextDet('icdar_2013_test', model, prms);
    save(sprintf('../savedata/TextDetection/data/result/%s.mat',prms.date),'expResult');

    analyzeResult(expResult);
end; 


