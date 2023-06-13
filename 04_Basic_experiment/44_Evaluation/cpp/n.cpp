/******************************************************************************
PROGRAM : n
AUTHER  : Masatsugu Kitadai
DATE    : 2023/6/8
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

const float grid_distance = 10.0; // 代表点距離 [mm]
const float t = delta_n / shutter_speed;

/** プロトタイプ宣言 **/
void Create_new_file(const char *name, const char *data_set);
void Load_data(const char *name, const char *data_set, int n);
void Gnuplot(const char *name, const char *data_set);

/******************************************************************************/
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
    char dirname[100];
    sprintf(dirname, "%s/%s/44_Evaluation/%s", dir_path, name, data_set);
    mkdir(dirname, dirmode);
    sprintf(dirname, "%s/%s/44_Evaluation/%s/data", dir_path, name, data_set);
    mkdir(dirname, dirmode);
    sprintf(dirname, "%s/%s/44_Evaluation/%s/graph", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    // 初期ファイルの作成
    Create_new_file(name, data_set);

    // データの読み込みと整理
    for (int i = 8; i <= 17; i++)
    {
        Load_data(name, data_set, i);
    }

    Gnuplot(name, data_set);

    return 0;
}

/******************************************************************************
PROGRAM : Create_new_file
概要：
******************************************************************************/
void Create_new_file(const char *name, const char *data_set)
{
    char filename[100];
    for (int i = 0; i < width_mm / grid_distance; i++)
        for (int j = 0; j < height_mm / grid_distance; j++)
        {
            float y_tmp = i * grid_distance;
            float z_tmp = j * grid_distance;
            sprintf(filename, "%s/%s/44_Evaluation/%s/data/x=%.0f,y=%.0f.dat", dir_path, name, data_set, y_tmp, z_tmp);
            fp = fopen(filename, "w");
            fclose(fp);
        }
}

/******************************************************************************
PROGRAM : Load_data
概要：
******************************************************************************/
void Load_data(const char *name, const char *data_set, int n)
{
    /* 変数の設定 */
    vector<float> y; // y軸方向 [mm]
    vector<float> z; // z軸方向 [mm]
    vector<float> v; // y方向の速度ベクトル [mm/s]
    vector<float> w; // z方向の速度ベクトル [mm/s]
    vector<float> r; // 相互相間係数 [-]

    /* ファイルの読み取り */
    float buf[10]; // 読み込み用のバッファ
    char readfile[100];
    sprintf(readfile, "%s/%s/43_PTV/%s_n=%d/PTV_velocity_dat/velocity.dat", dir_path, name, data_set, n);
    fp = fopen(readfile, "r");
    while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5])) != EOF)
    {
        const int y_buf = fmod(buf[0], grid_distance);
        const int z_buf = fmod(buf[1], grid_distance);

        if (y_buf == 0 && z_buf == 0)
        {
            y.push_back(buf[0]);
            z.push_back(buf[1]);
            v.push_back(buf[2]);
            w.push_back(buf[3]);
            r.push_back(buf[5]);
        }
    }
    fclose(fp);

    /* 代表点ごとの記入 (n-v) */
    char filename[100];
    for (int i = 0; i < r.size(); i++)
    {
        sprintf(filename, "%s/%s/44_Evaluation/%s/data/x=%.0f,y=%.0f.dat", dir_path, name, data_set, y[i], z[i]);
        fp = fopen(filename, "a");
        fprintf(fp, "%d\t%f\t%f\t%f\n", n, v[i] / t, w[i] / t, r[i]);
        fclose(fp);
    }
}

/******************************************************************************
PROGRAM : Gnuplot
概要：
******************************************************************************/
void Gnuplot(const char *name, const char *data_set)
{
    // ファイルの名前の取得
    char filename[100], graphname[100];
    for (int i = 0; i < width_mm / grid_distance; i++)
        for (int j = 0; j < height_mm / grid_distance; j++)
        {
            float y_tmp = i * grid_distance;
            float z_tmp = j * grid_distance;
            sprintf(filename, "%s/%s/44_Evaluation/%s/data/x=%.0f,y=%.0f.dat", dir_path, name, data_set, y_tmp, z_tmp);
            sprintf(graphname, "%s/%s/44_Evaluation/%s/graph/x=%.0f,y=%.0f.png", dir_path, name, data_set, y_tmp, z_tmp);

            // 描画範囲の指定
            const int x_min = 7;
            const int x_max = 18;

            const float y_min = -80;
            const float y_max = 80;

            // Gnuplot 呼び出し
            if ((gp = popen("gnuplot", "w")) == NULL)
            {
                printf("gnuplot is not here!\n");
                exit(0); // gnuplotが無い場合、異常ある場合は終了
            }

            fprintf(gp, "set terminal png size 800, 800 font 'Times New Roman, 20'\n");
            fprintf(gp, "set size ratio 1\n");

            // 出力ファイル
            fprintf(gp, "set output '%s'\n", graphname);

            // 非表示
            fprintf(gp, "set key right top\n");

            // 軸の範囲
            fprintf(gp, "set xrange [%d:%d]\n", x_min, x_max);
            fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
            fprintf(gp, "set y2range [%.3f:%.3f]\n", -0.0, 1.0);

            // 軸ラベル
            fprintf(gp, "set xlabel 'n [-]'\n");
            fprintf(gp, "set ylabel 'Velocity [mm/s]'\n");
            fprintf(gp, "set y2label 'Correlation [-]'\n");

            // 軸のラベル位置
            fprintf(gp, "set xlabel offset 0.0, 0.0\n");
            fprintf(gp, "set ylabel offset 0.0, 0.0\n");
            fprintf(gp, "set y2label offset 0.0, 0.0\n");
            fprintf(gp, "set ticslevel 0\n");

            // 軸の数値位置
            fprintf(gp, "set xtics 1 offset 0.0, 0.0\n");
            fprintf(gp, "set ytics 20 offset 0.0, 0.0 nomirror\n");
            fprintf(gp, "set y2tics 0.2 offset 0.0, 0.0\n");

            // グラフの出力 (n-v)
            fprintf(gp, "plot '%s' using 1:2 axes x1y1 with points lc 'blue' ps 1.5 pt 7 title 'v', ", filename);
            fprintf(gp, "'%s' using 1:2 axes x1y1 with lines lc 'blue' lw 1 notitle,", filename);
            // グラフの出力 (n-w)
            fprintf(gp, "'%s' using 1:3 axes x1y1 with points lc 'red' ps 1.5 pt 7 title 'w', ", filename);
            fprintf(gp, "'%s' using 1:3 axes x1y1 with lines lc 'red' lw 1 notitle, ", filename);
            // グラフの出力 (n-r)
            fprintf(gp, "'%s' using 1:4 axes x1y2 with points lc 'black' ps 1.5 pt 7 title 'R', ", filename);
            fprintf(gp, "'%s' using 1:4 axes x1y2 with lines lc 'black' lw 1 notitle\n", filename);

            fflush(gp); // Clean up Data

            fprintf(gp, "exit\n"); // Quit gnuplot

            pclose(gp);
        }
}