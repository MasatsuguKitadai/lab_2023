/******************************************************************************
PROGRAM : labeling_for_ptv
AUTHER  : Masatsugu Kitadai
DATE    : 2022/12/1
******************************************************************************/

// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

// 自作設定ファイル
#include "../hpp/settings.hpp"
#include "../../parameters.hpp"
#include "../hpp/loadbmp_8bit.hpp"

/******************************************************************************/

int main()
{
    // 8bit.bmp
    unsigned char ary[px_8_stretch];
    int ary_label[px_8_stretch];
    int ary_buf[20];

    float sum[2];
    float ave[2];
    int position;
    int area[1000];
    int count = 0;
    int min;
    int tmp = 0;

    int maximum_x[1000], maximum_y[1000];
    int minimum_x[1000], minimum_y[1000];

    // 最大・最小の座標
    float point_x[4][1000];
    float point_y[4][1000];

    // 中心座標
    int x[1000], y[1000];

    // 軸の設定

    // range x
    float x_min = 0;
    float x_max = width;

    // range y
    float y_min = 0;
    float y_max = height;

    // label
    const char *xxlabel = "x [px]";
    const char *yylabel = "y [px]";

    int num = 0; // ラベリングの番号
    int label = 0;

    for (int m = 1; m <= number; m++)
    {
        // 粒子画像の読み込み
        sprintf(filename[1], "result/02/stretch/blue/%d.bmp", m);
        loadBmp_full_8bit(filename[1], header_8bit, ary);

        /** ラベリング **/

        // 配列の可視化
        for (int i = 0; i < px_8_stretch; i++)
        {
            ary_label[i] = 0;
        }

        for (int i = 0; i < 100; i++)
        {
            buf[i] = 0;
        }

        for (int i = 0; i < 1000; i++)
        {
            area[i] = 0;
        }

        // ラベル番号の初期化
        int n = 0;

        for (int i = 0; i < px_8_stretch; i++)
        {
            // 変数の初期化
            label = 0;
            tmp = 0;

            if (ary[i] >= 20)
            // if (ary[i] != 0)
            {
                // エリア内のラベルの確認
                for (int j = -1; j <= 0; j++)
                {
                    for (int k = -1; k <= 1; k++)
                    {
                        // 調べる座標の計算
                        position = i + width * j + k;

                        // 座標のオーバーフローの回避
                        if (position < 0)
                        {
                            position = 0;
                        }
                        else if (position > px_8_stretch)
                        {
                            position = px_8_stretch;
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
                    for (int j = -1; j <= 0; j++)
                    {
                        for (int k = -1; k <= 1; k++)
                        {
                            // 調べる座標の計算
                            position = i + width * j + k;

                            // 座標のオーバーフローの回避
                            if (position < 0)
                            {
                                position = 0;
                            }
                            else if (position > px_8_stretch)
                            {
                                position = px_8_stretch;
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

        // 面積の計算
        for (int i = 0; i < px_8_stretch; i++)
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
        int count_1 = 1;
        int count_2 = 0;

        // 不必要なラベル番号
        int trash[100];

        // 配列初期化
        for (int i = 0; i < 100; i++)
        {
            trash[i] = 0;
        }

        // 閾値より高い面積のラベルを取得
        for (int i = 1; i <= n; i++)
        {
            if (area[i] > 5)
            {
                buf[count_1] = i;
                count_1 += 1;
            }
            else
            {
                trash[count_2] = i;
                count_2 += 1;
            }
        }

        // 移し替え
        count = count_1;

        // 必要ないラベル番号を排除
        for (int i = 0; i < count_2; i++)
            for (int j = 0; j < px_8_stretch; j++)
            {
                if (trash[i] == ary_label[j])
                {
                    ary_label[j] = 0;
                }
            }

        // ラベルの再張替え
        for (int i = 1; i < count; i++)
            for (int j = 0; j < px_8_stretch; j++)
            {
                if (ary_label[j] == buf[i])
                {
                    ary_label[j] = i;
                }
            }

        // 配列の初期化
        for (int i = 0; i < 1000; i++)
        {
            area[i] = 0;
        }

        // 面積の再計算
        for (int i = 0; i < px_8_stretch; i++)
            for (int j = 0; j < n; j++)
            {
                if (ary_label[i] == j)
                {
                    area[j] = area[j] + 1;
                }
            }

        // n の再カウント
        tmp = 0;

        for (int i = 1; i <= n; i++)
        {
            if (area[i] > 5)
            {
                // 面積を持つ数の確認
                tmp = tmp + 1;
                // printf("area[%d] = %d\n", i, area[i]);
            }
        }

        // n の書き換え
        n = tmp;

        // 配列の初期化
        for (int i = 0; i < 1000; i++)
        {
            x[i] = 0;
            y[i] = 0;
        }

        float x_tmp, y_tmp;

        /** 配列への格納 **/
        for (int i = 1; i <= n; i++)
        {
            float x_sum = 0;
            float y_sum = 0;
            int count_label = 0;

            for (int j = 0; j < px_8_stretch; j++)
            {
                if (i == ary_label[j])
                {
                    x_tmp = j % width;
                    y_tmp = j / width;

                    // 合計
                    x_sum = x_sum + x_tmp;
                    y_sum = y_sum + y_tmp;

                    count_label = count_label + 1;
                }
            }

            x[i] = x_sum / count_label;
            y[i] = y_sum / count_label;
        }

        /** ファイルの書き出しと描画 **/

        // 各座標ごとに書き出し
        sprintf(filename[2], "result/03/labeling/blue_dat/%d.dat", m);
        fp = fopen(filename[2], "w");

        for (int i = 1; i <= n; i++)
        {
            position = x[i] + width * y[i];
            fprintf(fp, "%d\t%d\t%d\n", x[i], y[i], position);
        }

        fclose(fp);

        if (m <= 1000)
        {
            // Gnuplot 呼び出し
            if ((gp = popen("gnuplot", "w")) == NULL)
            {
                printf("gnuplot is not here!\n");
                exit(0); // gnuplotが無い場合、異常ある場合は終了
            }

            /** Gnuplot **/

            sprintf(graphname[0], "result/03/labeling/blue_svg/%d.png", m);
            sprintf(graphtitle[0], "labeling plane [%03d]", m);
            sprintf(filename[3], "result/03/stretch/blue_png/%d.png", m);

            // fprintf(gp, "set terminal svg enhanced size 800, 600 font 'Times New Roman, 20'\n");
            fprintf(gp, "set terminal png enhanced size 1000, 600 font 'Times New Roman, 20'\n");
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
            // fprintf(gp, "plot '%s' using 1:2 with points lc 'red' pt 1 ps 1 notitle\n", filename[2]);
            fprintf(gp, "plot '%s' binary filetype=png with rgbimage, '%s' using 1:2 with points lc 'red' pt 1 ps 1 notitle\n", filename[3], filename[2]);

            fflush(gp); // Clean up Data

            fprintf(gp, "exit\n"); // Quit gnuplot

            pclose(gp);
        }

        printf("Labeling blue\t%4d\n", m);
    }

    return 0;
}