function generate_mongo_query(expResult)

id = expResult.prms.date
load(sprintf('data/result/%s.mat',id));

f = fopen(sprintf('expdata/query/%s.txt',id),'w');


% delete query
fprintf(f,'db.get_detail.remove({exp_id:"%s"});\n',id);

for i=1:numel(expResult.details)
    d = expResult.details(i);
    str = sprintf('{"exp_id":"%s","img_id":%d,',id,i);
    str = sprintf('%s"lines":[',str);
    for j=1:size(d.lines,1)
        line = d.lines(j,:);
        str = sprintf('%s[%d,%d,%d,%d],\n',str,line(1),line(2),line(3),line(4));
    end
    str = strcat(str,'],');

    str = sprintf('%s"words":[',str);
    for j=1:size(d.words,1)
        word = d.words(j,:);
        str = sprintf('%s[%d,%d,%d,%d],\n',str,word(1),word(2),word(3),word(4));
    end
    str = strcat(str,'],');

    str = sprintf('%s"chars":[',str);
    for j=1:size(d.chars,1)
        char = d.chars(j,:);
        str = sprintf('%s[%d,%d,%d,%d],\n',str,char(1),char(2),char(3),char(4));
    end
    str = strcat(str,'],');
    str = strcat(str,'}');
    fprintf(f,'db.det_detail.insert(%s);\n',str);
end
fclose(f);


% dsinfo = loadDetDataset('icdar_2013_test',1);
% f = fopen(sprintf('expdata/query/icdar2013.txt',id),'w');
% for i=1:numel(dsinfo)
%     str = sprintf('{"img_id":%d,',i);
%     str = sprintf('%s"bbs":[',str);
%     for j=1:size(dsinfo(i).bbs,1)
%         bb = dsinfo(i).bbs(j,:);
%         str = sprintf('%s[%d,%d,%d,%d],',str,bb(1),bb(2),bb(3),bb(4));
%     end
%     str = strcat(str,'],');
%     str = sprintf('%s"tags":[',str);
%     for j=1:size(dsinfo(i).tag,1)
%         tag = dsinfo(i).tag{j};
%         str = sprintf('%s"%s",',str,tag);
%     end
%     str = strcat(str,'],');
%     str = strcat(str,'}');
%     fprintf(f,'db.icdar_2013_gt.insert(%s);\n',str);
% end
