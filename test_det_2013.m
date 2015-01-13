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


prms.date = datestr(now,'yymmdd_HHMM');
writeLog(sprintf('Experiment started : %s',prms.date));
model = trainDetClf(1);

expResult = execTextDet('icdar_2013_test', model, prms);
save(sprintf('data/result/%s.mat',prms.date),'expResult');

analyzeResult(expResult);
genResultImage(expResult);
genMongoQuery(expResult);
writeLog(sprintf('Experiment ended : %s',prms.date));
