function model = trainDetClf(loadflag)

addpath 'liblinear-1.94/matlab/';

if loadflag
    load data/model_deteng
    return;
else
    % load data/tmp/positive;
    % load data/tmp/negative;
    [trainingX, ~] = extractICDAR2013();
    save data/tmp/positive trainingX;
    [features_neg, ~] = extractICDAR2013neg();
    save data/tmp/negative features_neg;
end

trainXCs1 = trainingX;
trainXCs2 = features_neg;
trainY1 = ones(size(trainXCs1,1),1);
trainY2 = ones(size(trainXCs2,1),1)*2;

trainXCs = [trainXCs1;trainXCs2];
trainY = [trainY1;trainY2];

%% train the model
model = train(trainY, sparse(trainXCs),'-s 0');
save data/model_deteng model;

%% test
[labels, accuracy, prob] = predict(trainY,sparse(trainXCs),model,'-b 1');
