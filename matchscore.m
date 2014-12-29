function [ max_score ] = matchscore( rect, gt_rects )
%CHECKOVERLAP Summary of this function goes here
%   Detailed explanation goes here
max_score = 0;
for i = 1:size(gt_rects,1)
    gt_rect = gt_rects(i,:);

    left = max(rect(1),gt_rect(1));
    right = min(rect(3), gt_rect(3));
    top = max(rect(2), gt_rect(2));
    bottom = min(rect(4), gt_rect(4));
    height = bottom - top;
    width = right - left;
    
    if height < 0 || width < 0
        match_score = 0;
    else
        match_score = height*width / ((rect(3)-rect(1))*(rect(4)-rect(2)));
    end
    if match_score > max_score
        max_score = match_score;
        max_gt = gt_rect;
    end
    
end

% if max_overlap_ratio > 0.8
%     fprintf('rect:%d,%d,%d,%d,gt_rect:%d,%d,%d,%d, ratio:%f\n',...
%         rect(1),rect(2),rect(3),rect(4),...
%         max_gt(1),max_gt(2),max_gt(3),max_gt(4),max_overlap_ratio);
% end

end

