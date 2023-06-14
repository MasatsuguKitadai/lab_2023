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

/* 変数の設定 */
const float grid_distance = 10.0;        // 代表点距離 [mm]
const float t = delta_n / shutter_speed; // 撮影時刻 [s]
const int n_max = 17;                    // PTVの最大対応枚数 [-]
const int n_min = 8;                     // PTVの最小対応枚数 [-]
const int n = n_max - n_min;             //

vector<vector<float>> y(n); // y軸方向 [mm]
vector<vector<float>> z(n); // z軸方向 [mm]
vector<vector<float>> v(n); // y方向の速度ベクトル [mm/s]
vector<vector<float>> w(n); // z方向の速度ベクトル [mm/s]
vector<vector<float>> r(n); // 相互相間係数 [-]

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
    sprintf(dirname, "%s/%s/44_Evaluation/%s/velocity", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    cout << "check" << endl;

    // データの読み込み
    for (int i = n_min; i <= n_max; i++)
    {
        Load_data(name, data_set, i);
    }
    cout << "check" << endl;

    cout << "n=" << r.size() << endl;

    // // 相間係数の並び替え
    // for (int i = 0; i < ; i++)
    //     for (int j = 0; j < n; j++)
    //     {
    //         float r_tmp = 0;
    //         int n_tmp = 0;
    //         if (r[i][j] > r_tmp)
    //         {
    //             n_tmp = j;
    //         }
    //     }

    return 0;
}

/******************************************************************************
PROGRAM : Load_data
概要：
******************************************************************************/
void Load_data(const char *name, const char *data_set, int n)
{
    /* ファイルの読み取り */
    float buf[10];      // 読み込み用のバッファ
    char readfile[100]; //
    sprintf(readfile, "%s/%s/43_PTV/%s_n=%d/PTV_velocity_dat/velocity.dat", dir_path, name, data_set, n);
    fp = fopen(readfile, "r");
    while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5])) != EOF)
    {
        y[n - 1].push_back(buf[0]);
        z[n - 1].push_back(buf[1]);
        v[n - 1].push_back(buf[2]);
        w[n - 1].push_back(buf[3]);
        r[n - 1].push_back(buf[5]);
    }
    fclose(fp);
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
            sprintf(filename, "%s/%s/44_Evaluation/%s/data/y=%.0f,z=%.0f.dat", dir_path, name, data_set, y_tmp, z_tmp);
            sprintf(graphname, "%s/%s/44_Evaluation/%s/graph/y=%.0f,z=%.0f.png", dir_path, name, data_set, y_tmp, z_tmp);

            // 描画範囲の指定
            const float x_min = 7.5;
            const float x_max = 17.5;

            const float y_min = -100;
            const float y_max = 100;

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
            fprintf(gp, "set xrange [%.1f:%.1f]\n", x_min, x_max);
            fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
            fprintf(gp, "set y2range [%.3f:%.3f]\n", -0.0, 1.0);

            // 軸ラベル
            fprintf(gp, "set xlabel 'n [-]'\n");
            fprintf(gp, "set ylabel 'Velocity [mm/s]'\n");
            fprintf(gp, "set y2label 'Correlation [-]'\n");

            // 軸のラベル位置
            fprintf(gp, "set xlabel offset 0.0, 0.0\n");
            fprintf(gp, "set ylabel offset 1.0, 0.0\n");
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