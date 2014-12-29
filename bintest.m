function img = bintest(n, ni)
addpath('../TextDetection/');
%file = sprintf('/home/matlab/liuxinhao/icdar2003/SceneTrialTest/ryoungt_05.08.2002/%s',n);
file = sprintf('../WordRecognition/testimage/%d.png',n)
img = imread(file);
if ni
    imshow(bt_niblackbin(rgb2gray(img))==1);
else
    img = otsubin(rgb2gray(img));
    imshow(img);
end

