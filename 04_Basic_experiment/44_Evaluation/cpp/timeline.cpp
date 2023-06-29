/******************************************************************************
PROGRAM : Timeline
AUTHER  : Masatsugu Kitadai
DATE    : 2023/06/28
******************************************************************************/
// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

using namespace std;
#include <vector>
#include <algorithm>
#include <iostream>
#include "../hpp/settings.hpp"

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/******************************************************************************/

int main()
{
    /* 保存ディレクトリの設定 */
    string name_str;
    cout << "Case Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    /* ディレクトリの作成 */
    char dirname[2][100];
    sprintf(dirname[0], "%s/%s/PTV/PTV_vorticity_dat", dir_path, name);
    sprintf(dirname[1], "%s/%s/PTV/PTV_vorticity_svg", dir_path, name);
    mkdir(dirname[0], dirmode);
    mkdir(dirname[1], dirmode);

    return 0;
}