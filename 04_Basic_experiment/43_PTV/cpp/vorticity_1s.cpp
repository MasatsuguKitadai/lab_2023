/******************************************************************************
PROGRAM : 12_PTV
AUTHER  : Masatsugu Kitadai
DATE    : 2022/12/13
******************************************************************************/
// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
#include "../hpp/settings.hpp"

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

// ファイル名
char filename_ptv[100];

// range x
float x_min = 50;
float x_max = width_px - 50;

// range y
float y_min = 50;
float y_max = height_px - 50;

// range color
float cb_min = -0.5;
float cb_max = 0.5;

// label
const char *xxlabel = "y [px]";
const char *yylabel = "z [px]";

/******************************************************************************/

int main()
{

    /* 渦度 */
}