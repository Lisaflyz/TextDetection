function utest(n)
%
% This script detects text in ICDAR 2013 scene images.
%
prms = getParams;
prms.useswt = 0;
prms.usemser = 1;
prms.colorbin = 0;
prms.overlap = 1;
prms.sw_ratio = 2;
prms.date = datestr(now,'yymmdd_HHMM');




model = trainDetClf(1);
addpath DetEval;


dsinfo = loadDetDataset('icdar_2013_test',1);
N = numel(dsinfo);

for k = n:n
    fprintf('\n# image %d %s\n', k, prms.date);
    I = imread(dsinfo(k).filename);
    I = adjustImage(I,0.05);
    scale = 1;
    if size(I,1)>2000, scale = 0.3; I = imresize(I,scale); else scale = 1; end

    det = detText2(I, model, prms);
    lines=det.lines; words=det.words;chars=det.chars;
    lines = lines/scale; words = words/scale; chars = chars/scale;

    dets = words;
    recall = calcDetScore(dsinfo(k).bbs,dets);
    for i = 1:numel(dsinfo(k).tag)
        fprintf('%2d : %5.1f%%  %s\n',i,recall(i)*100,dsinfo(k).tag{i});
    end
    result = DetEval(dets, dsinfo(k).bbs); % [r p h(2003) r p h(deteval)]
    fprintf('Result : \033[32m%.4f\033[39m\n', result(4));
    fprintf('Char :%4d, Word :%3d, Line :%3d\n',size(chars,1),size(words,1),size(lines,1));

    % detail = struct('lines',lines,'words',words,'chars',chars, ...
    %     'dets',dets,'result',result,'CC',CC,'idx',idx);

end

f = figure('Visible','on');imshow(uint8(I));
hold on;
for i = 1:size(chars,1)
    rect = chars(i,1:4);
    rectangle('Position',rect,'EdgeColor','g');
end
for i = 1:size(lines,1)
    rect = lines(i,1:4);
    rectangle('Position',rect,'EdgeColor','r','LineWidth',2);
end
for i = 1:size(words,1)
   rect = words(i,1:4);
   rectangle('Position',rect,'EdgeColor','b');
end



