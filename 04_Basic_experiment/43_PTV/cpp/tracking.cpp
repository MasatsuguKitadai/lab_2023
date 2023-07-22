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

/* プロトタイプ宣言 */
void Timeline(const char *name, const char *data_set);
void plot(const char *name, const char *data_set);

/******************************************************************************/
int main()
{
    /* 保存ディレクトリの設定 */
    string name_str;
    string data_set_str;

    cout << "Data Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    cout << "Data Set:";
    cin >> data_set_str;
    const char *data_set = data_set_str.c_str();

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/43_PTV/%s/tracking", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    Timeline(name, data_set);
    plot(name, data_set);

    return 0;
}

/******************************************************************************
PROGRAM : Time_line
概要：
******************************************************************************/
void Tracking(const char *name, const char *data_set)
{
    /* 粒子位置の読み込み */
    char readfile_blue[100], readfile_green[100], writefile[100];
    int x_tmp, y_tmp, buf;
    int progress_counter = 0;
    const char *program_name = "Tracking";

    // 初期ファイルの作成
    sprintf(writefile, "%s/%s/43_PTV/%s/tracking/tracking.dat", dir_path, name, data_set);
    fp = fopen(writefile, "w");
    fclose(fp);

    for (int i = 1; i <= number; i++)
    {
        /* ラベル：Blue */
        vector<int> xb;
        vector<int> yb;
        sprintf(readfile_blue, "%s/%s/43_PTV/%s/labeling_position_dat/%04d.dat", dir_path, name, data_set, i);
        fp = fopen(readfile_blue, "r");
        while ((fscanf(fp, "%d\t%d\t%d", &x_tmp, &y_tmp, &buf)) != EOF)
        {
            xb.push_back(x_tmp);
            yb.push_back(y_tmp);
        }
        fclose(fp);

        /* ラベル：Green */
        vector<int> xg;
        vector<int> yg;
        sprintf(readfile_green, "%s/%s/43_PTV/%s/labeling_position_dat/%04d.dat", dir_path, name, data_set, i);
        fp = fopen(readfile_green, "r");
        while ((fscanf(fp, "%d\t%d\t%d", &x_tmp, &y_tmp, &buf)) != EOF)
        {
            xg.push_back(x_tmp);
            yg.push_back(y_tmp);
        }
        fclose(fp);

        // 粒子位置の書き出し
        fp = fopen(writefile, "a");
        for (int j = 0; j < x.size(); j++)
        {
            fprintf(fp, "%f\t%d\t%d\n", i / frame_rate, x[j], y[j]);
        }
        fclose(fp);

        /* メモリの解放 */
        x.clear();
        x.shrink_to_fit();
        y.clear();
        y.shrink_to_fit();

        /* 進捗表示 */
        progress_counter = Progress_meter(program_name, i - 1, number, progress_counter);
    }
}