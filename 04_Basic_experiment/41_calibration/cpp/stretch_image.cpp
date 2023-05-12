/******************************************************************************
PROGRAM : stretch_image
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/17
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

// 自作設定ファイル
#include "../hpp/settings.hpp"
#include "../../parameters/parameters.hpp"
#include "../hpp/loadbmp_8bit.hpp"

/******************************************************************************/

double a[2], b[2], c[2], d[2], e[2], f[2], g[2], h[2], o[2], p[2];
double x, y;
int x_buf, y_buf;
double delta_x, delta_y;

// bmp 1次元配列
unsigned char binary[px_8_original];
unsigned char binary_stretch[px_8_stretch];
unsigned char binary_2d[width_original][height_original];
unsigned char binary_2d_new[width][height];

// gnuplot
char xxlabel[100] = "x [px]";
char yylabel[100] = "y [px]";
char zzlabel[2][100];
double x_max = width;
double x_min = 0;
double y_max = height;
double y_min = 0;
double cb_max = 150;
double cb_min = -50;

/******************************************************************************/

int main()
{

    /** ディレクトリの作成 **/
    sprintf(dirname[0], "%s/%s/stretch_image", dir_path, dataname);
    mkdir(dirname[0], dirmode);

    // 配列の初期化
    for (i = 0; i < px_8_original; i++)
    {
        binary[i] = 0;
    }

    for (i = 0; i < px_8_stretch; i++)
    {
        binary_stretch[i] = 0;
    }

    // 校正用画像の取得
    sprintf(filename[0], "%s/%s/binarization/8bit.bmp", dir_path, dataname);
    loadBmp_full_8bit(filename[0], header_8bit, binary);

    // 2次元配列への格納
    i = 0;
    j = 0;

    for (k = 0; k < px_8_original; k++)
    {
        if (i == width_original)
        {
            i = 0;
            j = j + 1;
        }

        binary_2d[i][j] = binary[k];
        i = i + 1;

        // printf("[%d][%d]\t2d = %d\tdata = %d\n", i, j, binary_2d[i][j], binary[k]);
    }

    // チェック（元画像の書き出し）

    // sprintf(filename[5], "%s/%s/binarization/8bit_test.bmp", dir_path, dataname);

    // unsigned char binary_buf[px_8_original];

    // k = 0;
    // for (j = 0; j < height_original; j++)
    // {
    //     for (i = 0; i < width_original; i++)
    //     {
    //         binary[k] = binary_2d[i][j];
    //         k = k + 1;
    //     }
    // }

    // fp = fopen(filename[5], "wb");

    // // ヘッダー情報の書き込み
    // fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

    // // 画像情報の書き込み
    // fwrite(binary, sizeof(unsigned char), px_8_original, fp);

    // fclose(fp);

    double distance; // 参照点の奥行間距離

    for (n = 0; n < 3; n++)
    {

        // キャリブレーション用の式の格納
        sprintf(filename[1], "%s/%s/curve_fit/curve_fit_%1.1fmm.dat", dir_path, dataname, 2.5 * n);
        printf("%s\n", filename[1]);

        i = 0;

        fp = fopen(filename[1], "r");

        while ((fscanf(fp, "%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf\t%lf", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5], &buf[6], &buf[7], &buf[8], &buf[9])) != EOF)
        {
            a[i] = buf[0];
            b[i] = buf[1];
            c[i] = buf[2];
            d[i] = buf[3];
            e[i] = buf[4];
            f[i] = buf[5];
            g[i] = buf[6];
            h[i] = buf[7];
            o[i] = buf[8];
            p[i] = buf[9];
            i = i + 1;
        }

        fclose(fp);

        // 変換後の座標書き出し用ファイル
        sprintf(filename[2], "%s/%s/stretch_image/coordinate_%1.1fmm.dat", dir_path, dataname, 2.5 * n);

        fp = fopen(filename[2], "w");

        // 参照位置の計算
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                // 元の画像における座標
                x = a[0] * i * i * i + b[0] * j * j * j + c[0] * i * i * j + d[0] * i * j * j + e[0] * i * i + f[0] * j * j + g[0] * i * j + h[0] * i + o[0] * j + p[0]; // x方向
                y = a[1] * i * i * i + b[1] * j * j * j + c[1] * i * i * j + d[1] * i * j * j + e[1] * i * i + f[1] * j * j + g[1] * i * j + h[1] * i + o[1] * j + p[1]; // y方向
                delta_x = x - i;
                delta_y = y - j;

                fprintf(fp, "%d\t%d\t%lf\t%lf\t%lf\t%lf\n", i, j, x, y, delta_x, delta_y);

                x_buf = x;
                y_buf = y;

                // 平均値の計算
                binary_2d_new[i][j] = (binary_2d[x_buf][y_buf] + binary_2d[x_buf + 1][y_buf] + binary_2d[x_buf][y_buf + 1] + binary_2d[x_buf + 1][y_buf + 1]) / 4;
            }

            fprintf(fp, "\n");
        }

        fclose(fp);

        // 一次元配列への格納
        k = 0;
        for (j = 0; j < height; j++)
        {
            for (i = 0; i < width; i++)
            {
                binary_stretch[k] = binary_2d_new[i][j];
                k = k + 1;
            }
        }

        // ヘッダの読み込み
        sprintf(filename[3], "header/%dx%d_8bit.bmp", width, height);
        loadBmp_header_8bit(filename[3], header_8bit);

        // 書き出すグラフの名前
        sprintf(filename[4], "%s/%s/stretch_image/coordinate_%1.1fmm.bmp", dir_path, dataname, 2.5 * n);

        fp = fopen(filename[4], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

        // 画像情報の書き込み
        fwrite(binary_stretch, sizeof(unsigned char), px_8_stretch, fp);

        fclose(fp);

        /** Gnuplot **/
        sprintf(zzlabel[0], "delta x [px]");
        sprintf(zzlabel[1], "delta y [px]");

        // Gnuplot 呼び出し
        if ((gp = popen("gnuplot", "w")) == NULL)
        {
            printf("gnuplot is not here!\n");
            exit(0); // gnuplotが無い場合、異常ある場合は終了
        }

        sprintf(graphtitle[0], "Correlation plane : %1.1f mm", n * 2.5);
        sprintf(graphname[0], "%s/%s/stretch_image/%1.1fmm_x.png", dir_path, dataname, n * 2.5);
        sprintf(graphname[1], "%s/%s/stretch_image/%1.1fmm_y.png", dir_path, dataname, n * 2.5);

        sprintf(filename[5], "%s/%s/get_peaks/peak_positions_%1.1fmm.dat", dir_path, dataname, 2.5 * n);

        fprintf(gp, "set terminal pngcairo enhanced size 800, 800 font 'Times New Roman, 16'\n");
        fprintf(gp, "set size ratio -1\n");

        // カラーマップの設定
        fprintf(gp, "set pm3d\n");

        // 非表示
        fprintf(gp, "unset key\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
        fprintf(gp, "set zrange [%.3f:%.3f]\n", cb_min, cb_max);

        // グラフタイトル
        fprintf(gp, "set title '%s' offset 0.0, -1.0\n", graphtitle[0]);

        // fprintf(gp, "set palette rgb 22,13,-31\n");

        fprintf(gp, "set palette defined(-3 'blue', 0 'white', 1 'red')\n");

        // 軸ラベル
        fprintf(gp, "set xlabel '%s'\n", xxlabel);
        fprintf(gp, "set ylabel '%s'\n", yylabel);

        // 軸のラベル位置
        fprintf(gp, "set xlabel offset -1.0, -1.0\n");
        fprintf(gp, "set ylabel offset 1.0, -1.0\n");

        // 軸の数値位置
        fprintf(gp, "set xtics offset -1.0, -1.0\n");
        fprintf(gp, "set ytics offset 1.0, -1.0\n");

        // 出力ファイルの設定とグラフの出力

        // 色付け
        fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);
        fprintf(gp, "set colorbox user origin 0.9, 0.4 size 0.03, 0.45\n");

        fprintf(gp, "set ztics 50\n");
        fprintf(gp, "set output '%s'\n", graphname[0]);
        fprintf(gp, "splot '%s' using 1:2:5 with pm3d lc palette title '%s', '%s' using 3:4:5 with points lc 'green' ps 1.5 pt 7 title 'Points'\n", filename[2], zzlabel[0], filename[5]);

        fprintf(gp, "set ztics 50\n");
        fprintf(gp, "set output '%s'\n", graphname[1]);
        fprintf(gp, "splot '%s' using 1:2:6 with pm3d lc palette title '%s', '%s' using 3:4:6 with points lc 'green' ps 1.5 pt 7 title 'Points'\n", filename[2], zzlabel[1], filename[5]);

        fflush(gp); // Clean up Data

        fprintf(gp, "exit\n"); // Quit gnuplot

        pclose(gp);
    }

    // 重ね合わせたグラフの表示

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    sprintf(graphtitle[0], "Correlation plane : ALL");
    sprintf(graphname[0], "%s/%s/stretch_image/all_x.png", dir_path, dataname);
    sprintf(graphname[1], "%s/%s/stretch_image/all_y.png", dir_path, dataname);

    sprintf(filename[6], "%s/%s/stretch_image/coordinate_%1.1fmm.dat", dir_path, dataname, 0.0);
    sprintf(filename[7], "%s/%s/stretch_image/coordinate_%1.1fmm.dat", dir_path, dataname, 2.5);
    sprintf(filename[8], "%s/%s/stretch_image/coordinate_%1.1fmm.dat", dir_path, dataname, 5.0);

    fprintf(gp, "set terminal pngcairo enhanced size 800, 800 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio -1\n");

    // カラーマップの設定
    fprintf(gp, "set pm3d\n");

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
    fprintf(gp, "set zrange [%.3f:%.3f]\n", cb_min, cb_max);

    // グラフタイトル
    fprintf(gp, "set title '%s' offset 0.0, -1.0\n", graphtitle[0]);

    // fprintf(gp, "set palette rgb 22,13,-31\n");

    fprintf(gp, "set palette defined(-3 'blue', 0 'white', 1 'red')\n");

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset -1.0, -1.0\n");
    fprintf(gp, "set ylabel offset 1.0, -1.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset -1.0, -1.0\n");
    fprintf(gp, "set ytics offset 1.0, -1.0\n");

    // 出力ファイルの設定とグラフの出力

    // 色付け
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);
    fprintf(gp, "set colorbox user origin 0.9, 0.4 size 0.03, 0.45\n");

    fprintf(gp, "set ztics 50\n");
    fprintf(gp, "set output '%s'\n", graphname[0]);
    fprintf(gp, "splot '%s' using 1:2:5 with pm3d lc palette notitle, '%s' using 1:2:5 with pm3d lc palette notitle, '%s' using 1:2:5 with pm3d lc palette notitle\n", filename[6], filename[7], filename[8]);

    fprintf(gp, "set ztics 50\n");
    fprintf(gp, "set output '%s'\n", graphname[1]);
    fprintf(gp, "splot '%s' using 1:2:6 with pm3d lc palette notitle, '%s' using 1:2:6 with pm3d lc palette notitle, '%s' using 1:2:6 with pm3d lc palette notitle \n", filename[6], filename[7], filename[8]);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);

    return 0;
}