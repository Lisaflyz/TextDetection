function drawBBs(file,bbs,bbs2,num)

% I = imread(file);
% for i = 1 : size(bbs,1)
%     bb = bbs(i,:);
%     I(bb(2):bb(2)+1,bb(1):bb(1)+bb(3),1) = 0;
%     I(bb(2):bb(2)+1,bb(1):bb(1)+bb(3),2) = 255;
%     I(bb(2):bb(2)+1,bb(1):bb(1)+bb(3),3) = 0;
%     I(bb(2)+bb(4)-1:bb(2)+bb(4),bb(1):bb(1)+bb(3),1) = 0;
%     I(bb(2)+bb(4)-1:bb(2)+bb(4),bb(1):bb(1)+bb(3),2) = 255;
%     I(bb(2)+bb(4)-1:bb(2)+bb(4),bb(1):bb(1)+bb(3),3) = 0;
%     I(bb(2):bb(2)+bb(4),bb(1):bb(1)+1,1) = 0;
%     I(bb(2):bb(2)+bb(4),bb(1):bb(1)+1,2) = 255;
%     I(bb(2):bb(2)+bb(4),bb(1):bb(1)+1,3) = 0;
%     I(bb(2):bb(2)+bb(4),bb(1)+bb(3)-1:bb(1)+bb(3),1) = 0;
%     I(bb(2):bb(2)+bb(4),bb(1)+bb(3)-1:bb(1)+bb(3),2) = 255;
%     I(bb(2):bb(2)+bb(4),bb(1)+bb(3)-1:bb(1)+bb(3),3) = 0;
% end
% for i = 1 : size(bbs2,1)
%     bb = bbs2(i,:);
%     I(bb(2):bb(2)+1,bb(1):bb(1)+bb(3),1) = 255;
%     I(bb(2):bb(2)+1,bb(1):bb(1)+bb(3),2) = 0;
%     I(bb(2):bb(2)+1,bb(1):bb(1)+bb(3),3) = 0;
%     I(bb(2)+bb(4)-1:bb(2)+bb(4),bb(1):bb(1)+bb(3),1) = 255;
%     I(bb(2)+bb(4)-1:bb(2)+bb(4),bb(1):bb(1)+bb(3),2) = 0;
%     I(bb(2)+bb(4)-1:bb(2)+bb(4),bb(1):bb(1)+bb(3),3) = 0;
%     I(bb(2):bb(2)+bb(4),bb(1):bb(1)+1,1) = 255;
%     I(bb(2):bb(2)+bb(4),bb(1):bb(1)+1,2) = 0;
%     I(bb(2):bb(2)+bb(4),bb(1):bb(1)+1,3) = 0;
%     I(bb(2):bb(2)+bb(4),bb(1)+bb(3)-1:bb(1)+bb(3),1) = 255;
%     I(bb(2):bb(2)+bb(4),bb(1)+bb(3)-1:bb(1)+bb(3),2) = 0;
%     I(bb(2):bb(2)+bb(4),bb(1)+bb(3)-1:bb(1)+bb(3),3) = 0;
% end
% imshow(I);
% return;

imshow(file);

for i = 1 : size(bbs,1)
	color = [0,1,0];
	rectangle('Position',bbs(i,1:4),'LineWidth',2,'LineStyle','-','EdgeColor',color);
end
if nargin == 3
	for i = 1 : size(bbs2,1)
		color = [1,0,0];
		rectangle('Position',bbs2(i,1:4),'LineWidth',2,'LineStyle','-','EdgeColor',color);
	end
end



if nargin == 4
    saveas(f_handle, sprintf('../images/save/detect/%d.png',num));
end
