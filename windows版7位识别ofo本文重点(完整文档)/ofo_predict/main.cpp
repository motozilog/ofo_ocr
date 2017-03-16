#include <QCoreApplication>
#include <iostream>
#include <fstream>
#include <direct.h>
#include <opencv2/opencv.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>
#include <opencv2/ml/ml.hpp>
#include "time.h"
using namespace std;
using namespace cv;
using namespace cv::ml;
void processImg()
{
    //QCoreApplicationa(argc,argv);
    //returna.exec();

     ifstream trainList;
     trainList.open("predictList.txt");
     string path = "./predict/";
     string filename = "261641.jpg";

     ofstream trainSetList;
     trainSetList.open("trainSetList.txt");

     ofstream trainFeature2;
     trainFeature2.open("trainFeature2.txt");

     ofstream trainFeature2Label;
     trainFeature2Label.open("trainFeature2Label.txt");

     while (!trainList.eof())
     {
         getline(trainList,filename);
         cout<<path+filename<<endl;

     cv::Mat lastImg;

     //s1-00:imread
     cv::Mat srcImg=cv::imread(path+filename);  //绝对路径。路径用/而不用\，否则会报错
     srcImg.copyTo(lastImg);
 //    cv::namedWindow("s1-00");
 //    cv::imshow("s1-00",srcImg);

     //s1-01:resize
     cv::Mat resizeImg;
     cv::resize(lastImg,resizeImg,Size(800,800));
     resizeImg.copyTo(lastImg);
 //    cv::namedWindow("s1-01");
 //    cv::imshow("s1-01",resizeImg);

     //s1-02:rotate
     cv::Mat rotateImg;
     //先设置中心点
     cv::Mat M = cv::getRotationMatrix2D(Point2f(lastImg.cols/2,lastImg.rows/2),270,1);
     //再用warpAffine旋转
     cv::warpAffine(lastImg,rotateImg,M,rotateImg.size());
     rotateImg.copyTo(lastImg);
 //    cv::namedWindow("s1-02");
 //    cv::imshow("s1-02",rotateImg);
     //输出
     mkdir("s1-02rotate");
     cv::imwrite("./s1-02rotate/"+filename,rotateImg);

     //s1-03:rgb2gray
     Mat grayImg;
 //    cv::cvtColor(lastImg,grayImg,COLOR_RGB2GRAY);
     cv::GaussianBlur(lastImg,grayImg,Size(11,11),0,0);  //高斯
 //    cv::cvtColor(lastImg,grayImg,COLOR_RGB2HLS);
 //    cv::inRange(grayImg,Scalar(90,32,0),Scalar(100,128,128),grayImg);
     cv::cvtColor(grayImg,grayImg,COLOR_RGB2GRAY);
     grayImg.copyTo(lastImg);
 //    cv::namedWindow("s1-03");
 //    cv::imshow("s1-03",grayImg);
     //输出
     mkdir("s1-03gray");
     cv::imwrite("./s1-03gray/"+filename,grayImg);

     //s1-04:bw
     Mat bwImg;
 //    cv::threshold(lastImg,bwImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
 //    cv::Sobel(lastImg,bwImg,-1,1,1,5);
     cv::Laplacian(lastImg,bwImg,-1,3); //边缘化
     cv::threshold(bwImg,bwImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
     cv::fastNlMeansDenoising(bwImg,bwImg,30,15,31); //去燥


     bwImg.copyTo(lastImg);
 //    cv::namedWindow("s1-04");
 //    cv::imshow("s1-04",bwImg);
     //输出
     mkdir("s1-04bw");
     cv::imwrite("./s1-04bw/"+filename,bwImg);

     //s1-05:第一次膨胀，去掉数字中的燥点
     Mat dilateImg1;
     Mat element1 = cv::getStructuringElement(MORPH_RECT,Size(3,3));
     cv::dilate(lastImg,dilateImg1,element1);
     cv::erode(dilateImg1,dilateImg1,element1);
     dilateImg1.copyTo(lastImg);
 //    cv::namedWindow("s1-05");
 //    cv::imshow("s1-05",dilateImg1);
     //输出
     mkdir("s1-05dilateImg1");
     cv::imwrite("./s1-05dilateImg1/"+filename,dilateImg1);

     //s1-06:绘制外接矩形，以寻找QR(7位)
     //cv::MemStorage *pStorage = NULL;
     Mat rectQRFindImg;
     int QRFinded = 0;
     int QRx,QRy,QRwidth,QRheight;
     cv::Point2f QRFindRotatePoint[4];//用于旋转
     lastImg.copyTo(rectQRFindImg);
     cv::threshold(rectQRFindImg,rectQRFindImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);

     vector< vector< Point> > contoursQRFind;
     cv::findContours(rectQRFindImg,contoursQRFind,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE);
 //    cv::drawContours(rectQRFindImg,contoursQRFind,-1,Scalar(0,0,255),1);

     for (int i=0;i<contoursQRFind.size();i++)
     {
         //绘制最小外接矩形
         cv::Rect rectQRFind=cv::boundingRect(contoursQRFind[i]);
 //        cout<<rectQRFind.width<<"*"<<rectQRFind.height<<" x:"<<rectQRFind.x<<" y:"<<rectQRFind.y<<endl;


         //判断是否为QR
         if(
                 ((float(rectQRFind.width)/rectQRFind.height)>0.7)&&
                 ((float(rectQRFind.width)/rectQRFind.height)<1.3)&&
                 ((rectQRFind.width)>200)&&
                 ((rectQRFind.width)<400)&&
                 ((rectQRFind.x>10))&&
                 ((rectQRFind.x<200))&&
                 ((rectQRFind.y>50))&&
                 ((rectQRFind.y<400))&&
                 ((contoursQRFind[i].size())>800)

                 )

         {
 //            cv::rectangle(rectQRFindImg,rect,Scalar(0,0,0),CV_FILLED);
             cv::rectangle(rectQRFindImg,rectQRFind,Scalar(255,255,255));
             cout<<"draw"<<rectQRFind.width<<"*"<<rectQRFind.height<<" x:"<<rectQRFind.x<<" y:"<<rectQRFind.y<<" c:"<<contoursQRFind[i].size()<<endl;
             QRx=rectQRFind.x;
             QRy=rectQRFind.y;
             QRwidth=rectQRFind.width;
             QRheight=rectQRFind.height;
             QRFinded++;
             //旋转
             cv::RotatedRect rectQRFindRotate=cv::minAreaRect(contoursQRFind[i]);
             rectQRFindRotate.points(QRFindRotatePoint);
             for (int j=0; j<=3; j++)
             {
                 line(rectQRFindImg,QRFindRotatePoint[j],QRFindRotatePoint[(j+1)%4],Scalar(255,255,255),10);
 //                cout<<"rotate:"<<QRFindRotatePoint[j]<<endl;
             }
         }

     }
     lastImg = rectQRFindImg;
 //    cv::namedWindow("s1-06");
 //    cv::imshow("s1-06",rectQRFindImg);

     mkdir("s1-06rectQRFindImg");
     cv::imwrite("./s1-06rectQRFindImg/"+filename,rectQRFindImg);

     if(QRFinded!=1)
     {
         cout<<"ERROR: Can't Find correct QR"<<endl;
         continue;
     }

     //s1-07 cropQR(方便排错)
 //    if(QRFinded==1)
 //    {
 //        Mat cropQRImg;
 //        bwImg.copyTo(cropQRImg);
 //        // Setup a rectangle to define your region of interest
 //        //Rect参数：左上角x坐标, 左上角y坐标, 宽度, 高度
 //        cv::Rect maskQRRect(QRx, QRy, QRwidth, QRheight);
 //        // Crop the full image to that image contained by the rectangle myROI
 //        // Note that this doesn't copy the data
 //        cv::Mat croppedQRImg(cropQRImg, maskQRRect);

 //        cv::namedWindow("s1-07");
 //        cv::imshow("s1-07",croppedQRImg);
 //        mkdir("s1-07croppedQRImg");
 //        cv::imwrite("./s1-07croppedQRImg/"+filename,croppedQRImg);
 //    }


     //s1-08利用QR旋转
     cv::Mat rotateOfoImg;
     rotateImg.copyTo(rotateOfoImg); //不知为什么用lastImg会调用rotateImg
     double angelOfo;
     //需要补一个寻找左上角和右上角点。想到的思路是x+y最小的为左上角，而接下来的是右上角
     int findLeft,findLeftXY=800*800;
     for (int j=0; j<=3; j++)
     {
         if ((QRFindRotatePoint[j].x+QRFindRotatePoint[j].y)<findLeftXY)
         {
             findLeftXY=QRFindRotatePoint[j].x+QRFindRotatePoint[j].y;
             findLeft=j;
         }

     }

     // 用arctan来求旋转角度，公式(y/x)*180/3.14
     angelOfo = atan ((QRFindRotatePoint[findLeft+1].y-QRFindRotatePoint[findLeft].y)/(QRFindRotatePoint[findLeft+1].x-QRFindRotatePoint[findLeft].x))*180/3.14;
     //先设置中心点
     cv::Mat M2 = cv::getRotationMatrix2D(Point2f(rotateOfoImg.cols/2,rotateOfoImg.rows/2),angelOfo,1);
     //再用warpAffine旋转
     //参数 源，目标，角度，尺寸
     cv::warpAffine(rotateOfoImg,rotateOfoImg,M2,rotateOfoImg.size());
 //        cv::namedWindow("s1-08");
 //        cv::imshow("s1-08",rotateOfoImg);
     rotateOfoImg.copyTo(lastImg);
     mkdir("s1-08rotateOfoImg");
     cv::imwrite("./s1-08rotateOfoImg/"+filename,rotateOfoImg);

     //s1-09 粗略将车牌连QR一起crop出来
     Mat cropOfo7CharImg;
     rotateOfoImg.copyTo(cropOfo7CharImg);


     //二点间距离公式
     // sqrt(pow((x2-x1),2)+pow((y2-y1),2))
     float distanceQRx= sqrt(pow((QRFindRotatePoint[findLeft+1].x-QRFindRotatePoint[findLeft].x),2) +pow((QRFindRotatePoint[findLeft+1].y-QRFindRotatePoint[findLeft].y),2));
     float distanceQRy= sqrt(pow((QRFindRotatePoint[findLeft].x-QRFindRotatePoint[findLeft-1].x),2) +pow((QRFindRotatePoint[findLeft].y-QRFindRotatePoint[findLeft-1].y),2));
     cout<<"distanceX: "<<distanceQRx<<" Y: "<<distanceQRy<<endl;


     int maskOfo7CharX=distanceQRx*1.4;
     if((maskOfo7CharX+QRx+distanceQRx)>800)
     {
         maskOfo7CharX=800-(QRx+distanceQRx);
     }

     cv::Rect maskOfo7Char(QRx+distanceQRx, QRy+(distanceQRy*0.6), maskOfo7CharX, distanceQRy*0.5);
     cv::Mat croppedOfo7CharImg(cropOfo7CharImg, maskOfo7Char);

 //        cv::namedWindow("s1-09");
 //        cv::imshow("s1-09",croppedOfo7CharImg);
     lastImg.empty();
     cropOfo7CharImg.copyTo(lastImg);

     mkdir("s1-09croppedOfo7CharImg");
     cv::imwrite("./s1-09croppedOfo7CharImg/"+filename,croppedOfo7CharImg);

     //s2-01 用小矩阵，再次进行中值滤波、灰度、二值化、膨胀
     Mat ofo7CharReProcessImg;
     int Ofo7CharFind=0;
     int Ofo7CharX,Ofo7CharY,Ofo7CharWidth,Ofo7CharHeight;
     croppedOfo7CharImg.copyTo(ofo7CharReProcessImg);
     mkdir("s2-01ofo7CharReProcessImg");

     cv::medianBlur(ofo7CharReProcessImg,ofo7CharReProcessImg,11);//中值滤波
     cv::cvtColor(ofo7CharReProcessImg,ofo7CharReProcessImg,COLOR_BGR2GRAY);
     cv::Canny(ofo7CharReProcessImg,ofo7CharReProcessImg,50,150,3);//取边缘
     cv::imwrite("./s2-01ofo7CharReProcessImg/z02_Canny_"+filename,ofo7CharReProcessImg);
     Mat elementOfo7CharReProcess = cv::getStructuringElement(MORPH_RECT,Size(25,1));//设置膨胀参数
     cv::dilate(ofo7CharReProcessImg,ofo7CharReProcessImg,elementOfo7CharReProcess); //膨胀
     cv::imwrite("./s2-01ofo7CharReProcessImg/z03_dilate_"+filename,ofo7CharReProcessImg);

     cv::threshold(ofo7CharReProcessImg,ofo7CharReProcessImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY); //再次二值化，为绘制矩形做准备
     vector< vector< Point> > contoursOfo7CharFind; //存放指针
     cv::findContours(ofo7CharReProcessImg,contoursOfo7CharFind,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE); //找轮廓
     for (int i=0;i<contoursOfo7CharFind.size();i++)
     {
         //绘制最小外接矩形
         cv::Rect rectOfo7CharFind=cv::boundingRect(contoursOfo7CharFind[i]);
         cout<<"find7Char: "<< rectOfo7CharFind.width <<"*"<<rectOfo7CharFind.height<<" c:"<<contoursOfo7CharFind[i].size()<<endl;
         //判断是否为7个字符
         if(
                 (((float(rectOfo7CharFind.width))/rectOfo7CharFind.height)>3.2)&&
                 (((float(rectOfo7CharFind.width))/rectOfo7CharFind.height)<4.8)&&
                 ((rectOfo7CharFind.width>distanceQRx)||(rectOfo7CharFind.width>distanceQRy))

                 )
         {
             cv::rectangle(ofo7CharReProcessImg,rectOfo7CharFind,Scalar(255,255,255));
             cout<<"Draw7Char: "<< rectOfo7CharFind.width <<"*"<<rectOfo7CharFind.height<<endl;
             Ofo7CharX=rectOfo7CharFind.x;
             Ofo7CharY=rectOfo7CharFind.y;
             Ofo7CharWidth=rectOfo7CharFind.width;
             Ofo7CharHeight=rectOfo7CharFind.height;
             Ofo7CharFind++;
         }
     }
     if(Ofo7CharFind!=1)
     {
         cout<<"ERROR:Can't Find OFO 7 Char position"<<endl;
         continue;
     }
 //    cv::namedWindow("s2-01");
 //    cv::imshow("s2-01",ofo7CharReProcessImg);

     cv::imwrite("./s2-01ofo7CharReProcessImg/"+filename,ofo7CharReProcessImg);

     //s2-02 crop出来
     Mat cropReProcessImg;
     croppedOfo7CharImg.copyTo(cropReProcessImg);
     cv::Rect maskOfo7CharReProcessImg(Ofo7CharX, Ofo7CharY, Ofo7CharWidth,Ofo7CharHeight);
     cv::Mat croppedOfo7CharReProcessImg(cropReProcessImg, maskOfo7CharReProcessImg);
 //    cv::namedWindow("s2-02");
 //    cv::imshow("s2-02",croppedOfo7CharReProcessImg);
     mkdir("s2-02croppedOfo7CharReProcessImg");
     cv::imwrite("./s2-02croppedOfo7CharReProcessImg/"+filename,croppedOfo7CharReProcessImg);

     //s2-03 将7个字找出来
     Mat char7findImg;
     Mat char7findImgBw;
     //charList结构：1标记,x,y,w,h
     int charList[7][5]={};
     int charListCount=0;
     croppedOfo7CharReProcessImg.copyTo(char7findImg);
     cout<<char7findImg.cols<<"*"<<char7findImg.rows<<endl;
     cv::medianBlur(char7findImg,char7findImg,5);//中值滤波
 //    cv::equalizeHist(char7findImg,char7findImg);

     //尝试用黄色来分割
 //    cvtColor(char7findImg,char7findImg,COLOR_BGR2HSV);
 //    vector<Mat> channels;
 //    split(char7findImg,channels);
 //    cv::namedWindow("s2-03H");
 //    cv::imshow("s2-03H",channels[0]);
 //    cv::namedWindow("s2-03S");
 //    cv::imshow("s2-03S",channels[1]);
 //    cv::namedWindow("s2-03V");
 //    cv::imshow("s2-03V",channels[2]);
 //    cv::inRange(char7findImg,Scalar(16,100,64),Scalar(64,255,255),char7findImg);
 //    cv::namedWindow("s2-031");
 //    cv::imshow("s2-031",char7findImg);

     cv::cvtColor(char7findImg,char7findImg,COLOR_BGR2GRAY);
     cv::threshold(char7findImg,char7findImg,0,255,CV_THRESH_OTSU+CV_THRESH_BINARY); //再次二值化，为绘制矩形做准备
     char7findImg.copyTo(char7findImgBw);



     vector< vector< Point> > contourschar7findImg; //存放指针
     cv::findContours(char7findImg,contourschar7findImg,CV_RETR_EXTERNAL,CV_CHAIN_APPROX_NONE); //找轮廓
     for (int i=0;i<contourschar7findImg.size();i++)
     {
         //绘制最小外接矩形
         cv::Rect rectRotateReProcess=cv::boundingRect(contourschar7findImg[i]);
 //        cv::rectangle(char7findImg,rectRotateReProcess,Scalar(255,255,255));
         cout<<"Char:"<<rectRotateReProcess.width<<"*"<<rectRotateReProcess.height<<" x:"<<rectRotateReProcess.x<<" y:"<<rectRotateReProcess.y<<endl;
         if (
                 (((float(rectRotateReProcess.height))/rectRotateReProcess.width)>1.5)&&
                 (((float(rectRotateReProcess.height))/rectRotateReProcess.width)<4)&&
                 (rectRotateReProcess.height>((char7findImg.rows)*0.67))
                 )
         {
             cv::rectangle(char7findImg,rectRotateReProcess,Scalar(255,255,255));


             charList[charListCount][1]=rectRotateReProcess.x;
             charList[charListCount][2]=rectRotateReProcess.y;
             charList[charListCount][3]=rectRotateReProcess.width;
             charList[charListCount][4]=rectRotateReProcess.height;
             charListCount++;
         }

     }
     if(charListCount!=7)
     {
         cout<<"ERROR: Can't find ALL 7 char"<<endl;
         continue;
     }
 //    cv::namedWindow("s2-03");
 //    cv::imshow("s2-03",char7findImg);
     mkdir("s2-03char7findImg");
     cv::imwrite("./s2-03char7findImg/"+filename,char7findImg);

     //s2-04排序
     //冒泡排序
     int charSortTemp[5];
     for (int i=0;i<7;i++)
     {
         for (int j=i+1;j<7;j++)
         {
             if(charList[i][1]>charList[j][1])
             {
                 for(int k=0;k<5;k++)
                 {
                     charSortTemp[k]=charList[i][k];
                     charList[i][k]=charList[j][k];
                     charList[j][k]=charSortTemp[k];

                 }
             }
         }
     }

     //判断是否为1
 //    for (int i=0;i<7;i++)
 //    {
 //        cout<<"x:"<<charList[i][1]<<" y:"<<charList[i][2]<<" "<<charList[i][3]<<"*"<<charList[i][4]<<"ratio:"<<float(charList[i][4])/charList[i][3]<<endl;
 //        if((float(charList[i][4])/charList[i][3])>2.8)
 //        {
 //            charList[i][0]=1;
 //        }
 //    }

     //s2-05 crop出来，并且resize 32*72
     Mat croppedCharListImg[7];
     Mat croppedCharTempImg;

     char7findImgBw.copyTo(croppedCharTempImg);
     mkdir("s2-05croppedChar");
     for (int i=0;i<7;i++)
     {
         cout<<filename[i]<<endl;
         cv::Rect maskCharList(charList[i][1],charList[i][2],charList[i][3],charList[i][4]);
         cv::Mat croppedChar(croppedCharTempImg, maskCharList);
         croppedChar.copyTo(croppedCharListImg[i]);
         cv::resize(croppedCharListImg[i],croppedCharListImg[i],Size(32,72),0,0,INTER_CUBIC);
         cv::imwrite("./s2-05croppedChar/"+filename+"_"+ to_string(i)+"__"+filename[i]+".jpg",croppedCharListImg[i]);
 //        trainSetList<<filename[i]<<"\t"<<"./s2-05croppedChar/"+filename+"_"+ to_string(i)+"__"+filename[i]+".jpg"<<endl;
         trainSetList<<"./s2-05croppedChar/"+filename+"_"+ to_string(i)+"__"+filename[i]+".jpg"<<endl;
     }

     //s2-06 将Mat矩阵转换成二维矩阵，因为Mat矩阵只能用一维指针来操作，太麻烦了
     int imgArray[7][72][32];
     for(int i=0;i<7;i++)
     {
         //先二值化
         cv::threshold(croppedCharListImg[i],croppedCharListImg[i],0,255,CV_THRESH_OTSU+CV_THRESH_BINARY);
         croppedCharListImg[i].convertTo(croppedCharListImg[i],CV_8U);
         cv::MatIterator_<cv::Vec3b> it = croppedCharListImg[i].begin<cv::Vec3b>();
         int t=0;
         for(;it!=croppedCharListImg[i].end<cv::Vec3b>();it++)
         {
             if((((*it)[0])==255)&&(((*it)[1])==255)&&(((*it)[2])==255))
             {
                 imgArray[i][t/32][t%32]=1;
             } else {
                 imgArray[i][t/32][t%32]=0;
             }
             t++;
         }

         //输出，以便于观察
 //        for(int j=0;j<72;j++)
 //        {
 //            for(int k=0;k<32;k++)
 //            {
 //                if(imgArray[i][j][k]==1)
 //                {
 //                    cout<<imgArray[i][j][k];
 //                } else {
 //                    cout<<" ";
 //                }
 //            }
 //            cout<<""<<endl;
 //        }
     }

     //s2-07 计算feature2
     int feature2Extract[7][32/8*72/8];
     for (int i=0;i<7;i++)
     {
         cv::MatIterator_<cv::Vec3b> it = croppedCharListImg[i].begin<cv::Vec3b>();
         int arrayNum=0;
         for(int j=0;j<72;j=j+8)
         {
             for (int k=0;k<32;k=k+8)
             {
                 int countBW=0;
                 for (int lj=0;lj<8;lj++)
                 {
                     for(int lk=0;lk<8;lk++)
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
     //输出至trainFeature2
     for (int i=0;i<7;i++)
     {
         trainFeature2Label<<filename[i]<<endl;
         for (int j=0;j<32/8*72/8;j++)
         {
             if (j!=(32/8*72/8)-1)
             {
                 trainFeature2<<feature2Extract[i][j]<<" ";
             } else {
                 trainFeature2<<feature2Extract[i][j]<<endl;
             }
         }
     }



     //end of list
     }
     trainSetList.close();
     trainFeature2Label.close();
     trainFeature2.close();
}

void trainSvm();
int main(void)
{
//    processImg();

    trainSvm();
    cv::waitKey(0);
    cv::destroyAllWindows();
    return 0;
}

void trainSvm()
{
    //s01 获取训练行数
    ifstream trainFeature2;
    trainFeature2.open("trainFeature2.txt");
    ifstream trainFeature2Label;
    trainFeature2Label.open("trainFeature2Label.txt");
    ofstream predictData;
    predictData.open("predictData.txt");

    string line;
    int countLines = 0;

    while (getline(trainFeature2,line))
    {
        ++countLines;
    }
//    countLines--; //去掉尾行

    cout<<"countLines:"<<countLines<<endl;
    trainFeature2.close();

    //s02将feature加载到Mat中
    int labels[countLines];
    for (int i=0;i<countLines;i++)
    {
        trainFeature2Label>>labels[i];
    }


    trainFeature2.open("trainFeature2.txt"); //因为之前统计行时close了一次
    float trainingData[countLines][(32/8*72/8)];
    for (int i=0;i<countLines;i++)
    {
        line.empty();
        getline(trainFeature2,line);
        int j=0;

        istringstream iss(line);
        do
        {
            string sub;
            iss >> sub;
            if (sub!= "")
            {
//                cout << sub <<endl;
                trainingData[i][j]=stoi(sub);
                j++;
            }
        } while(iss);
    }

    //校对
    for (int i=0;i<countLines;i++)
    {
        cout<<labels[i]<<"\t";
        for(int j=0;j<32/8*72/8;j++)
        {
            cout<<trainingData[i][j]<<" ";
        }
        cout<<endl;
    }

    //转成Mat(预测只能逐行逐行来)
    for (int i=0;i<countLines;i++)
    {

    Mat trainningDataMat(1,32/8*72/8,CV_32FC1,trainingData[i]);


    //预测SVM
    cv::Ptr<cv::ml::SVM> mSvm2;
    mSvm2 = cv::ml::SVM::load<cv::ml::SVM>("svmtrain.txt");
    float q = mSvm2->predict(trainningDataMat);
    cout<<"predict: "<<q<<"\treal:"<<labels[i]<<endl;
    predictData<<"predict: "<<q<<"\treal:"<<labels[i]<<endl;
    }

}

