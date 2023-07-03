/******************************************************************************
PROGRAM : error
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
void gnuplot_3();
void gnuplot_4();

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/******************************************************************************/

int main()
{
    gnuplot_1();
    gnuplot_2();
    gnuplot_3();
    gnuplot_4();

    return 0;
}

void gnuplot_1()
{
    /** Gnuplot **/

    // ファイル名
    char readfile[] = "data/error_1.dat";
    char graphfile[] = "results/error_1.svg";
    char graphtitle[] = "Rotation speed : {/Symbol w} = 10.0 × π / 180 [rad/s]";

    printf("READ FILE   : %s\n", readfile);
    printf("GRAPH FILE  : %s\n", graphfile);

    // 軸の設定

    // range x
    const float x_min = 25;
    const float x_max = 525;

    // range y
    const float y_min = 1.0;
    const float y_max = 7.0;

    // label
    const char *xxlabel = "Number of blue particles in one image : {/:Italic n_p} [-/frame]";
    const char *yylabel = "RMSE ratio : {/:Italic E} [%]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 500, 500 font 'Times New Roman, 18'\n");
    fprintf(gp, "set size ratio 0.8\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");
    // fprintf(gp, "set title '%s' offset 0.0, -0.5\n", graphtitle);

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.0f'\n");
    fprintf(gp, "set format cb '%%.1f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 0.5, 0.0\n", yylabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 50 offset 0.0, 0.2\n");
    fprintf(gp, "set ytics 1 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:4 with points pt 7 ps 0.5 lc 'black' notitle, '%s' using 1:4 with lines lw 0.5 lc 'black' notitle\n", readfile, readfile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}

void gnuplot_2()
{
    /** Gnuplot **/

    // ファイル名
    char readfile_1[] = "data/error_2-50.dat";
    char readfile_2[] = "data/error_2-300.dat";
    char graphfile[] = "results/error_2.svg";
    char graphtitle[] = "Number of blue particles in one image : {/:Italic n} = 50 [-/frame]";

    printf("READ FILE (1) : %s\n", readfile_1);
    printf("READ FILE (2) : %s\n", readfile_2);
    printf("GRAPH FILE    : %s\n", graphfile);

    // 軸の設定

    // range x
    const float x_min = 5.5;
    const float x_max = 14.5;

    // range y
    const float y_min = 1.0;
    const float y_max = 7.0;

    // label
    const char *xxlabel = "Rotational speed : {/Symbol w} × 180 / {/Symbol p} [deg/s]";
    const char *yylabel = "RMSE ratio : {/:Italic E}  [%]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 500, 500 font 'Times New Roman, 18'\n");
    fprintf(gp, "set size ratio 0.8\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");
    // fprintf(gp, "set key top outside font 'Times New Roman, 10'\n");
    // fprintf(gp, "set key right top\n");
    // fprintf(gp, "set title '%s' offset 0.0, -0.5\n", graphtitle);

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.0f'\n");
    fprintf(gp, "set format cb '%%.1f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 0.5, 0.0\n", yylabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 1 offset 0.0, 0.2\n");
    fprintf(gp, "set ytics 1 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:4 with points pt 7 ps 0.5 lc 'red' title '{/:Italic n_p} =   50', '%s' using 1:4 with lines lw 0.5 lc 'red' notitle, ", readfile_1, readfile_1);
    fprintf(gp, "'%s' using 1:4 with points pt 7 ps 0.5 lc 'blue' title '{/:Italic n_p} = 300', '%s' using 1:4 with lines lw 0.5 lc 'blue' notitle\n", readfile_2, readfile_2);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}

void gnuplot_3()
{
    /** Gnuplot **/

    // ファイル名
    char readfile[] = "data/error_2-50.dat";
    char graphfile[] = "results/error_3.svg";
    char graphtitle[] = "Number of blue particles in one image : {/:Italic n} = 50 [-/frame]";

    printf("READ FILE (1) : %s\n", readfile);
    printf("GRAPH FILE    : %s\n", graphfile);

    // 軸の設定

    // range x
    const float x_min = 5.5;
    const float x_max = 14.5;

    // range y
    const float y_min = 1.0;
    const float y_max = 4.0;

    // label
    const char *xxlabel = "Rotational speed : {/Symbol w} × 180 / {/Symbol p} [deg/s]";
    const char *yylabel = "RMSE ratio : {/:Italic E}  [%]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 500, 500 font 'Times New Roman, 18'\n");
    fprintf(gp, "set size ratio 0.8\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.1f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 0.5, 0.0\n", yylabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 1 offset 0.0, 0.2\n");
    fprintf(gp, "set ytics 0.5 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:4 with points pt 7 ps 0.5 lc 'red' notitle, '%s' using 1:4 with lines lw 0.5 lc 'red' notitle\n ", readfile, readfile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}

void gnuplot_4()
{
    /** Gnuplot **/

    // ファイル名
    char readfile[] = "data/error_2-300.dat";
    char graphfile[] = "results/error_4.svg";
    char graphtitle[] = "Number of blue particles in one image : {/:Italic n} = 50 [-/frame]";

    printf("READ FILE (1) : %s\n", readfile);
    printf("GRAPH FILE    : %s\n", graphfile);

    // 軸の設定

    // range x
    const float x_min = 5.5;
    const float x_max = 14.5;

    // range y
    const float y_min = 4.0;
    const float y_max = 7.0;

    // label
    const char *xxlabel = "Rotational speed : {/Symbol w} × 180 / {/Symbol p} [deg/s]";
    const char *yylabel = "RMSE ratio : {/:Italic E}  [%]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 500, 500 font 'Times New Roman, 18'\n");
    fprintf(gp, "set size ratio 0.8\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.1f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 0.5, 0.0\n", yylabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 1 offset 0.0, 0.2\n");
    fprintf(gp, "set ytics 0.5 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:4 with points pt 7 ps 0.5 lc 'blue' notitle, '%s' using 1:4 with lines lw 0.5 lc 'blue' notitle\n ", readfile, readfile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}