/******************************************************************************
PROGRAM : Velocity
AUTHER  : Masatsugu Kitadai
DATE    : 2023/05/24
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

/** プロトタイプ宣言 **/
void gnuplot_1(const char data_name[]);

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/******************************************************************************/

int main()
{
    const char data_1[] = "velocity_correct";
    const char data_2[] = "velocity_10-50";
    const char data_3[] = "velocity_10-500";
    const char data_4[] = "velocity_10-1000";
    gnuplot_1(data_1);
    gnuplot_1(data_2);
    gnuplot_1(data_3);
    gnuplot_1(data_4);

    return 0;
}

void gnuplot_1(const char data_name[])
{
    /** Gnuplot **/

    // ファイル名
    char readfile[100];
    char graphfile[100];
    sprintf(readfile, "data/%s.dat", data_name);
    sprintf(graphfile, "results/%s.svg", data_name);

    printf("READ FILE   : %s\n", readfile);
    printf("GRAPH FILE  : %s\n", graphfile);

    // 軸の設定

    // ベクトルの倍率
    const float times = 2.0;

    // range x
    const float x_min = 40 + 2.5;
    const float x_max = 60 - 2.5;

    // range y
    const float y_min = 40 + 2.5;
    const float y_max = 60 - 2.5;

    // range color
    float cb_min = 0;
    float cb_max = 2.5;

    // label
    const char *xxlabel = "{/:Italic y} [mm]";
    const char *yylabel = "{/:Italic z} [mm]";
    const char *cblabel = "Velocity [mm/s]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 500, 500 font 'Times New Roman, 22'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.0f'\n");
    fprintf(gp, "set format cb '%%.1f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 1.0, 0.0\n", yylabel);
    fprintf(gp, "set cblabel '%s' offset 0.0, 0.0\n", cblabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 2 offset 0.0, 0.2\n");
    fprintf(gp, "set ytics 2 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:2:($3*%f):($4*%f):5 with vectors head filled lc palette lw 1 notitle\n", readfile, times, times);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}