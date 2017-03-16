#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
using namespace std;
using namespace cv;
int main(void)
{
   //QCoreApplicationa(argc,argv);
   //returna.exec();
   cout<<"opencv"<<endl;
   cv::namedWindow("test");
   cv::Mat lastImg;

   //s1-00:imread
   cv::Mat srcImg=cv::imread("d:/ofo_ocr/IMG_20170204_114256_cr.jpg");  //绝对路径。路径用/而不用\，否则会报错
//   cv::imshow("test",srcImg);
   lastImg = srcImg;

   //s1-01:resize
   cv::Mat resizeImg;
   cv::resize(lastImg,resizeImg,Size(640,640));
//   cv::imshow("test",resizeImg);
   lastImg = resizeImg;

   //s1-02:rotate
   cv::Mat rotateImg;
   //先设置中心点
   cv::Mat M = cv::getRotationMatrix2D(Point2f(lastImg.cols/2,lastImg.rows/2),270,1);
   //再用warpAffine旋转
   cv::warpAffine(lastImg,rotateImg,M,rotateImg.size());
//   cv::imshow("test",rotateImg);
//   cv::inRange(rotateImg,Scalar(93,81,0),Scalar(255,255,0),rotateImg);
   cv::imwrite("d:/ofo_ocr/s1-02_rotateImg.jpg",rotateImg);
   lastImg = rotateImg;

   //s1-03:rgb2gray
   Mat grayImg;
   cv::cvtColor(lastImg,grayImg,COLOR_RGB2GRAY);
//   cv::imshow("test",grayImg);
   cv::imwrite("d:/ofo_ocr/s1-03_grayImg.jpg",grayImg);
   lastImg = grayImg;

   //s1-04:im2bw
   Mat bwImg;
   cv::threshold(lastImg,bwImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
   lastImg = bwImg;
   cv::imwrite("d:/ofo_ocr/s1-04_bwImg.jpg",bwImg);
   cv::imshow("test",bwImg);

//   //s05:sobel
//   Mat grad_x, grad_y;
//   Mat abs_grad_x, abs_grad_y;
//   Mat sobelImg;
//   cv::Sobel(lastImg,grad_x,lastImg.depth(),1,0);
//   cv::Sobel(lastImg,grad_y,lastImg.depth(),0,1);
//   cv::convertScaleAbs(grad_x,abs_grad_x);
//   cv::convertScaleAbs(grad_y,abs_grad_y);
//   cv::addWeighted(abs_grad_x,0.5,abs_grad_y,0.5,0,sobelImg);
//   lastImg = sobelImg;
//   cv::imwrite("d:/ofo_ocr/sobelImg.jpg",sobelImg);
//   cv::imshow("test",sobelImg);


   //s1-05:第一次膨胀，去掉数字中的燥点
   Mat dilateImg1;
   Mat element1 = cv::getStructuringElement(MORPH_RECT,Size(3,3));
   cv::dilate(lastImg,dilateImg1,element1);
   lastImg = dilateImg1;
   cv::imshow("test",dilateImg1);
   cv::imwrite("d:/ofo_ocr/s1-05_dilateImg1.jpg",dilateImg1);


   //s1-06:findContours
   //cv::MemStorage *pStorage = NULL;
   Mat rectImg;
   rectImg = lastImg;
   vector< vector< Point> > contours;
   cv::findContours(lastImg,contours,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
   cv::drawContours(rectImg,contours,-1,Scalar(0,0,255),1);

   for (int i=0;i<contours.size();i++)
   {
       //绘制最小外接矩形
       cv::RotatedRect rect=cv::minAreaRect(contours[i]);
       cv::Point2f P[4];
       rect.points(P);
       if ((rect.size.width/rect.size.height >0.3)&&(rect.size.width/rect.size.height <0.8)&&(rect.size.width>24)&&(rect.size.width<144))
       {
//       cout<<rect.size.width<<endl;
       for (int j=0; j<=3; j++)
       {
           line(rectImg,P[j],P[(j+1)%4],Scalar(255,0,0),1);


       }
       }

   }
   lastImg = rectImg;
   cv::imshow("test",rectImg);
   cv::imwrite("d:/ofo_ocr/s1-06_rectImg.jpg",rectImg);

   //s1-07:第二次膨胀，将数字连起来
   Mat dilateImg2;
   Mat element2 = cv::getStructuringElement(MORPH_RECT,Size(23,1));
   cv::dilate(lastImg,dilateImg2,element2);
   lastImg = dilateImg2;
   cv::imshow("test",lastImg);
   cv::imwrite("d:/ofo_ocr/s1-08_dilateImg2.jpg",dilateImg2);

   //s1-08:findContours，第二次取最小外接矩形，将OFO的6个数字取出来
   Mat rectImg2;
   rotateImg.copyTo(rectImg2);
   //要再次二值化，否则会有奇怪问题
   cv::threshold(lastImg,lastImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
   vector< vector< Point> > contours2;
   cv::findContours(lastImg,contours2,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
   cout<<contours2.size()<<endl;
//   cv::drawContours(rectImg2,contours2,-1,Scalar(0,0,255),1);

   cv::Point2f P[4];
   for (int i=0;i<contours2.size();i++)
   {
       //绘制最小外接矩形
       cv::RotatedRect rect=cv::minAreaRect(contours2[i]);
       rect.points(P);
       if ((rect.size.width/rect.size.height >3.8)&&(rect.size.width/rect.size.height <5.2)&&(rect.size.width>256)&&(rect.size.width<600))
       {
           cout<<rect.size.width<<endl;
           cout<<rect.size.height<<endl;
           for (int j=0; j<=3; j++)
           {
               line(rectImg2,P[j],P[(j+1)%4],Scalar(255,0,0),1);
           }
       }
   }
   lastImg = rectImg2;
   cv::imshow("test",lastImg);
   cv::imwrite("d:/ofo_ocr/s1-08_rectImg2.jpg",rectImg2);

   //s1-08-2:根据上面的矩形，将它crop出来
   int minX=640,maxX=0,minY=640,maxY=0;

   for (int i=0;i<=3;i++)
   {
       if((P[i].x)<minX)
       {
           minX=P[i].x;
       }
       if((P[i].x)>maxX)
       {
           maxX=P[i].x;
       }
       if((P[i].y)<minY)
       {
           minY=P[i].y;
       }
       if((P[i].y)>maxY)
       {
           maxY=P[i].y;
       }

   }
   cout<<"minX:"<<minX<<endl;
   cout<<"minY:"<<minY<<endl;
   cout<<"maxX:"<<maxX<<endl;
   cout<<"maxY:"<<maxY<<endl;


   cv::Mat cropSource = rotateImg;
   // Setup a rectangle to define your region of interest
   //Rect参数：左上角x坐标, 左上角y坐标, 宽度, 高度
   cv::Rect maskRect(minX, minY, maxX-minX, maxY-minY);
   // Crop the full image to that image contained by the rectangle myROI
   // Note that this doesn't copy the data
   cv::Mat croppedRef(cropSource, maskRect);

   cv::Mat croppedImg;
   // Copy the data into new matrix
   croppedRef.copyTo(croppedImg);

   lastImg=croppedImg;
   cv::imshow("test",croppedImg);
   cv::imwrite("d:/ofo_ocr/s1-08-2_croppedImg.jpg",croppedImg);

   //s2-01:rotate
   cv::Mat rotateOfoImg;
   //计算角度
   double angel;
   double x,y;
   x = P[2].x - P[1].x;
   y = P[2].y - P[1].y;
   cout<<"x:"<<x<<" "<<"y:"<<y<<endl;
   angel = atan (y/x)*180/3.14;
   cout<<angel<<endl;


   //先设置中心点
   cv::Mat M2 = cv::getRotationMatrix2D(Point2f(lastImg.cols/2,lastImg.rows/2),angel,1);
   //再用warpAffine旋转
   cv::warpAffine(lastImg,rotateOfoImg,M2,lastImg.size());
   cv::imshow("test",rotateOfoImg);
   cv::imwrite("d:/ofo_ocr/s2-01_rotateOfoImg.jpg",rotateOfoImg);
   lastImg = rotateOfoImg;

   //s2-02 rgb2gray,im2bw
   cv::Mat bwOfoImg;
   cv::cvtColor(lastImg,bwOfoImg,COLOR_RGB2GRAY);
   cv::threshold(bwOfoImg,bwOfoImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
   cv::imshow("test",bwOfoImg);
   cv::imwrite("d:/ofo_ocr/s2-02_bwOfoImg.jpg",bwOfoImg);
   lastImg = bwOfoImg;

   //s2-03 中值滤波
   cv::Mat medianBlurImg;
   cv::medianBlur(lastImg,medianBlurImg,9);
   cv::imshow("test",medianBlurImg);
   cv::imwrite("d:/ofo_ocr/s2-03_medianBlurImg.jpg",medianBlurImg);
   lastImg = medianBlurImg;

   //s2-04:findContours
   cv::Mat rectOfoImg;
   cv::Mat charOfoImg[6];
   float ofoX[6][5]; //ID、左上角x、左上角y、width、height
   lastImg.copyTo(rectOfoImg);
   cv::threshold(rectOfoImg,rectOfoImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
   vector< vector< Point> > contoursOfo;
   cv::findContours(rectOfoImg,contoursOfo,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);

   for (int i=0;i<contoursOfo.size();i++)
   {
       //绘制最小外接矩形(不旋转)
       cv::Rect rectOfo=cv::boundingRect(contoursOfo[i]);
       if((rectOfo.width>16)&&(rectOfo.width<64)&&(rectOfo.height>32))
       {
           cv::rectangle(rectOfoImg,rectOfo,Scalar(255,255,255));
//           cout<<rectOfo.tl()<<endl;
//           cout<<rectOfo.width<<endl;
//           cout<<rectOfo.height<<endl;
           ofoX[i][0]=i;
           ofoX[i][1]=rectOfo.tl().x;
           ofoX[i][2]=rectOfo.tl().y;
           ofoX[i][3]=rectOfo.width;
           ofoX[i][4]=rectOfo.height;
       }
   }
//   cout<<ofoX<<endl;
   cv::imshow("test",rectOfoImg);
   lastImg = rectOfoImg;
   cv::imwrite("d:/ofo_ocr/s2-04_rectOfoImg.jpg",rectOfoImg);

   //s2-04-2:
   //冒泡排序
   int j,temp[5];
   for (int i=0;i<6;i++)
   {
       for(j=i+1;j<6;j++)
       {
           if(ofoX[i][1]>ofoX[j][1])
           {
               temp[1]=ofoX[i][1];
               ofoX[i][1]=ofoX[j][1];
               ofoX[j][1]=temp[1];

               temp[2]=ofoX[i][2];
               ofoX[i][2]=ofoX[j][2];
               ofoX[j][2]=temp[2];

               temp[3]=ofoX[i][3];
               ofoX[i][3]=ofoX[j][3];
               ofoX[j][3]=temp[3];

               temp[4]=ofoX[i][4];
               ofoX[i][4]=ofoX[j][4];
               ofoX[j][4]=temp[4];
            }
       }
   }
   //冒泡排序后输出
   for (int i=0;i<6;i++)
   {
       cv::Mat cropSource = lastImg;

       medianBlurImg.copyTo(cropSource);

       cv::Rect maskRect(ofoX[i][1], ofoX[i][2], ofoX[i][3], ofoX[i][4]);
       cv::Mat croppedRef(cropSource, maskRect);
//       cv::Mat croppedImg;
       croppedRef.copyTo(charOfoImg[i]);

       //fileName
       stringstream charOfoFileNameTemp;
       charOfoFileNameTemp << "d:/ofo_ocr/s2-04-2_charOfoImg_";
       charOfoFileNameTemp <<i;
       charOfoFileNameTemp << ".jpg";
       cout<<charOfoFileNameTemp<<endl;
       string charOfoFileName = charOfoFileNameTemp.str();

       //write
       cv::imwrite(charOfoFileName,charOfoImg[i]);

//       cout<<ofoX[i][1]<<endl;
   }

   //s2-04-3：将1挑出来
   int c[6]={};
   for (int i=0;i<6;i++)
   {
       float yx=ofoX[i][4]/ofoX[i][3];
       if((yx>2.5)&&(yx<3.5))
       {
           c[i]=1;
       } else {
           c[i]=0;
       }
   }

   //s2-05:将不是"1"的全部resize成48*80
   for (int i=0;i<6;i++)
   {
       if (c[i]==0)
       {
           cv::resize(charOfoImg[i],charOfoImg[i],Size(48,80));
       }
       //fileName
       stringstream charOfoFileNameTemp;
       charOfoFileNameTemp << "d:/ofo_ocr/s2-05_charOfoImg_";
       charOfoFileNameTemp <<i;
       charOfoFileNameTemp << ".jpg";
       cout<<charOfoFileNameTemp<<endl;
       string charOfoFileName = charOfoFileNameTemp.str();

       //write
       cv::imwrite(charOfoFileName,charOfoImg[i]);

   }

   //s3-01:将Mat矩阵转换成二维矩阵
   int imgArray[6][80][48];


   for(int i=0;i<6;i++)
   {
       //先二值化
       cv::threshold(charOfoImg[i],charOfoImg[i],0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
       charOfoImg[i].convertTo(charOfoImg[i],CV_8U);
       if (c[i]==0)
       {
           cv::MatIterator_<cv::Vec3b> it = charOfoImg[i].begin<cv::Vec3b>();
           int t=0;
           for(;it!=charOfoImg[i].end<cv::Vec3b>();it++)
           {
               if((((*it)[0])==255)&&(((*it)[1])==255)&&(((*it)[2])==255))
               {
                   imgArray[i][t/48][t%48]=1;
               } else {
                   imgArray[i][t/48][t%48]=0;
               }
               t++;
           }

           //输出，以便于观察
           //fileName
           stringstream charOutArrayTxtTemp;
           charOutArrayTxtTemp << "d:/ofo_ocr/s3-01_outArray_";
           charOutArrayTxtTemp <<i;
           charOutArrayTxtTemp << ".txt";
           cout<<charOutArrayTxtTemp<<endl;
           string charOutArrayTxt = charOutArrayTxtTemp.str();

           ofstream outArrayTxt(charOutArrayTxt);
           for(int j=0;j<80;j++)
           {
               for(int k=0;k<48;k++)
               {
                   if(imgArray[i][j][k]==1)
                   {
                       cout<<imgArray[i][j][k];
                       outArrayTxt<<imgArray[i][j][k];
                   } else {
                       cout<<" ";
                       outArrayTxt<<" ";
                   }
               }
               cout<<""<<endl;
               outArrayTxt<<""<<endl;
           }
           outArrayTxt.close();
       }
   }
   //s3-02:按8*8提取特征
   int feature2Extract[6][48/8*80/8];
   for (int i=0;i<6;i++)
   {
       if (c[i]==0)
       {
           int arrayNum=0;
           for (int j=0;j<80;j=j+8)
           {
               for (int k=0;k<48;k=k+8)
               {
                   int countBW=0;
                   for (int lj=0;lj<8;lj++)
                   {
                       for (int lk=0;lk<8;lk++)
                       {
                           if ((imgArray[i][j+lj][k+lk])==1)
                           {
                               countBW++;
                           }

                       }
                   }
                   feature2Extract[i][arrayNum]=countBW;
                   arrayNum++;
               }
           }
       }
   }
   //输出，以便观察
   for (int i=0;i<6;i++)
   {
       if (c[i]==0)
       {
           //fileName
           stringstream charOutArrayTxtTemp;
           charOutArrayTxtTemp << "d:/ofo_ocr/s3-02_outFeature2_";
           charOutArrayTxtTemp <<i;
           charOutArrayTxtTemp << ".txt";
           cout<<charOutArrayTxtTemp<<endl;
           string charOutArrayTxt = charOutArrayTxtTemp.str();

           ofstream outArrayTxt(charOutArrayTxt);

           for (int j=0;j<48/8*80/8;j++)
           {
               outArrayTxt<<feature2Extract[i][j]<<",";
           }
           outArrayTxt.close();


       }
   }
   //至此主程序框架编写暂告一段落，接下来是优化










   cv::waitKey(10);
   cv::destroyWindow("test");
   return 0;
}
