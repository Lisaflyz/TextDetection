function writeLog(text)

%fid = fopen(sprintf('log/log%s.txt',datestr(date,'mmdd')),'a');
fid = fopen('log/log.txt','a');
%fprintf(fid,'[%s] %s\n',datestr(now,'HH:MM:SS'),text);
fprintf(fid,'[%s] %s\n',datestr(now,'yyyy-mm-dd HH:MM:SS'),text);
fclose(fid);
