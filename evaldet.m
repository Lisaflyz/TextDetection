function [precision,recall,hmean] = evaldet

detpath = 'res/';
N = 233;

nums = zeros(N,2);
result = zeros(N,6);
for k = 1:N
    imgfile = strcat('../images/detection/icdar2013/test/img_',num2str(k),'.jpg');
    [pathstr, name, ext] = fileparts(imgfile);

    file = strcat('../images/detection/icdar2013/test_gt/gt_img_',num2str(k),'.txt');
    fileID = fopen(file);
    C = textscan(fileID,'%d, %d, %d, %d, %s','commentStyle','#');
    gt_rects =double([C{1} C{2} C{3} C{4}]);
    nums(k,1) = size(gt_rects,1);

    file = strcat(detpath,'res_img_',num2str(k),'.txt');
    fileID = fopen(file);
    C = textscan(fileID,'%d, %d, %d, %d','commentStyle','#');
    dt_rects =double([C{1} C{2} C{3} C{4}]);
    nums(k,2) = size(dt_rects,1);    

    output = detevalmex(dt_rects,gt_rects);

    result(k,1:3) = output(1,:);
    result(k,4:6) = output(2,:);
end

%%
t03p = sum(result(:,1) .* nums(:,2)) / sum(nums(:,2));
t03r = sum(result(:,2) .* nums(:,1)) / sum(nums(:,1));
t03h = 2.0*t03p*t03r/(t03p+t03r);

t13p = sum(result(:,4) .* nums(:,2)) / sum(nums(:,2));
t13r = sum(result(:,5) .* nums(:,1)) / sum(nums(:,1));
t13h = 2.0*t13p*t13r/(t13p+t13r);

fprintf('Evaluation 03: precision:%f,recall:%f,hmean:%f\n',t03p,t03r,t03h);
fprintf('Evaluation 13: precision:%f,recall:%f,hmean:%f\n',t13p,t13r,t13h);

precision=t03p; recall=t03r; hmean=t03h;
end
