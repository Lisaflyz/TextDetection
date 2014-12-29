function [ feature ] = computefeature( patch )
%comptue HOG features for an image patch
%when patch is very wide, possible with multipe characters, in that case
%returned features is a matrix contains multipe sample feature
[hp wp] = size(patch);
if hp > 64
    patch = medfilt2(patch, [4 4], 'symmetric');
%     patch = bilateralFilter(patch);
%     imwrite(uint8(patch),strcat('patch/patch',num2str(hp),'.png'));

end
hogfeat = [];
aspect = hp / wp;
if hp * 1.5 > wp
    hogfeat = compute(patch);
    hogfeat = hogfeat';
    feature = [hogfeat,aspect];
else % when component is very wide, possible with multipe characters
    stepsize = round(hp / 10);
    for i = 1:stepsize:wp-hp-1
        subpatch = patch(:,i:i+hp);
        subfeat = compute(subpatch);
        subfeat = subfeat';
        hogfeat = [hogfeat;subfeat];
        
    end
    feature = [hogfeat,ones(size(hogfeat,1),1)];
end





function [ hogfeat ] = compute(patch)
[hp wp] = size(patch);

%resize the patch according to its height and compute HOG;
if hp <= 24
    patch2 = imresize(patch,[16 16],'cubic');
    params = [9 4 2 0 0.2];
    hogfeat = HOG(patch2,params);
elseif hp > 24 && hp <= 48
    patch2 = imresize(patch,[32 32],'cubic');
    params = [9 8 2 0 0.2];
    hogfeat = HOG(patch2,params);
else
    patch2 = imresize(patch,[64 64],'cubic');
    params = [9 16 2 0 0.2];
    hogfeat = HOG(patch2,params);
end


