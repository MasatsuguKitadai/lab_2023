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
void gnuplot_1(char readfile[], char graphfile[]);
void gnuplot_2();

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

// ファイル名
const char lls_1[] = "data/lls_1.dat";
const char lls_2[] = "data/lls_2.dat";
const char graphtitle[] = "Positions of tracer particle";

/******************************************************************************/

int main()
{
    char graphfile[] = "results/particle_position.svg";
    char readfile[] = "data/particle_position.dat";
    gnuplot_1(readfile, graphfile);

    for (int i = 1; i < 100; i++)
    {
        char readfile_plot[100];
        char graphfile_plot[100];
        sprintf(readfile_plot, "data/data_3d/%d.dat", i);
        sprintf(graphfile_plot, "data/data_3d/%d.svg", i + 100);
        gnuplot_1(readfile_plot, graphfile_plot);
    }

    return 0;
}

void gnuplot_1(char readfile[], char graphfile[])
{
    /** Gnuplot **/

    printf("READ FILE   : %s\n", readfile);
    printf("GRAPH FILE  : %s\n", graphfile);
    printf("GRAPH TITLE : %s\n", graphtitle);

    // 軸の設定

    // range x
    const float x_min = 7.0 - 0.025;
    const float x_max = 7.0 + 0.025;

    // range y
    const float y_min = 20 + 1;
    const float y_max = 80 - 1;

    // range z
    const float z_min = 20 + 1;
    const float z_max = 80 - 1;

    // label
    const char *xxlabel = "{/:Italic x} [mm]";
    const char *yylabel = "{/:Italic y} [mm]";
    const char *zzlabel = "{/:Italic z} [mm]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg size 520, 520 font 'Times New Roman, 20'\n");
    // fprintf(gp, "set terminal png size 500, 500 font 'Times New Roman, 18'\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // グラフのマージン
    fprintf(gp, "set lmargin 5\n");
    fprintf(gp, "set size ratio 1\n");

    // 非表示
    fprintf(gp, "unset key\n");

    // 視点の変更
    fprintf(gp, "set view 60, 30, 1, 1\n");

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.2f'\n");
    fprintf(gp, "set format y '%%.1f'\n");
    fprintf(gp, "set format z '%%.1f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
    fprintf(gp, "set zrange [%.3f:%.3f]\n", z_min, z_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);
    fprintf(gp, "set zlabel '%s' rotate by 90\n", zzlabel);

    // Set margin
    fprintf(gp, "set lmargin screen 0.25\n"); // <steps in scan>,<steps between scans>
    fprintf(gp, "set rmargin screen 0.80\n"); // <steps in scan>,<steps between scans>
    fprintf(gp, "set tmargin screen 0.80\n"); // <steps in scan>,<steps between scans>
    fprintf(gp, "set bmargin screen 0.25\n"); // <steps in scan>,<steps between scans>

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 4.0, 0.5\n");
    fprintf(gp, "set ylabel offset -4.5, 0.5\n");
    fprintf(gp, "set zlabel offset 3.0, 0.0\n");
    fprintf(gp, "set ticslevel 0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics 0.01 offset 0.0, -0.2 font 'Times New Roman, 16'\n");
    fprintf(gp, "set ytics 20 offset 1.0, 0.0    font 'Times New Roman, 16'\n");
    fprintf(gp, "set ztics 20 offset 1.5, 0.0    font 'Times New Roman, 16'\n");

    // グラフの出力
    fprintf(gp, "splot '%s' using 1:2:3 with points lc 'grey50' ps 0.2 pt 7 notitle, ", readfile); // 粒子位置の描画
    fprintf(gp, "'%s' using 1:2:3 with lines lw 3 lc 'blue' notitle, ", lls_1);                    // LLS(1) の描画
    fprintf(gp, "'%s' using 1:2:3 with lines lw 3 lc 'green' notitle\n", lls_2);                   // LLS(2) の描画

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}