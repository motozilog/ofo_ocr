clc
clear all;
a = imread('ofo.jpg');
imshow(a)

a = rgb2gray (a);
a = edge (a, 'Sobel');
imshow (a)

img  = imread('ofo.jpg');
[l,m] =  bwlabel (a,8);
status = regionprops (a,'BoundingBox');
figure(10);
imshow (img);
hold on;
for j = 1:m
    rectangle ('position', status(j).BoundingBox, 'edgecolor', 'r');
end
hold off;

for k = 1:m
    if status(k).BoundingBox(1) < 100
        continue
    elseif status(k).BoundingBox(1) >400
        continue
    elseif status(k).BoundingBox(2) <100
        continue
    elseif status(k).BoundingBox(2) >400
        continue
    elseif status(k).BoundingBox(3) <64
        continue
    elseif status(k).BoundingBox(3) >128
        continue
    elseif status(k).BoundingBox(4) <64
        continue
    elseif status(k).BoundingBox(4) >128
        continue
    elseif (status(k).BoundingBox(4)/status(k).BoundingBox(3) > 1.2)
        continue
    elseif (status(k).BoundingBox(4)/status(k).BoundingBox(3) < 0.8)
        continue
    else
        i2 = imcrop (img,status(k).BoundingBox);
        imwrite (i2,['ofo','_',int2str(k),'.bmp']);
    end
end

b = i2;
b = im2bw (b);
imshow (b)
imwrite (b,['ofo_QR.bmp']);

%查找旋转点
[x,y] = size(b);

%在第一行中找到第一个白点
for i=1:x
    for j=1:y
%         if(b(x,y)==1)
%             i
%             j
%         end
        b(x,y)
    end
end
    

% se = strel ('rectangle', [7 47]);
% a = imclose (a,se);
% 
% a = imopen (a,se);
% imshow (a)
