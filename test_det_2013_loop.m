function test_det_2013
%
% This script detects text in ICDAR 2013 scene images.
%
clear all;
prms = getParams;
prms.useswt = 0;
prms.usemser = 1;
prms.colorbin = 0;
prms.overlap = 1;

model = trainDetClf(1);
for i=1:0.2:4
    prms.distance_ratio = i;

    prms.date = datestr(now,'yymmdd_HHMM');
    writeLog(sprintf('Experiment started : %s %f',prms.date,prms.sw_ratio));
    expResult = execTextDet('icdar_2013_test', model, prms);
    save(sprintf('../savedata/TextDetection/data/result/%s.mat',prms.date),'expResult');

    analyzeResult(expResult);
end


