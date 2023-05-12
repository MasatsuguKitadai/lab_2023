/******************************************************************************
PROGRAM : correlation
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/14
******************************************************************************/

// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

// 自作設定ファイル
#include "../hpp/settings.hpp"
#include "../../parameters/parameters.hpp"
#include "../hpp/loadbmp_8bit.hpp"

/******************************************************************************/

int main()
{
    // ディレクトリの作成
    sprintf(dirname[0], "%s/%s/correlation", dir_path, dataname);
    mkdir(dirname[0], dirmode);

    /** (1) 円形画像の読み込み ****************************************************************************/

    // dot.bmp
    int size = 40;
    int px_dot = size * size;
    unsigned char ary_dot[px_dot];

    // 参照dot画像の読み込み
    // sprintf(filename[0], "dot/%dx%d.bmp", interr_size, interr_size);
    sprintf(filename[0], "dot/dot.bmp");
    loadBmp_full_8bit(filename[0], header_8bit, ary_dot);

    /** (2) 校正用画像の読み込み と 相関係数の計算 ****************************************************************************/

    // 8bit.bmp
    unsigned char ary_calibration[px_8_original];
    double sum[2];
    double ave[2];
    double R = 0;
    int x, y;
    int position[2];

    // 校正画像の読み込みde
    sprintf(filename[1], "%s/%s/binarization/8bit.bmp", dir_path, dataname);
    loadBmp_full_8bit(filename[1], header_8bit, ary_calibration);

    /** 相互相関係数の算出 *********************************************************************/

    // 配列の初期化
    for (i = 0; i < 2; i++)
    {
        sum[i] = 0;
        ave[i] = 0;
    }

    // 濃度値の総和
    for (i = 0; i < px_dot; i++)
    {
        sum[0] = ary_dot[i] + sum[0];
    }

    // 濃度値の平均
    ave[0] = sum[0] / px_dot;

    /** 相関係数の算出準備 (1) **/

    // 相互相関平面の書き出し
    sprintf(filename[2], "%s/%s/correlation/correlation.dat", dir_path, dataname);
    fp = fopen(filename[2], "w");

    for (y = 0; y < height_original - interr_size; y++)
    {
        for (x = 0; x < width_original - interr_size; x++)
        {
            sum[1] = 0;

            // 合計値・平均値の算出
            for (j = 0; j < interr_size; j++)
            {
                for (i = 0; i < interr_size; i++)
                {
                    position[0] = width_original * (y + j) + (x + i);
                    sum[1] = ary_calibration[position[0]] + sum[1];
                }
            }

            ave[1] = sum[1] / (interr_size * interr_size);

            // 相関係数 R の算出

            // 配列の初期化
            for (i = 0; i < 10; i++)
            {
                cal[i] = 0;
            }

            for (i = 0; i < interr_size; i++)
            {
                for (j = 0; j < interr_size; j++)
                {
                    position[0] = interr_size * j + i;                // dot 画像の位置
                    position[1] = width_original * (y + j) + (x + i); // 校正画像の位置

                    cal[0] = ary_dot[position[0]] - ave[0];
                    cal[1] = ary_calibration[position[1]] - ave[1];
                    cal[2] = cal[0] * cal[1] + cal[2]; // 分子
                    cal[3] = cal[0] * cal[0] + cal[3]; // 分母（乗数）
                    cal[4] = cal[1] * cal[1] + cal[4]; // 分母（乗数）
                }
            }

            cal[5] = sqrt(cal[3]);
            cal[6] = sqrt(cal[4]);

            if (cal[5] * cal[6] == 0)
            {
                R = 0;
            }
            else
            {
                R = cal[2] / (cal[5] * cal[6]);
            }

            fprintf(fp, "%d\t%d\t%lf\n", x + interr_size / 2, y + interr_size / 2, R);
        }

        fprintf(fp, "\n");
    }

    fclose(fp);

    /** Gnuplot **/

    sprintf(graphname[0], "%s/%s/correlation/correlation.png", dir_path, dataname);
    sprintf(graphtitle[0], "Correlation plane");

    // 軸の設定

    // range x
    double x_min = 0;
    double x_max = width_original;

    // range y
    double y_min = 0;
    double y_max = height_original;

    // range color
    double cb_min = -1;
    double cb_max = 1;

    // label
    const char *xxlabel = "x [px]";
    const char *yylabel = "y [px]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal pngcairo enhanced size 1000, 800 font 'Times New Roman, 20'\n");
    // fprintf(gp, "set terminal svg enhanced size 1000, 1000 font 'Times New Roman, 24' \n");
    fprintf(gp, "set size ratio -1\n");

    // 描画マージン
    // fprintf(gp, "set lmargin screen 0.15\n");
    // fprintf(gp, "set rmargin screen 0.85\n");
    // fprintf(gp, "set tmargin screen 0.85\n");
    // fprintf(gp, "set bmargin screen 0.15\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname[0]);

    // 非表示
    fprintf(gp, "unset key\n");

    fprintf(gp, "set pm3d map\n");
    fprintf(gp, "unset ztics \n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s'\n", graphtitle[0]);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.0\n");
    fprintf(gp, "set ylabel offset -1.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.0\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "splot '%s' using 1:2:3 with pm3d lc palette notitle\n", filename[2]);
    // fprintf(gp, "splot '%s' with pm3d lc palette notitle\n", filename);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot
    pclose(gp);

    return 0;
}