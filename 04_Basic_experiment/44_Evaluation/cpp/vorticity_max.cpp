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
const float t = delta_n / shutter_speed; // 撮影時刻 [s]
const int n = n_max - n_min;             // 配列の数 [-]

/** プロトタイプ宣言 **/
void Vorticity(const char *name, const char *data_set);
void Plot_vorticity(const char *name, const char *data_set);

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

    /* 速度場のプロット */
    Vorticity(name, data_set);

    /* 速度場のプロット */
    Plot_vorticity(name, data_set);

    return 0;
}

/******************************************************************************
PROGRAM : Vorticity
概要：
******************************************************************************/
void Vorticity(const char *name, const char *data_set)
{
    /* 変数の設定 */
    vector<float> y;     // y軸方向 [mm]
    vector<float> z;     // z軸方向 [mm]
    vector<float> v;     // y方向の速度ベクトル [mm/s]
    vector<float> w;     // z方向の速度ベクトル [mm/s]
    vector<float> vw;    // 速度ベクトルの大きさ [mm/s]
    vector<float> r;     // 相互相間係数 [-]
    vector<float> omega; // 相互相間係数 [-]

    /* ファイルの読み取り */
    float buf[10];      // 読み込み用のバッファ
    char readfile[100]; //
    sprintf(readfile, "%s/%s/44_Evaluation/%s/result/velocity.dat", dir_path, name, data_set);
    fp = fopen(readfile, "r");
    while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4])) != EOF)
    {
        y.push_back(buf[0]);
        z.push_back(buf[1]);
        v.push_back(buf[2]);
        w.push_back(buf[3]);
        vw.push_back(buf[4]);
    }
    fclose(fp);

    // サイズのリサイズ
    omega.resize(y.size());

    /* 渦度の計算 */
    const int l = 21;

    for (int i = l; i < y.size() - l; i++)
    {
        const float cell_11 = w[i - 1]; // (i+1,   j)
        const float cell_12 = w[i + 1]; // (i-1,   j)
        const float cell_21 = v[i - l]; // (  i, j+1)
        const float cell_22 = v[i + l]; // (  i, j-1)

        const float cell_1 = (cell_12 - cell_11) / grid_distance;
        const float cell_2 = (cell_22 - cell_21) / grid_distance;
        omega[i] = cell_1 - cell_2; // 渦度
    }

    /* 書き出し */
    int counter = 0;
    const float offset = width_mm / width_px * grid_size / 2;
    char whitefile[100]; //
    sprintf(whitefile, "%s/%s/44_Evaluation/%s/result/vorticity.dat", dir_path, name, data_set);
    fp = fopen(whitefile, "w");
    for (int i = 0; i < omega.size(); i++)
    {
        // pm3d 用の改行
        if (counter == 20)
        {
            counter += 1;
            continue;
        }
        else if (counter == 21)
        {
            fprintf(fp, "\n");
            counter = 0;
        }

        fprintf(fp, "%f\t%f\t%f\n", y[i] + offset, z[i] + offset, omega[i]);
        counter += 1;
    }

    fclose(fp);
}

/******************************************************************************
PROGRAM : plot_vorticity
概要：
******************************************************************************/
void Plot_vorticity(const char *name, const char *data_set)
{
    /** Gnuplot **/

    // ファイル名
    char filename_1[100];
    char filename_2[100];
    char graphname[100];
    char graphtitle[100];

    sprintf(filename_1, "%s/%s/44_Evaluation/%s/result/vorticity.dat", dir_path, name, data_set);
    sprintf(filename_2, "%s/%s/44_Evaluation/%s/result/velocity.dat", dir_path, name, data_set);
    sprintf(graphname, "%s/%s/44_Evaluation/%s/result/vorticity.svg", dir_path, name, data_set);

    // 軸の設定

    // // range x
    const float x_min = 0.0;
    const float x_max = width_mm;

    // // range y
    const float y_min = 0.0;
    const float y_max = height_mm;

    // range color
    const float cb_min = -0.2;
    const float cb_max = 0.2;

    // label
    const char *xxlabel = "{/:Italic y} [mm]";
    const char *yylabel = "{/:Italic z} [mm]";
    const char *cblabel = "{/:Italic n} [-]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 800, 400 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname);
    fprintf(gp, "set multiplot\n");

    // 非表示
    fprintf(gp, "unset key\n");

    // グラフのタイプ
    fprintf(gp, "set view map\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%f':'%f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 1.5, 0.0\n", yylabel);
    fprintf(gp, "set cblabel '%s' offset -1.0, 0.0\n", cblabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 10 offset 0.0, 0.0\n");
    fprintf(gp, "set ytics 10 offset 1.0, 0.0\n");
    fprintf(gp, "set cbtics 0.1\n");

    // グラフの出力
    fprintf(gp, "set pm3d map\n");
    fprintf(gp, "splot '%s' using 1:2:3 with pm3d, '%s' using 1:2:(0):($3*2.0):($4*2.0):(0) with vectors head filled lt 2 lc 'black' lw 1 notitle\n", filename_1, filename_2);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}
