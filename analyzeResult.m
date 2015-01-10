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

fprintf('Evaluation 03: precision:%f,recall:%f,hmean:%f\n',t03p,t03r,t03h);
fprintf('Evaluation 13: precision:%f,recall:%f,hmean:%f\n',t13p,t13r,t13h);


% output result for icdar2013, image
for i = 1:N
    d = expResult.details(i);

    res = zeros(1,4);
    if numel(d.dets)>0
        res = d.dets;
        res(:,3:4)=res(:,1:2)+res(:,3:4)-1;
    end
    dlmwrite(sprintf('res/res_img_%d.txt',i), res(:,1:4), 'newline', 'pc');

    % if expResult.prms.saveimg==1
    %     I = imread(dsinfo(i).filename);
    %     rectimg = myrectangle(I,d.dets);
    %     imwrite(uint8(rectimg),sprintf('images/save/det_%d.jpg',i));
    % end
end
end
