/******************************************************************************
PROGRAM : make_dir
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/11
******************************************************************************/

// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

/******************************************************************************/

int main()
{
    // ディレクトリの作成
    char dir_name[100];
    mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

    sprintf(dir_name, "result/03");
    mkdir(dir_name, dirmode);

    // ディレクトリの作成
    sprintf(dir_name, "result/03/stretch");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/stretch/blue_png");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/stretch/green_png");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/labeling");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/labeling/blue_dat");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/labeling/green_dat");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/labeling/blue_svg");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/labeling/green_svg");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/ptv");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/ptv/vector");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/ptv/img");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/ptv/velocity");
    mkdir(dir_name, dirmode);

    sprintf(dir_name, "result/03/ptv/vorticity");
    mkdir(dir_name, dirmode);

    // sprintf(dir_name, "result/03/ptv_100ms");
    // mkdir(dir_name, dirmode);

    // sprintf(dir_name, "result/03/ptv_100ms/img");
    // mkdir(dir_name, dirmode);

    // sprintf(dir_name, "result/03/ptv_100ms/velocity");
    // mkdir(dir_name, dirmode);

    // sprintf(dir_name, "result/03/ptv_100ms/vorticity");
    // mkdir(dir_name, dirmode);

    // sprintf(dir_name, "result/03/ptv_100ms/img_vor");
    // mkdir(dir_name, dirmode);

    return 0;
}