function diffResult(id1, id2, n)

load(sprintf('data/result/%s.mat',id1));
r1 = expResult.result(:,4);
p1 = expResult.result(:,5);
load(sprintf('data/result/%s.mat',id2));
r2 = expResult.result(:,4);
p2 = expResult.result(:,5);
dsinfo = loadDetDataset(expResult.prms.testset,1);
N = numel(dsinfo);
if nargin==3
    N = n;
end

for i=1:N
    if abs(r1(i)-r2(i))<0.01
        fprintf('%3d : Recall %8.2f%% %8.2f%%',i,r1(i)*100,r2(i)*100);
    elseif r1(i) < r2(i)
        fprintf('%3d : Recall %8.2f%% \033[32m%8.2f%%\033[39m',i,r1(i)*100,r2(i)*100);
    elseif r1(i) > r2(i)
        fprintf('%3d : Recall \033[32m%8.2f%%\033[39m %8.2f%%',i,r1(i)*100,r2(i)*100);
    end

    if abs(p1(i)-p2(i))<0.01
        fprintf('     Precision %8.2f%% %8.2f%%\n',p1(i)*100,p2(i)*100);
    elseif p1(i) < p2(i)
        fprintf('     Precision %8.2f%% \033[32m%8.2f%%\033[39m\n',p1(i)*100,p2(i)*100);
    elseif p1(i) > p2(i)
        fprintf('     Precision \033[32m%8.2f%%\033[39m %8.2f%%\n',p1(i)*100,p2(i)*100);
    end
end
