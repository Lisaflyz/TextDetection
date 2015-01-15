function test_det_2013
%
% This script detects text in ICDAR 2013 scene images.
%
clear all;
prms = getParams;


prms.date = datestr(now,'yymmdd_HHMM');
writeLog(sprintf('Experiment started : %s',prms.date));
model = trainDetClf(1);

prms.maxthr = 0.95;
prms.maxthr2 = 0.9;
prms.finalthr = 0.30;
expResult = execTextDet('icdar_2013_test', model, prms);
save(sprintf('data/result/%s.mat',prms.date),'expResult');

analyzeResult(expResult);
genResultImage(expResult);
genMongoQuery(expResult);
writeLog(sprintf('Experiment ended : %s',prms.date));
