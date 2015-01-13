function img = adjustImage(img, v)

[H W ch] = size(img);
if ch == 1
    img = imadjust(img,[v 1-v]);
elseif ch == 3
    %img = imadjust(img,[.05 .05 0.05; .95 .95 0.95],[]);
    img = imadjust(img,[v v v; 1-v 1-v 1-v],[]);
end

