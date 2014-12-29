addpath 'liblinear-1.94/matlab/';

%% extract positive and negative samples
[trainingX, trainingY] = extractICDAR2013();

[features_neg, trainingY] = extractICDAR2013neg();


save positive trainingX;
save negative features_neg;

% load positive.mat;
% load negative.mat;

trainXCs1 = trainingX;
% trainY1 = trainingY;
trainY1 = ones(size(trainXCs1,1),1);




trainXCs2 = features_neg;
trainY2 = ones(size(trainXCs2,1),1)*2;


trainXCs = [trainXCs1;trainXCs2];
trainY = [trainY1;trainY2];



%% train the model
model = train(trainY, sparse(trainXCs),'-s 0');

[labels, accuracy, prob] = predict(trainY,sparse(trainXCs),model,'-b 1');

save model_0528 model;

