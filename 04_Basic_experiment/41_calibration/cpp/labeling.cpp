/******************************************************************************
PROGRAM : labeling
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/15
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
    sprintf(dirname[0], "%s/%s/labeling", dir_path, dataname);
    mkdir(dirname[0], dirmode);

    sprintf(dirname[0], "%s/%s/labeling/dat", dir_path, dataname);
    mkdir(dirname[0], dirmode);

    sprintf(dirname[0], "%s/%s/labeling/png", dir_path, dataname);
    mkdir(dirname[0], dirmode);

    // 8bit.bmp
    unsigned char ary_calibration[px_8_original];
    int ary_label[px_8_original];
    int ary_buf[20];

    double sum[2];
    double ave[2];
    int position;
    int area[1000];
    int count = 0;
    int min;
    int tmp = 0;

    // 校正画像の読み込み
    sprintf(filename[1], "%s/%s/binarization/8bit.bmp", dir_path, dataname);
    loadBmp_full_8bit(filename[1], header_8bit, ary_calibration);

    /** ラベリング **/

    int num = 0; // ラベリングの番号
    int label = 0;

    // 配列の可視化
    for (i = 0; i < px_8_original; i++)
    {
        ary_label[i] = 0;
    }

    for (i = 0; i < 100; i++)
    {
        buf[i] = 0;
    }

    for (i = 0; i < 1000; i++)
    {
        area[i] = 0;
    }

    // ラベル番号の初期化
    n = 0;

    for (i = 0; i < px_8_original; i++)
    {
        // 変数の初期化
        label = 0;
        tmp = 0;

        if (ary_calibration[i] != 0)
        {
            // エリア内のラベルの確認
            for (j = -2; j < 0; j++)
            {
                for (k = -2; k < 2; k++)
                {
                    // 調べる座標の計算
                    position = i + width_original * j + k;

                    // 座標のオーバーフローの回避
                    if (position < 0)
                    {
                        position = 0;
                    }
                    else if (position > px_8_original)
                    {
                        position = px_8_original;
                    }

                    // 最小ラベルの取得
                    if (ary_label[position] != 0)
                    {
                        tmp = ary_label[position];

                        // 値が入っていない場合はどんな値でも格納
                        if (label == 0)
                        {
                            label = tmp;
                        }

                        // 最小のラベルへの入れ替え
                        if (tmp < label)
                        {
                            label = tmp;
                        }
                    }
                }
            }

            if (label != 0)
            {
                // ラベル番号の付与
                ary_label[i] = label;

                // ラベルの張替え
                for (j = -2; j < 0; j++)
                {
                    for (k = -2; k < 2; k++)
                    {
                        // 調べる座標の計算
                        position = i + width_original * j + k;

                        // 座標のオーバーフローの回避
                        if (position < 0)
                        {
                            position = 0;
                        }
                        else if (position > px_8_original)
                        {
                            position = px_8_original;
                        }

                        // 張替え
                        if (ary_label[position] > label)
                        {
                            ary_label[position] = label;
                        }
                    }
                }
            }
            else
            {
                // 新たなラベル番号の付与
                n = n + 1;
                ary_label[i] = n;
            }
        }
    }

    // 面積を用いた誤認識の回避

    // 面積の計算
    for (i = 0; i < px_8_original; i++)
    {
        label = ary_label[i];

        for (j = 0; j < n; j++)
        {
            if (label == j)
            {
                area[j] = area[j] + 1;
            }
        }
    }

    // カウント用メモリの初期化
    j = 1;

    // 閾値より高い面積のラベルを取得
    for (i = 1; i < n; i++)
    {
        if (area[i] > 20)
        {
            buf[j] = i;
            j = j + 1;
        }
    }

    // 移し替え
    count = j;

    // ラベルの再張替え
    for (i = 1; i < count; i++)
    {
        for (j = 0; j < px_8_original; j++)
        {
            if (ary_label[j] == buf[i])
            {
                ary_label[j] = i;
            }
        }
    }

    // 配列の初期化
    for (i = 0; i < 1000; i++)
    {
        area[i] = 0;
    }

    // 面積の再計算
    for (i = 0; i < px_8_original; i++)
    {
        for (j = 0; j < n; j++)
        {
            if (ary_label[i] == j)
            {
                area[j] = area[j] + 1;
            }
        }
    }

    // n の再カウント
    tmp = 0;

    for (i = 1; i < n; i++)
    {
        if (area[i] > 40)
        {
            // 面積を持つ数の確認
            tmp = tmp + 1;
            // printf("area[%d] = %d\n", i, area[i]);
        }
    }

    // n の書き換え
    n = tmp;
    printf("n = %d\n", n);

    /** ファイルの書き出しと描画 **/

    // 軸の設定

    // range x
    double x_min = 0;
    double x_max = width_original;

    // range y
    double y_min = 0;
    double y_max = height_original;

    // label
    const char *xxlabel = "x [px]";
    const char *yylabel = "y [px]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    // グラフの背景
    sprintf(filename[2], "%s/%s/binarization/8bit.png", dir_path, dataname);

    for (m = 1; m <= n; m++)
    {
        // ファイルの書き出し
        sprintf(filename[3], "%s/%s/labeling/dat/labeling_%d.dat", dir_path, dataname, m);
        fp = fopen(filename[3], "w");

        for (j = 0; j < height_original; j++)
        {
            for (i = 0; i < width_original; i++)
            {
                position = width_original * j + i; // 調べる座標

                if (ary_label[position] == m)
                {
                    fprintf(fp, "%d\t%d\n", i, j);
                }
            }
        }

        fclose(fp);

        /** Gnuplot **/

        sprintf(graphname[0], "%s/%s/labeling/png/labeling_%d.png", dir_path, dataname, m);
        sprintf(graphtitle[0], "labeling plane [%2d]", m);

        fprintf(gp, "set terminal pngcairo enhanced size 1000, 800 font 'Times New Roman, 20'\n");
        fprintf(gp, "set size ratio -1\n");

        // 出力ファイル
        fprintf(gp, "set output '%s'\n", graphname[0]);

        // 非表示
        fprintf(gp, "unset key\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

        // グラフタイトル
        fprintf(gp, "set title '%s'\n", graphtitle[0]);

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
        fprintf(gp, "plot '%s' binary filetype=png with rgbimage, '%s' using 1:2 with points  lc 'red' pt 5 ps 0.5 notitle\n", filename[2], filename[3]);
        // fprintf(gp, "splot '%s' with pm3d lc palette notitle\n", filename);
        fflush(gp); // Clean up Data

        printf("in progress...\t%d\n", m);
    }

    // 一度に排出
    sprintf(filename[3], "%s/%s/labeling/labeling_gnuplot.dat", dir_path, dataname);
    fp = fopen(filename[3], "w");

    // 各座標ごとに書き出し
    sprintf(filename[4], "%s/%s/labeling/labeling.dat", dir_path, dataname);
    fp_2 = fopen(filename[4], "w");

    for (j = 0; j < height_original; j++)
    {
        for (i = 0; i < width_original; i++)
        {
            position = width_original * j + i; // 調べる座標

            if (ary_label[position] <= n)
            {
                fprintf(fp_2, "%d\t%d\t%d\n", i, j, ary_label[position]);
            }
            else
            {
                fprintf(fp_2, "%d\t%d\t0\n", i, j);
            }

            if (ary_label[position] <= n && ary_label[position] != 0)
            {
                fprintf(fp, "%d\t%d\t%d\n", i, j, ary_label[position]);
            }
        }

        fprintf(fp_2, "\n");
    }

    fclose(fp);
    fclose(fp_2);

    /** Gnuplot **/

    sprintf(graphname[0], "%s/%s/labeling/labeling.png", dir_path, dataname);
    sprintf(graphtitle[0], "labeling plane");

    fprintf(gp, "set terminal pngcairo enhanced size 1000, 800 font 'Times New Roman, 20'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname[0]);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s'\n", graphtitle[0]);

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
    fprintf(gp, "plot '%s' binary filetype=png with rgbimage, '%s' using 1:2 with points  lc 'red' pt 5 ps 0.5 notitle\n", filename[2], filename[3]);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot
    pclose(gp);

    return 0;
}