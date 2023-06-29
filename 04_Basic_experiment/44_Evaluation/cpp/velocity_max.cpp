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

vector<vector<float>> y(n + 1);  // y軸方向 [mm]
vector<vector<float>> z(n + 1);  // z軸方向 [mm]
vector<vector<float>> v(n + 1);  // y方向の速度ベクトル [mm/s]
vector<vector<float>> w(n + 1);  // z方向の速度ベクトル [mm/s]
vector<vector<float>> vw(n + 1); // 速度ベクトルの大きさ [mm/s]
vector<vector<float>> r(n + 1);  // 相互相間係数 [-]
vector<float> n_r;               // 最大相間係数をとる対応数 [-]

/** プロトタイプ宣言 **/
void Select_Rmax(const char *name, const char *data_set);
void Load_data(const char *name, const char *data_set, int n2);
void Plot_velocity(const char *name, const char *data_set);
void Plot_number(const char *name, const char *data_set);

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
    sprintf(dirname, "%s/%s/44_Evaluation/%s/result", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    /* 最大相間係数の選択 */
    Select_Rmax(name, data_set);

    /* 速度場のプロット */
    Plot_number(name, data_set);

    return 0;
}

/******************************************************************************
PROGRAM : Select_Rmax
概要：
******************************************************************************/
void Select_Rmax(const char *name, const char *data_set)
{
    /* データの読み込み */
    for (int i = n_min; i <= n_max; i++)
    {
        Load_data(name, data_set, i);
    }

    /* データの数 */
    printf("\n");
    printf("number = %ld\n", r.at(0).size());
    printf("\n");

    /* 最大相間係数の取得 */
    for (int i = 0; i < r.at(0).size(); i++)
    {
        /* 最も高い相間係数の取得 */
        float r_tmp = 0;
        int n_tmp = 0;
        for (int j = 0; j < n; j++)
        {
            if (r[j][i] > r_tmp)
            {
                r_tmp = r[j][i];
                n_tmp = j;
            }
        }

        /* 格納 */
        n_r.push_back(n_tmp);
    }

    /* データの書き出し (1) Velocity*/
    int counter = 0;
    char filename[100];
    sprintf(filename, "%s/%s/44_Evaluation/%s/result/velocity.dat", dir_path, name, data_set);
    fp = fopen(filename, "w");
    for (int i = 0; i < r.at(0).size(); i++)
    {
        // // pm3d 用の改行
        // if (counter == 21)
        // {
        //     fprintf(fp, "\n");
        //     counter = 0;
        // }

        // データの記入
        const int tmp = n_r[i];
        fprintf(fp, "%f\t%f\t%f\t%f\t%f\n", y[tmp][i], z[tmp][i], v[tmp][i], w[tmp][i], vw[tmp][i]);
        counter += 1;
    }
    fclose(fp);
    Plot_velocity(name, data_set);

    /* データの書き出し (2) Number*/
    const float offset = width_mm / width_px * grid_size / 2;
    counter = 0;
    sprintf(filename, "%s/%s/44_Evaluation/%s/result/number.dat", dir_path, name, data_set);
    fp = fopen(filename, "w");
    for (int i = 0; i < r.at(0).size(); i++)
    {
        // pm3d 用の改行
        if (counter == 21)
        {
            fprintf(fp, "\n");
            counter = 0;
        }

        // データの記入
        const int tmp = n_r[i];
        if (y[tmp][i] != 0 && z[tmp][i] != 0)
        {
            fprintf(fp, "%f\t%f\t%f\n", y[tmp][i] - offset, z[tmp][i] - offset, n_r[i] + n_min);
        }

        counter += 1;
    }
    fclose(fp);
}

/******************************************************************************
PROGRAM : Load_data
概要：
******************************************************************************/
void Load_data(const char *name, const char *data_set, int n2)
{
    /* ファイルの読み取り */
    float buf[10];      // 読み込み用のバッファ
    char readfile[100]; //
    sprintf(readfile, "%s/%s/43_PTV/%s_n=%d/PTV_velocity_dat/velocity.dat", dir_path, name, data_set, n2);
    cout << "READ FILE ... : n = " << n2 << endl;
    fp = fopen(readfile, "r");
    while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5])) != EOF)
    {
        y.at(n2 - n_min).push_back(buf[0]);
        z.at(n2 - n_min).push_back(buf[1]);
        v.at(n2 - n_min).push_back(buf[2]);
        w.at(n2 - n_min).push_back(buf[3]);
        vw.at(n2 - n_min).push_back(buf[4]);
        r.at(n2 - n_min).push_back(buf[5]);
    }
    fclose(fp);
}

/******************************************************************************
PROGRAM : plot_velocity
概要：
******************************************************************************/
void Plot_velocity(const char *name, const char *data_set)
{
    /** Gnuplot **/

    // ファイル名
    char filename[100];
    char graphname[100];
    char graphtitle[100];

    sprintf(filename, "%s/%s/44_Evaluation/%s/result/velocity.dat", dir_path, name, data_set);
    sprintf(graphname, "%s/%s/44_Evaluation/%s/result/velocity.svg", dir_path, name, data_set);

    printf("Plot Velocity : %s\n", filename);

    // 軸の設定

    // // range x
    const float x_min = 0.0;
    const float x_max = width_mm;

    // // range y
    const float y_min = 0.0;
    const float y_max = height_mm;

    // range color
    const float cb_min = 0;
    const float cb_max = 50;

    // label
    const char *xxlabel = "{/:Italic y} [mm]";
    const char *yylabel = "{/:Italic z} [mm] ";
    const char *cblabel = "Velocity [mm/s]]";

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

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 1.0, 0.0\n", yylabel);
    fprintf(gp, "set cblabel '%s' offset -0.5, 0.0\n", cblabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 10 offset 0.0, 0.0\n");
    fprintf(gp, "set ytics 10 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:2:3:4:5 with vectors head filled lc palette lw 1 notitle\n", filename);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}

/******************************************************************************
PROGRAM : plot_number
概要：
******************************************************************************/
void Plot_number(const char *name, const char *data_set)
{
    /** Gnuplot **/

    // ファイル名
    char filename_1[100];
    char filename_2[100];
    char graphname[100];
    char graphtitle[100];

    sprintf(filename_1, "%s/%s/44_Evaluation/%s/result/number.dat", dir_path, name, data_set);
    sprintf(filename_2, "%s/%s/44_Evaluation/%s/result/velocity.dat", dir_path, name, data_set);
    sprintf(graphname, "%s/%s/44_Evaluation/%s/result/number.svg", dir_path, name, data_set);

    printf("Plot Number : %s\n", filename_2);

    // 軸の設定

    // // range x
    const float x_min = 0.0;
    const float x_max = width_mm;

    // // range y
    const float y_min = 0.0;
    const float y_max = height_mm;

    // range color
    const int cb_min = n_min;
    const int cb_max = n_max;

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
    fprintf(gp, "set cbrange['%d':'%d']\n", cb_min, cb_max);
    fprintf(gp, "set cbtics 1\n");

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 1.5, 0.0\n", yylabel);
    fprintf(gp, "set cblabel '%s' offset -0.5, 0.0\n", cblabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 10 offset 0.0, 0.0\n");
    fprintf(gp, "set ytics 10 offset 1.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "set pm3d map\n");
    fprintf(gp, "splot '%s' using 1:2:3 with pm3d, '%s' using 1:2:(0):($3*2.0):($4*2.0):(0) with vectors head filled lt 2 lc 'black' lw 1 notitle\n", filename_1, filename_2);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}
