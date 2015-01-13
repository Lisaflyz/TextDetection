function analyzeResult(expResult)

dsinfo = loadDetDataset(expResult.prms.testset,1);
N = numel(dsinfo);
if N ~= numel(expResult.details); fprintf('ERROR\n'); end

% result
nums   = zeros(N,2);
result = zeros(N,6);
for i = 1:N
    d = expResult.details(i);
    nums(i,1) = size(dsinfo(i).bbs,1);
    nums(i,2) = size(d.dets,1);
    result(i,1:6) = d.result(1:6);
end

t03r = sum(result(:,1) .* nums(:,1)) / sum(nums(:,1));
t03p = sum(result(:,2) .* nums(:,2)) / sum(nums(:,2));
t03h = 2.0*t03p*t03r/(t03p+t03r);

t13r = sum(result(:,4) .* nums(:,1)) / sum(nums(:,1));
t13p = sum(result(:,5) .* nums(:,2)) / sum(nums(:,2));
t13h = 2.0*t13p*t13r/(t13p+t13r);

% fprintf('Evaluation 03: precision:%f,recall:%f,hmean:%f\n',t03p*100,t03r*100,t03h*100);
% fprintf('Evaluation 13: precision:%f,recall:%f,hmean:%f\n',t13p*100,t13r*100,t13h*100);
fprintf('Recall : \33[32m%2.2f%%\33[39m Precision:%2.2f%% Hmean:%2.2f%%\n\n',t13r*100,t13p*100,t13h*100);
p = expResult.prms;
writeLog(sprintf('%s R:%2.2f%% P:%2.2f%% H:%2.2f%% sw:%.2f h:%.2f' ...
        ,p.date,t13r*100,t13p*100,t13h*100,p.sw_ratio,p.distance_ratio));


% output result for icdar2013
for i = 1:N
    d = expResult.details(i);
    I = imread(dsinfo(i).filename);
    H = size(I,1);
    W = size(I,2);

    res = zeros(1,4);
    if numel(d.dets)>0
        res = d.dets;
        res(:,3:4)=res(:,1:2)+res(:,3:4)-1;
    end
    for j=1:size(res,1)
        if res(j,3) >= W
            res(j,3) = W-1;
        end
        if res(j,4) >= H
            res(j,4) = H-1;
        end
    end
    dlmwrite(sprintf('res/res_img_%d.txt',i), round(res(:,1:4)), 'newline', 'pc');
end

end
