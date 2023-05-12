/*****************************************************************************
PROGRAM : header_create_by_opencv
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/11
******************************************************************************/

// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

// opencv ライブラリ
#include <opencv2/core.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/highgui.hpp>

// 自作設定ファイル
#include "../hpp/settings.hpp"
#include "../../parameters/parameters.hpp"

/*****************************************************************************/

int main()
{
    //** 撮影画像用 **//

    // 書き出しファイルの指定s
    // sprintf(filename[0], "header/%dx%d_8bit.bmp", width_original, height_original);
    // sprintf(filename[1], "header/%dx%d_24bit.bmp", width_original, height_original);

    // 8bit グレースケールの作成
    const int size_x = 10;
    const int size_y = size_x;

    sprintf(filename[0], "header/%dx%d_8bit.bmp", size_x, size_y);

    cv::Mat image_0(cv::Size(size_x, size_y), CV_8UC1);
    image_0 = cv::Scalar::all(0);

    //  画像を書き出し
    cv::imwrite(filename[0], image_0);

    // // 8bit グレースケールの作成
    // cv::Mat image_0(cv::Size(width_original, height_original), CV_8UC1);
    // image_0 = cv::Scalar::all(0);

    // //  画像を書き出し
    // cv::imwrite(filename[0], image_0);

    // // 24bit カラーの作成
    // cv::Mat image_1(cv::Size(width_original, height_original), CV_8UC3);
    // image_1 = cv::Scalar::all(0);

    // // 画像を書き出し
    // cv::imwrite(filename[1], image_1);

    // //** 変換画像用 **//

    // // 書き出しファイルの指定
    // sprintf(filename[2], "header/%dx%d_8bit.bmp", width, height);
    // sprintf(filename[3], "header/%dx%d_24bit.bmp", width, height);

    // // 8bit グレースケールの作成
    // cv::Mat image_2(cv::Size(width, height), CV_8UC1);
    // image_2 = cv::Scalar::all(0);

    // //  画像を書き出し
    // cv::imwrite(filename[2], image_2);

    // // 24bit カラーの作成
    // cv::Mat image_3(cv::Size(width, height), CV_8UC3);
    // image_3 = cv::Scalar::all(0);

    // // 画像を書き出し
    // cv::imwrite(filename[3], image_3);

    return 0;
}