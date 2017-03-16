% function carIdentify ()
clear all;
clc

[Image_ID] = csvread ('Plate_Index.csv',0,0,[0,0,0,0]);

for i =1:length (Image_ID)
filename=[int2str(Image_ID(i)),'.jpg'];
filename
img = imread (['Plate_Image\',filename]);

%高斯
a = imgaussfilt (img, 3);

%灰度化
a = rgb2gray (a);

%sobel边缘检测
a = edge (a, 'Sobel');
% imshow (a);

%开闭操作
% 开运算：去除较小的明亮区域
% 闭运算：消除低亮度值的孤立点
se = strel ('rectangle',[7 27]); 
a = imclose (a,se);

a = imopen (a,se);

se = strel ('rectangle',[7 95]); 
a = imclose (a,se);

se = strel ('rectangle',[11 3]); 
a = imopen (a,se);

imwrite (a,'imout.jpg');

%画出所有的外接矩型(代码来自MATLAB中文论坛)
[l,m] =  bwlabel (a,8);
status = regionprops (l,'BoundingBox');
figure(10);
imshow (img);
hold on;
for j = 1:m
    rectangle ('position', status(j).BoundingBox, 'edgecolor', 'r');
end
hold off;
frame = getframe;
rec = frame2im(frame);
imwrite(rec,['S3_Rectangle_Image\',filename])

%查找最接近的图形
for k = 1:m
%左边距的x坐标<200的，丢弃
    if status(k).BoundingBox(1) < 200
        status(k).BoundingBox(1)
        continue
%横向尺寸少于200的，丢弃
    elseif status(k).BoundingBox(3) < 200
        status(k).BoundingBox(3)
        continue
    elseif status(k).BoundingBox(4) < 50
        status(k).BoundingBox(4)
        continue
    else
        i2 = imcrop (img,status(k).BoundingBox);
        imwrite (i2,['S4_Crop_Image\',int2str(Image_ID(i)),'_',int2str(k),'.jpg']);
    end
end
end

