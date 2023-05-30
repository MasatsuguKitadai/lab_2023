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
void gnuplot_1();
void gnuplot_2();

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/******************************************************************************/

int main()
{
    gnuplot_1();
    gnuplot_2();

    return 0;
}

void gnuplot_1()
{
    /** Gnuplot **/

    // ファイル名
    char readfile[] = "data/delta_001.dat";
    char graphfile[] = "results/velocity_delta.svg";
    char graphtitle[] = "Time averaged velocity : Wake of delta wing model";

    printf("READ FILE   : %s\n", readfile);
    printf("GRAPH FILE  : %s\n", graphfile);
    printf("GRAPH TITLE : %s\n", graphtitle);

    // 軸の設定

    // // range x
    const float x_min = 20;
    const float x_max = 80;

    // // range y
    const float y_min = 0;
    const float y_max = 30;

    // range color
    float cb_min = 0;
    float cb_max = 50;

    // label
    const char *xxlabel = "y [mm]";
    const char *yylabel = "z [mm]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 1200, 700 font 'Times New Roman, 34'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s' offset 0.0, -0.5\n", graphtitle);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 1.0, 0.0\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 0.0, 1.0\n", yylabel);
    fprintf(gp, "set cblabel 'velocity [mm/s]' offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics 10 offset 0.0, 0.2\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:2:3:4:5 with vectors lc palette lw 1 notitle\n", readfile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}

void gnuplot_2()
{
    /** Gnuplot **/

    // ファイル名
    char readfile[] = "data/tyre_001.dat";
    char graphfile[] = "results/velocity_tyre.svg";
    char graphtitle[] = "Time averaged velocity : Wake of vehicle model";

    printf("READ FILE   : %s\n", readfile);
    printf("GRAPH FILE  : %s\n", graphfile);
    printf("GRAPH TITLE : %s\n", graphtitle);

    // 軸の設定

    // // range x
    const float x_min = 0;
    const float x_max = 30;

    // // range y
    const float y_min = 90;
    const float y_max = 30;

    // range color
    float cb_min = 0;
    float cb_max = 50;

    // label
    const char *xxlabel = "z [mm]";
    const char *yylabel = "y [mm]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 700, 1000 font 'Times New Roman, 30'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s' offset 0.0, -0.5\n", graphtitle);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 1.0, 0.0\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 0.0, 1.0\n", yylabel);
    fprintf(gp, "set cblabel 'velocity [mm/s]' offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics 10 offset 0.0, 0.2\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 2:1:4:3:5 with vectors lc palette lw 1 notitle\n", readfile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}