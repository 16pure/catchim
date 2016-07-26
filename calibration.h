//---------------------------------------------------------
// 概要      : カメラキャリブレーション
// File Name : calibration.h
// Library   : OpenCV 2
//---------------------------------------------------------

#ifndef CALIBRATION_H
#define CALIBRATION_H

//#include <stdio.h>
//#include <cv.h>
//#include <cxcore.h>
//#include <highgui.h>
#include <iostream>
#include <opencv2/opencv.hpp>

#define CAPTURE_NUMBER	(5)									//	撮影するチェックパターンの回数(3回以上)
//#define CORNER_WIDTH	12									//	コーナーの横の個数
#define CORNER_WIDTH	(10)									//	コーナーの横の個数
//#define CORNER_HEIGHT	11									//	コーナーの縦の個数
#define CORNER_HEIGHT	(7)									//	コーナーの縦の個数
#define CORNER_NUMBER	( CORNER_WIDTH * CORNER_HEIGHT )	//　コーナーの個数
//#define UNIT			15									//	コーナー間隔 (単位はmm)
#define UNIT			(24.0)									//	コーナー間隔 (単位はmm)

//	cvFindChessboardCornersのフラグ用定数
#define ADAPTIVE_THRESH	1	//	CV_CALIB_CB_ADAPTIVE_THRESHを設定するかどうか
#define NORMALIZE_IMAGE	1	//	CV_CALIB_CB_NORMALIZE_IMAGEを設定するかどうか
#define FILTER_QUADS	1	//	CV_CALIB_CB_FILTER_QUADSを設定するかどうか

//	cvTermCriteria用定数
#define MAX_ITERATIONS	20		//	反復数の最大値
#define EPSILON			0.001	//	目標精度

//	cvFindCornerSubPix用定数
#define SEARCH_WINDOW_HALF_WIDTH	5	//	検索ウィンドウの横幅の半分のサイズ
#define SEARCH_WINDOW_HALF_HEIGHT	5	//	検索ウィンドウの縦幅の半分のサイズ
#define DEAD_REGION_HALF_WIDTH		-1	//	総和対象外領域の横幅の半分のサイズ
#define DEAD_REGION_HALF_HEIGHT	-1	//	総和対象外領域の縦幅の半分のサイズ

//	cvCalibrateCamera2のフラグ用定数
#define CALIBRATE_CAMERA_FLAG	0	//	cvCalibrateCamera2のflagを設定する(リファレンス参照)

#endif CALIBRATION_H
