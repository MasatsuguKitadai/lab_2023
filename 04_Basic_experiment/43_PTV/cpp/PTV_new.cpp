/******************************************************************************
PROGRAM : PTV_new
AUTHER  : Masatsugu Kitadai
DATE    : 2022/07/22
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

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/*****************************************************************************/

#include "../hpp/settings.hpp"

/******************************************************************************
FUNCTION：main
概要    ：
******************************************************************************/
int main()
{
    /* 保存ディレクトリの設定 */
    string name_str;
    cout << "Data Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    string data_set_str;
    cout << "Data Set:";
    cin >> data_set_str;
    const char *data_set = data_set_str.c_str();

    /* ディレクトリの作成 */
    char dirname[3][100];
    sprintf(dirname[0], "%s/%s/43_PTV/%s/PTV_vector_dat", dir_path, name, data_set);
    sprintf(dirname[1], "%s/%s/43_PTV/%s/PTV_vector_svg", dir_path, name, data_set);
    mkdir(dirname[0], dirmode);
    mkdir(dirname[1], dirmode);

    return 0;
}

/******************************************************************************
FUNCTION：PTV
概要    ：
******************************************************************************/
void PTV(const char *name, const char *data_set, int n)
{
    /** ラベリング位置(青)の取得 **/
    vector<float> xb;
    vector<float> yb;
    float x_tmp, y_tmp, buf;
    char readfile[100];
    sprintf(readfile, "%s/%s/43_PTV/%s/labeling_position_dat/%04d.dat", dir_path, name, data_set, num);
    fp = fopen(readfile, "r");
    while ((fscanf(fp, "%f\t%f\t%f", &x_tmp, &y_tmp, &buf)) != EOF)
    {
        xb.push_back(x_tmp);
        yb.push_back(y_tmp);
    }
    fclose(fp);

    /** ラベリング位置(緑)の取得 **/
    vector<float> xg;
    vector<float> yg;
    sprintf(readfile, "%s/%s/43_PTV/%s/labeling_position_dat_G/%04d.dat", dir_path, name, data_set, num);
    fp = fopen(readfile, "r");
    while ((fscanf(fp, "%f\t%f\t%f", &x_tmp, &y_tmp, &buf)) != EOF)
    {
        xg.push_back(x_tmp);
        yg.push_back(y_tmp);
    }
    fclose(fp);
}