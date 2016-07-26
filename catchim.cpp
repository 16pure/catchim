#include "opencv2/opencv.hpp"
#include <iostream>

#define MAX_FRAME 1000

int main()
{
  cv::VideoCapture cap(0);//デバイスのオープン
  //cap.set(CV_CAP_PROP_FRAME_WIDTH,1920);
  //cap.set(CV_CAP_PROP_FRAME_HEIGHT,1080);
	

  if(!cap.isOpened())//カメラデバイスが正常にオープンしたか確認．
  {
    //読み込みに失敗したときの処理
    return -1;
  }

  while(1)//無限ループ
  {
    cv::Mat frame,grayImg1;
    cap >> frame; // get a new frame from camera
    //
    //取得したフレーム画像に対して，クレースケール変換や2値化などの処理を書き込む．
    //
    for (int i=0;i<frame.rows;i++){
      for (int j=0;j<frame.cols;j++){
        //frame.data[i*frame.step+j*3+1]=255;
      }
    }
    //cv::cvtColor(frame, grayImg1, CV_BGR2GRAY);

    cv::imshow("window", frame);//画像を表示．

    int key = cv::waitKey(1);
    if(key == 113)//qボタンが押されたとき
    {
      break;//whileループから抜ける．
    }
    else if(key == 115)
    {
     for(int numFrame=0; numFrame < MAX_FRAME; numFrame++)
	{	
		char im[100];
		cap >> frame;
		sprintf( im,"/media/itolab/disk2/work/sasai/cv/opencv-test/catchi/%06d.png",numFrame);
		imwrite(im,frame);
		if(key == 113)//qボタンが押されたとき
 		   {
      			break;//whileループから抜ける．
    		   }
	}
		printf("stop\n");

    }
    }
  cv::destroyAllWindows();
  return 0;
}
