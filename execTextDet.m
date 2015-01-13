function expResult = execTextDet(testset, model, prms)

addpath DetEval;

prms.testset = testset;
expResult.prms = prms;

dsinfo = loadDetDataset(testset,1);
N = numel(dsinfo);
expResult.result = zeros(N,6);
expResult.details = repmat(struct('lines',[],'words',[],'chars',[], ...
    'dets',[],'result',[],'CC',[],'idx',[]),N,1);

for k = 1:N
    fprintf('\n# image %d %s\n', k, prms.date);
    I = imread(dsinfo(k).filename);
    if prms.adjust, I = adjustImage(I,0.05); end
    scale = 1;
    if size(I,1)>2000, scale = 0.3; I = imresize(I,scale); else scale = 1; end

    det = detText(I, model, prms);
    det.lines = det.lines/scale; det.words = det.words/scale; det.chars = det.chars/scale;

    % recall = calcDetScore(dsinfo(k).bbs,det.words);
    % for i = 1:numel(dsinfo(k).tag)
    %     fprintf('%2d : %5.1f%%  %s\n',i,recall(i)*100,dsinfo(k).tag{i});
    % end
    result = DetEval(det.words, dsinfo(k).bbs); % [r p h(2003) r p h(deteval)]
    fprintf('Result : R\033[32m%.4f\033[39m P\033[33m%.4f\033[39m\n', result(4),result(5));
    fprintf('Char :%4d, Word :%3d, Line :%3d\n', ...
        size(det.chars,1),size(det.words,1),size(det.lines,1));

    % detail = struct('lines',lines,'words',words,'chars',chars, ...
    %     'dets',dets,'result',result,'CC',CC,'idx',idx);
    expResult.details(k) = det;
    expResult.result(k,1:6) = result(1:6);

    if k==50 || k==100, save(sprintf('data/result/%s.mat',prms.date),'expResult'); end
end


