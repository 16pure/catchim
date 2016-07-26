//---------------------------------------------------------
// 概要      : カメラキャリブレーション
// File Name : calibration.cpp
// Library   : OpenCV 2
//---------------------------------------------------------
#include <iostream>
#include <opencv2/opencv.hpp>
#include "calibration.h"
//#include "calibration_opencv2.h"
using namespace std;
using namespace cv;

//CvCapture *capture = NULL;	//	カメラキャプチャ用の構造体
VideoCapture capture(0);

//char windowNameCapture[] = "Capture"; 				//	キャプチャした画像を表示するウィンドウの名前
string windowNameCapture = "Capture"; 				//	キャプチャした画像を表示するウィンドウの名前
//char windowNameUndistortion[] = "Undistortion";		//	レンズ歪みを補正した画像を表示するウィンドウの名前
string windowNameUndistortion = "Undistortion";		//	レンズ歪みを補正した画像を表示するウィンドウの名前

//
//	行列を画面に表示する
//
//	引数:
//		disp   : 表示する数値の書式
//		matrix : 表示する行列
//
/*
void printMatrix( char *disp, CvMat *matrix ) {
	for ( int y=0; y < matrix->height; y++ ) {
		for ( int x=0; x < matrix->width; x++ ) {
			printf( disp, cvmGet( matrix, y, x ) ); 
		}
		printf( "\n" );
	}
	printf( "\n" );
}
*/

//
//	外部パラメータ行列を画面に表示する
//
//	引数:
//		rotationMatrix    :  回転行列
//		translationVector :　並進ベクトル
//
/*
void printExtrinsicMatrix( CvMat *rotationMatrix, CvMat *translationVector ) {
	for ( int i = 0; i<3; i++ ) {
		printf(
			"%lf %lf %lf %lf\n",
			cvmGet( rotationMatrix, i, 0 ),
			cvmGet( rotationMatrix, i, 1 ),
			cvmGet( rotationMatrix, i, 2 ),
			cvmGet( translationVector, 0, i )
			);
	}
}
*/

//
//	cvFindChessboardCorners用のフラグを生成する
//
int createFindChessboardCornersFlag() {
	int flag = 0;

	if ( ADAPTIVE_THRESH != 0 ) {
		flag = flag | CV_CALIB_CB_ADAPTIVE_THRESH;
	}
	if ( NORMALIZE_IMAGE != 0 ) {
		flag = flag | CV_CALIB_CB_NORMALIZE_IMAGE;
	}
	if ( FILTER_QUADS != 0 ) {
		flag = flag | CV_CALIB_CB_FILTER_QUADS;
	}

	return flag;
}

//
//	コーナーを検出する
//
//	引数:
//      frameImage : キャプチャ画像用IplImage
//      grayImage  : グレースケール画像用IplImage
//      corners    : コーナーの位置を格納する変数
//
//	戻り値:
//		0   : コーナーがすべて検出できなかった場合
//		非0 : コーナーがすべて検出された場合
//
//int findCorners( IplImage *frameImage, IplImage *grayImage, CvPoint2D32f *corners ) {
int findCorners( Mat &frameImage, Mat &grayImage, vector<Point2f>& corners ) {
	//int cornerCount;				//	検出したコーナーの数
	int findChessboardCornersFlag;	//	cvFindChessboardCorners用フラグ
	//int findFlag;					//	コーナーがすべて検出できたかのフラグ
	bool findFlag;

	//	cvChessboardCorners用フラグを生成する
	findChessboardCornersFlag = createFindChessboardCornersFlag();

	//	コーナーを検出する
	/*
	findFlag=cvFindChessboardCorners(
	frameImage,
	cvSize( CORNER_WIDTH, CORNER_HEIGHT ),
	corners,
	&cornerCount,
	findChessboardCornersFlag
	);
	*/
	findFlag = findChessboardCorners(
		frameImage,
		Size( CORNER_WIDTH, CORNER_HEIGHT ),
		corners,
		findChessboardCornersFlag
	);

	//if( findFlag != 0 ) {
	if( findFlag ) {
		//	コーナーがすべて検出された場合
		//	検出されたコーナーの位置をサブピクセル単位にする
		//cvCvtColor( frameImage, grayImage, CV_BGR2GRAY );
		cvtColor( frameImage, grayImage, CV_BGR2GRAY );

		//CvTermCriteria criteria={ CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, MAX_ITERATIONS, EPSILON };
		TermCriteria criteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, MAX_ITERATIONS, EPSILON );
		/*
		cvFindCornerSubPix(
		grayImage,
		corners,
		cornerCount,
		cvSize( SEARCH_WINDOW_HALF_WIDTH, SEARCH_WINDOW_HALF_HEIGHT ),
		cvSize( DEAD_REGION_HALF_WIDTH, DEAD_REGION_HALF_HEIGHT ), 
		cvTermCriteria( CV_TERMCRIT_ITER | CV_TERMCRIT_EPS, MAX_ITERATIONS, EPSILON )
		);
		*/
		cornerSubPix(
			grayImage,
			corners,
			cvSize( SEARCH_WINDOW_HALF_WIDTH, SEARCH_WINDOW_HALF_HEIGHT ),
			cvSize( DEAD_REGION_HALF_WIDTH, DEAD_REGION_HALF_HEIGHT ), 
			criteria
			);

	}

	//	コーナーの位置を描く
	//cvDrawChessboardCorners( frameImage, cvSize( CORNER_WIDTH, CORNER_HEIGHT ), corners, cornerCount, findFlag );
	drawChessboardCorners( frameImage, cvSize( CORNER_WIDTH, CORNER_HEIGHT ), corners, findFlag );

	return findFlag;
}

//
//	カメラパラメータを推定する
//
//	引数:
//      intrinsicMatrix       : 内部パラメータ行列
//      distortionCoefficient : レンズ歪み係数
//
//	戻り値:
//		0 :	プログラムを終了する場合
//		1 :	チェックパターンを撮り終わった場合
//
//int estimateCameraParameters( CvMat *intrinsicMatrix, CvMat *distortionCoefficient ) {
int estimateCameraParameters( Mat &intrinsicMatrix, Mat &distortionCoefficient ) {

	int captureCount = 0;	//	キャプチャしたチェックパターンの枚数
	int returnValue = 0;	//	戻り値用変数
	int findFlag;			//	コーナーがすべて検出できたかのフラグ
	int key;				//	キー入力用の変数

	//CvPoint2D32f corners[CORNER_NUMBER];	//	コーナーの位置を格納する変数
	vector<Point2f> corners;

	//	画像を生成する
	//IplImage *frameImage = cvQueryFrame( capture );									//	キャプチャ画像用IplImage
	Mat frameImage;
	capture >> frameImage;
	//IplImage *grayImage = cvCreateImage( cvGetSize( frameImage ), IPL_DEPTH_8U, 1 );	//	グレースケール画像用IplImage
	Mat grayImage( frameImage.size(), CV_8UC1 );

	//	行列を生成する
	//CvMat *worldCoordinates = cvCreateMat( CORNER_NUMBER * CAPTURE_NUMBER, 3, CV_64FC1 );	//	世界座標用行列
	vector<vector<Point3f> > worldCoordinates;
	//CvMat *imageCoordinates = cvCreateMat( CORNER_NUMBER * CAPTURE_NUMBER, 2, CV_64FC1 );	//	画像座標用行列
	vector<vector<Point2f> > imageCoordinates;

	//CvMat *pointCounts = cvCreateMat( CAPTURE_NUMBER, 1, CV_32SC1 );						//	コーナー数を格納した行列
	Mat pointCounts( CAPTURE_NUMBER, 1, CV_32SC1 );
	//CvMat *rotationVectors = cvCreateMat( CAPTURE_NUMBER, 3, CV_64FC1 );					//	回転ベクトル
	vector<Mat> rotationVectors;
	//CvMat *translationVectors = cvCreateMat( CAPTURE_NUMBER, 3, CV_64FC1 );				//	並進ベクトル
	vector<Mat> translationVectors;

	//	世界座標を設定する	
	for ( int i = 0; i < CAPTURE_NUMBER; i++){
		vector<Point3f> tmp_3f;
		for ( int j = 0; j < CORNER_NUMBER; j++){
			//cvSetReal2D( worldCoordinates, i * CORNER_NUMBER + j, 0, ( j % CORNER_WIDTH ) * UNIT );
			//cvSetReal2D( worldCoordinates, i * CORNER_NUMBER + j, 1, ( j / CORNER_WIDTH ) * UNIT );
			//cvSetReal2D( worldCoordinates, i * CORNER_NUMBER + j, 2, 0.0 ); 			
			tmp_3f.push_back( Point3f( ( j % CORNER_WIDTH ) * UNIT, ( j / CORNER_WIDTH ) * UNIT, 0.0 ));
		}
		worldCoordinates.push_back( tmp_3f );
	}	

	//	コーナー数を設定する
	for ( int i = 0; i < CAPTURE_NUMBER; i++){
		//cvSetReal2D( pointCounts, i, 0, CORNER_NUMBER );
		pointCounts.at<float>(i, 0) = CORNER_NUMBER;
	}
	
	while ( 1 ) {
		//frameImage = cvQueryFrame(capture);
		capture >> frameImage;

		//	コーナーを検出する
		findFlag = findCorners( frameImage, grayImage, corners );

		//key=cvWaitKey( 10 );
		key=waitKey( 10 );
		if ( key == ' ' ) {
			//	スペースキーが押されたら
			if ( findFlag != 0 ) {
				//	コーナーがすべて検出された場合
				//	画像座標を設定する
				vector<Point2f> tmp_2f;
				
				for ( int i = 0; i < CORNER_NUMBER; i++){
					//cvSetReal2D( imageCoordinates, captureCount * CORNER_NUMBER + i, 0, corners[i].x );
					//cvSetReal2D( imageCoordinates, captureCount * CORNER_NUMBER + i, 1, corners[i].y );
					tmp_2f.push_back( Point2f( corners[i].x, corners[i].y ) );
				}
				imageCoordinates.push_back( tmp_2f );

				captureCount++;
				/*画像保存する				
				imwrite("test.png",frameImage);
				*/

				//printf( "%d枚目キャプチャしました\n",captureCount );
				cout << captureCount << "枚目キャプチャしました\n";

				if ( captureCount == CAPTURE_NUMBER ) {
					//	設定した回数チェックパターンを撮った場合
					//	カメラパラメータを推定する
					/*
					cvCalibrateCamera2(
					worldCoordinates,
					imageCoordinates,
					pointCounts,
					cvGetSize( frameImage ),
					intrinsicMatrix,
					distortionCoefficient,
					rotationVectors,
					translationVectors,
					CALIBRATE_CAMERA_FLAG
					);
					*/
					calibrateCamera(
						worldCoordinates,
						imageCoordinates,
						frameImage.size(),
						intrinsicMatrix,
						distortionCoefficient,
						rotationVectors,
						translationVectors
						);

					//printf( "\nレンズ歪み係数\n" );
					cout << "\nレンズ歪み係数\n";
					//printMatrix( "%lf ", distortionCoefficient );
					cout << distortionCoefficient << endl;
					//printf( "\n内部パラメータ\n" );
					cout << "\n内部パラメータ\n";
					//printMatrix( "%lf ", intrinsicMatrix );
					cout << intrinsicMatrix << endl;

					returnValue = 1;
					break;
				}
			}
		} else if ( key == 'q' ) {
			//	'q'キーが押されたら
			returnValue = 0;
			break;
		}

		//	画像を表示する
		//cvShowImage( windowNameCapture, frameImage );
		imshow( windowNameCapture, frameImage );
	}

	//	メモリを解放する
	//cvReleaseMat( &worldCoordinates );
	//cvReleaseMat( &imageCoordinates );
	//cvReleaseMat( &pointCounts );
	//cvReleaseMat( &rotationVectors );
	//cvReleaseMat( &translationVectors );
	//cvReleaseImage( &grayImage );

	return returnValue;
}

//
//	外部パラメータを推定する
//
//	引数:
//      intrinsicMatrix       : 内部パラメータ行列
//      distortionCoefficient : レンズ歪み係数
//
//void estimateExtrinsicParameters( CvMat *intrinsicMatrix, CvMat *distortionCoefficient ) {
void estimateExtrinsicParameters( Mat &intrinsicMatrix, Mat &distortionCoefficient ) {

	int key;		//	キー入力用の変数
	int findFlag;	//	コーナーが完全に検出できたかのフラグ

	//	画像を生成する
	//IplImage *frameImage = cvQueryFrame( capture );												//	キャプチャ画像用IplImage
	Mat frameImage;
	capture >> frameImage;
	//IplImage *grayImage =cvCreateImage( cvGetSize( frameImage ), IPL_DEPTH_8U, 1 );				//	グレースケール画像用IplImage
	Mat grayImage( frameImage.size(), CV_8UC1 );
	//IplImage *undistortionImage = cvCreateImage( cvGetSize( frameImage ), IPL_DEPTH_8U, 3 );	//	レンズ歪みを補正した画像用IplImage
	Mat undistortionImage( frameImage.size(), CV_8UC3 );

	//	行列を生成する
	//CvMat *worldCoordinates = cvCreateMat( CORNER_NUMBER, 3, CV_64FC1 );	//	世界座標用行列
	vector<Point3d> worldCoordinates;
	//CvMat *imageCoordinates = cvCreateMat( CORNER_NUMBER, 2, CV_64FC1 );	//	画像座標用行列
	vector<Point2d> imageCoordinates;
	//CvMat *rotationVector = cvCreateMat( 1, 3, CV_64FC1 );					//	回転ベクトル
	Mat rotationVector;
	//CvMat *rotationMatrix = cvCreateMat( 3, 3, CV_64FC1 );					//	回転行列
	Mat rotationMatrix;
	//CvMat *translationVector = cvCreateMat( 1, 3, CV_64FC1 );				//	並進ベクトル
	Mat translationVector;

	//CvPoint2D32f corners[CORNER_NUMBER];	//	コーナーの位置を格納する変数
	vector<Point2f> corners;

	//	ウィンドウを生成する
	//cvNamedWindow( windowNameUndistortion, CV_WINDOW_AUTOSIZE );
	namedWindow( windowNameUndistortion, CV_WINDOW_AUTOSIZE );

	//	世界座標を設定する
	for ( int i = 0; i < CORNER_NUMBER; i++ ){
		//cvSetReal2D( worldCoordinates, i, 0, ( i % CORNER_WIDTH ) * UNIT );		
		//cvSetReal2D( worldCoordinates, i, 1, ( i / CORNER_WIDTH ) * UNIT );		
		//cvSetReal2D( worldCoordinates, i, 2, 0.0 );		
		worldCoordinates.push_back( Point3d( ( i % CORNER_WIDTH ) * UNIT, ( i / CORNER_WIDTH ) * UNIT, 0.0 ));
	}

	while ( 1 ) {
		//frameImage = cvQueryFrame( capture );
		capture >> frameImage;

		//	コーナーを検出する
		findFlag = findCorners( frameImage, grayImage, corners );

		//	レンズ歪みを補正した画像を生成する
		/*
		cvUndistort2(
		frameImage,
		undistortionImage,
		intrinsicMatrix,
		distortionCoefficient
		);
		*/
		undistort(
			frameImage,
			undistortionImage,
			intrinsicMatrix,
			distortionCoefficient
			);

		//key=cvWaitKey( 10 );
		key = waitKey( 10 );
		if ( key == ' ' ) {
			//	スペースキーが押されたら
			if ( findFlag != 0 ) {
				//	コーナーがすべて検出された場合
				//	画像座標を設定する
				for ( int i = 0; i < CORNER_NUMBER; i++ ){
					//cvSetReal2D( imageCoordinates, i, 0, corners[i].x);
					//cvSetReal2D( imageCoordinates, i, 1, corners[i].y);
					imageCoordinates.push_back( Point2d( corners[i].x, corners[i].y ) );
				}

				//	外部パラメータを推定する
				/*
				cvFindExtrinsicCameraParams2(
				worldCoordinates,
				imageCoordinates,
				intrinsicMatrix,
				distortionCoefficient,
				rotationVector,
				translationVector
				);
				*/
				solvePnP(
					worldCoordinates,
					imageCoordinates,
					intrinsicMatrix,
					distortionCoefficient,
					rotationVector,
					translationVector
				);


				//	回転ベクトルを回転行列に変換する
				//cvRodrigues2( rotationVector, rotationMatrix, NULL );
				Rodrigues( rotationVector, rotationMatrix);

				printf( "\n外部パラメータ\n" );
				//printExtrinsicMatrix( rotationMatrix, translationVector );
				cout << rotationMatrix << endl;
				cout << translationVector << endl;
			}
		} else if ( key == 'q' ) {
			//	'q'キーが押されたらループを抜ける
			break;
		}

		//cvFlip( undistortionImage, undistortionImage );
		flip( undistortionImage, undistortionImage, 0);

		//	画像を表示する
		//cvShowImage( windowNameCapture, frameImage );
		imshow( windowNameCapture, frameImage );
		//cvShowImage( windowNameUndistortion, undistortionImage );
		imshow( windowNameUndistortion, undistortionImage );
	}

	//	メモリを解放する
	//cvReleaseMat( &worldCoordinates );
	//cvReleaseMat( &imageCoordinates );
	//cvReleaseMat( &rotationVector );
	//cvReleaseMat( &rotationMatrix );
	//cvReleaseMat( &translationVector );
	//cvReleaseImage( &grayImage );
	//cvReleaseImage( &undistortionImage );
}

int main(){

	//	行列を生成する
	//CvMat *intrinsicMatrix = cvCreateMat( 3, 3, CV_64FC1 );			//	内部パラメータ用行列
	Mat intrinsicMatrix;
	//CvMat *distortionCoefficient = cvCreateMat( 4, 1, CV_64FC1 );	//	レンズ歪み係数用行列
	Mat distortionCoefficient;

	if ( CAPTURE_NUMBER < 3 ) {
		printf( "チェックパターンは3回以上撮影してください\n" );
	}

	//	カメラを初期化する
	/*
	if ( ( capture = cvCreateCameraCapture( -1 ) ) == NULL ) {
	//	カメラが見つからなかった場合
	printf( "カメラが見つかりません\n" );
	return -1;
	}
	*/
	if(!capture.isOpened()) {
		cout << "カメラが見つかりません" << endl;
		return -1;
	}

	//	ウィンドウを生成する
	//cvNamedWindow( windowNameCapture, CV_WINDOW_AUTOSIZE );
	namedWindow( windowNameCapture, CV_WINDOW_AUTOSIZE );

	if ( estimateCameraParameters( intrinsicMatrix, distortionCoefficient ) != 0 ) {
		estimateExtrinsicParameters( intrinsicMatrix, distortionCoefficient );
	}

	//	キャプチャを解放する
	//cvReleaseCapture( &capture );
	//	メモリを解放する
	//cvReleaseMat( &intrinsicMatrix );
	//cvReleaseMat( &distortionCoefficient );
	//	ウィンドウを破棄する
	//cvDestroyWindow( windowNameCapture );
	//cvDestroyWindow( windowNameUndistortion );

	return 0;
}
