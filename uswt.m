function CC = swt2cc(SWT)

file = '../images/detection/icdar2013/test/img_7.jpg';

I = imread(file);
addpath('swt/');
SWT = swt(double(rgb2gray(I)),1);
if 1
    label = labeling_swt_matlab(SWT);
    CC = label2cc(label);
else
    [label CC] = labeling_swt(SWT);
end
    
% imshow(label)

end

function flag = isok(label, sw1, sw2)
flag = 0;
thr = 3;
if label == 0, return; end
if sw1 < 0 || sw2 < 0, return; end
if sw1/sw2 > 3 || sw2/sw1 > 3, return; end
flag = 1;
end

function CC = label2cc(label)
[H W] = size(label);
max(max(label))
pixels = cell(1,max(max(label)));
for i=1:H
    for j=1:W
        if label(i,j) > 0
            pixels{label(i,j)} = [pixels{label(i,j)},sub2ind([H W],i,j)];
        end
    end
end
CC.PixelIdxList = pixels;

llist = unique(label);
llist = llist(llist~=0);

CC.PixelIdxList = [];
for i=1:numel(llist)
    pixels = {find(label==llist(i))};
    CC.PixelIdxList = [CC.PixelIdxList, pixels];
end
CC.NumObjects = numel(CC.PixelIdxList);
CC.ImageSize = size(label);
CC.Connectivity = 8;
end

function label = labeling_swt_matlab(SWT)
    [H W] = size(SWT);
    label = zeros(H,W);
    L = 0;
    thr = 3;
    for i = 1:H
        for j=1:W
            p = SWT(i,j);
            % 背景、ラベル付けされてる画素は無視
            if p < 0 || label(i,j) > 0
                continue;
            end

            % 隣接していて連結する可能性のある画素を取得
            nei = [];
            if i~=1 && j~=1 && isok(label(i-1,j-1),SWT(i-1,j-1),SWT(i,j))
                nei = [nei, label(i-1,j-1)];
            end
            if i~=1 && j~=W && isok(label(i-1,j+1),SWT(i-1,j+1),SWT(i,j))
                nei = [nei, label(i-1,j+1)];
            end
            if i~=1 && isok(label(i-1,j),SWT(i-1,j),SWT(i,j))
                nei = [nei, label(i-1,j)];
            end
            if j~=1 && isok(label(i,j-1),SWT(i,j-1),SWT(i,j))
                nei = [nei, label(i,j-1)];
            end
            nei = unique(nei(nei~=0));
            num = numel(nei);
            if num==0 % 周りが全てラベリングされてない
                L = L+1;
                label(i,j) = L;
            elseif num==1 % 周りが同じラベル
                label(i,j) = nei(1);
            elseif num==2 % 周りに２つラベル(上の行と左のピクセル)
                label(i,j) = min(nei);
                label(find(label==max(nei))) = min(nei);
            end
        end
    end
end
