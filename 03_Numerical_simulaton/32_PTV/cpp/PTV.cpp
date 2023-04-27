/******************************************************************************
PROGRAM : PTV_ver2
AUTHER  : Masatsugu Kitadai
DATE    : 2022/12/15
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
#include "../hpp/functions.hpp"
#include "../hpp/parameters.hpp"

/*****************************************************************************/

int px = width * height;
int cal_area[cal_size][cal_size];

/******************************************************************************/

void PTV(int num_1, int num_2, const char *name)
{
    /** 変数の設定 **/

    // ファイル名
    char filename_ptv[100];
    char filename_ptv_2[100];
    char filename_ptv_3[100];

    sprintf(filename_ptv, "%s/%s/LLS_1/particle_image_bmp/%d.bmp", dir_path, name, num_1);
    sprintf(filename_ptv_2, "%s/%s/LLS_2/particle_image_bmp/%d.bmp", dir_path, name, num_1);
    sprintf(filename_ptv_3, "%s/%s/PTV/PTV_vector_dat/%d.dat", dir_path, name, num_1);

    // 一般の変数
    int i, j, k, l, n, m;
    int h, w;

    /** 粒子座標の読み込み **/
    float x[500], y[500];

    // 配列の初期化
    for (i = 0; i < 500; i++)
    {
        x[i] = 0;
        y[i] = 0;
    }

    char filename[100];
    sprintf(filename, "%s/%s/LLS_1/labeling_position_dat/%d.dat", dir_path, name, num_1);
    fp = fopen(filename, "r");

    i = 0;
    while ((fscanf(fp, "%f\t%f\t%f", &buf[0], &buf[1], &buf[2])) != EOF)
    {
        x[i] = buf[0];
        y[i] = buf[1];
        i = i + 1;
    }

    fclose(fp);

    counter = i;

    /** BMPファイルの読み取り **/

    // image 1次元配列
    unsigned char ary[px];

    // (1) 8bit ファイルの読み込み
    Load_Bmp_8bit(filename_ptv, header_8bit, ary);

    // (x,y)配列に変換
    h = 0;
    w = 0;

    for (i = 0; i < px; i++)
    {
        if (w == width)
        {
            w = 0;
            image_0[w][h] = ary[i];
            h = h + 1;
        }
        else
        {
            image_0[w][h] = ary[i];
        }

        w = w + 1;
    }

    /******************************************************************************/

    // (2) 2枚目バイナリ読み込み
    Load_Bmp_8bit(filename_ptv_2, header_8bit, ary);

    // (x,y)配列に変換
    h = 0;
    w = 0;

    for (i = 0; i < px; i++)
    {
        if (w == width)
        {
            w = 0;
            h = h + 1;
        }

        image_1[w][h] = ary[i];
        w = w + 1;
    }

    /** 相関係数の算出 *********************************************************************/

    float sum[2];
    float ave[2];
    float R, R_tmp;
    int x_ptv, y_ptv;
    int buf[4];

    // ベクトルの算出
    int position_w[2]; // 粒子位置(x)
    int position_h[2]; // 粒子位置(y)

    float vx, vy;
    float vx_value, vy_value;
    float v_value, angle;

    /** 相関係数の算出準備 (0) **/

    int start_x, start_y;

    fp = fopen(filename_ptv_3, "w");

    for (k = 1; k < counter + 1; k++)
    {
        // 画像中心の座標
        position_w[0] = cal_size / 2;
        position_h[0] = cal_size / 2;

        position_w[1] = cal_size / 2;
        position_h[1] = cal_size / 2;

        // x が 0 以下になる
        if (x[k] - win_size / 2 < 0)
        {
            continue;
        }
        else if (x[k] + win_size / 2 >= width)
        {
            continue;
        }
        else
        {
            start_x = x[k] - win_size / 2;
        }

        // y が 0 以下になる
        if (y[k] - win_size / 2 < 0)
        {
            continue;
        }
        else if (y[k] + win_size / 2 >= height)
        {
            continue;
        }
        else
        {
            start_y = y[k] - win_size / 2;
        }

        // 配列への格納
        for (i = 0; i < win_size; i++)
            for (j = 0; j < win_size; j++)
            {
                win_area[i][j] = image_0[i + start_x][j + start_y];
            }

        // 配列の初期化
        for (i = 0; i < 2; i++)
        {
            sum[i] = 0;
            ave[i] = 0;
        }

        // 濃度値の総和
        for (i = 0; i < win_size; i++)
            for (j = 0; j < win_size; j++)
            {
                sum[0] = win_area[i][j] + sum[0];
            }

        // 濃度地の平均
        ave[0] = sum[0] / (win_size * win_size);
        // printf("ave[0] = %.3f\n", ave[0]); // 固定

        /** 相関係数の算出準備 (1) **/

        // 初期化
        R = 0;
        R_tmp = 0;
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;

        // x が 0 以下になる
        if (x[k] - cal_size / 2 < 0)
        {
            continue;
        }
        else if (x[k] + cal_size / 2 >= width)
        {
            continue;
        }
        else
        {
            start_x = x[k] - cal_size / 2;
        }

        // y が 0 以下になる
        if (y[k] - cal_size / 2 < 0)
        {
            continue;
        }
        // y が 1024以上になる
        else if (y[k] + cal_size / 2 >= height)
        {
            continue;
        }
        else
        {
            start_y = y[k] - cal_size / 2;
        }

        // 解析用の格納
        for (i = 0; i < cal_size; i++)
            for (j = 0; j < cal_size; j++)
            {
                cal_area[i][j] = image_1[i + start_x][j + start_y];
            }

        // サブピクセル解析
        float R_sub[cal_size - win_size][cal_size - win_size];
        int index_x = 0;
        int index_y = 0;

        // 配列の初期化
        for (i = 0; i <= cal_size - win_size; i++)
            for (j = 0; j <= cal_size - win_size; j++)
            {
                R_sub[i][j] = 0;
            }

        for (y_ptv = 0; y_ptv <= cal_size - win_size; y_ptv++)
            for (x_ptv = 0; x_ptv <= cal_size - win_size; x_ptv++)
            {
                sum[1] = 0;

                // 合計値・平均値の算出
                for (i = 0; i < win_size; i++)
                    for (j = 0; j < win_size; j++)
                    {
                        sum[1] = cal_area[i + x_ptv][j + y_ptv] + sum[1];
                    }

                ave[1] = sum[1] / (win_size * win_size);

                // 相関係数 R の算出

                float cal[10];

                for (i = 0; i < 10; i++)
                {
                    cal[i] = 0;
                }

                for (i = 0; i < win_size; i++)
                {
                    for (j = 0; j < win_size; j++)
                    {
                        cal[0] = win_area[i][j] - ave[0];
                        cal[1] = cal_area[i + x_ptv][j + y_ptv] - ave[1];

                        cal[2] = cal[0] * cal[1] + cal[2]; // 分子
                        cal[3] = cal[0] * cal[0] + cal[3]; // 分母（乗数）
                        cal[4] = cal[1] * cal[1] + cal[4]; // 分母（乗数）
                    }
                }

                cal[5] = sqrt(cal[3]);
                cal[6] = sqrt(cal[4]);

                // 相関係数の算出
                if (cal[5] * cal[6] == 0)
                {
                    R_sub[x_ptv][y_ptv] = 0;
                }
                else
                {
                    R_sub[x_ptv][y_ptv] = cal[2] / (cal[5] * cal[6]);
                }

                if (R_sub[x_ptv][y_ptv] > R)
                {
                    index_x = x_ptv;
                    index_y = y_ptv;
                    R = R_sub[x_ptv][y_ptv];
                    position_w[1] = x_ptv + (win_size / 2);
                    position_h[1] = y_ptv + (win_size / 2);
                    // printf("[%d]\t(%d, %d)\tR = %lf\n", k, x_ptv, y_ptv, R);
                }
            }

        vx = position_w[1] - position_w[0];
        vy = position_h[1] - position_h[0];

        if (cal_size - win_size > index_x && index_x > 0 && cal_size - win_size > index_y && index_y > 0 && R_sub[index_x][index_y] != 0)
        {
            // サブピクセル精度での特定
            float x_sbpx = log(R_sub[index_x - 1][index_y] / R_sub[index_x + 1][index_y]) / (2 * log(R_sub[index_x - 1][index_y] * R_sub[index_x + 1][index_y] / (R_sub[index_x][index_y] * R_sub[index_x][index_y])));
            float y_sbpx = log(R_sub[index_x][index_y - 1] / R_sub[index_x][index_y + 1]) / (2 * log(R_sub[index_x][index_y - 1] * R_sub[index_x][index_y + 1] / (R_sub[index_x][index_y] * R_sub[index_x][index_y])));

            // printf("x_sbpx = %lf\ty_sbpx = %lf\n", x_sbpx, y_sbpx);

            if (isnan(x_sbpx) == false)
            {
                vx += x_sbpx;
            }

            if (isnan(y_sbpx) == false)
            {
                vy += y_sbpx;
            }
        }

        // ベクトルの計算
        if (R <= 0.90)
        {
            v_value = 0;
            angle = 0;
            vx = 0;
            vy = 0;
        }
        else
        {
            v_value = sqrt(vx * vx + vy * vy);
            fprintf(fp, "%.1f\t%.1f\t%lf\t%lf\t%lf\t%lf\n", x[k], y[k], vx, vy, v_value, R);
        }
    }

    fclose(fp);

    // return 0;
}

void plot_ptv(int num, const char *name)
{
    /** Gnuplot **/

    // ファイル名
    char filename_ptv[100];
    char graphname[100];
    char imagename[150];
    char graphtitle[100];

    sprintf(filename_ptv, "%s/%s/PTV/PTV_vector_dat/%d.dat", dir_path, name, num);
    sprintf(graphname, "%s/%s/PTV/PTV_vector_png/%d.png", dir_path, name, num);
    sprintf(graphtitle, "PTV");

    // 軸の設定

    // range x
    float x_min = 0;
    float x_max = width;

    // range y
    float y_min = 0;
    float y_max = height;

    // range color
    float cb_min = 0;
    float cb_max = cal_size / 2;

    // label
    const char *xxlabel = "y [px]";
    const char *yylabel = "z [px] ";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    // fprintf(gp, "set terminal svg enhanced size 800, 600 font 'Times New Roman, 20'\n");
    fprintf(gp, "set terminal png enhanced size 800, 600 font 'Times New Roman, 20'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s'\n", graphtitle);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.0\n");
    fprintf(gp, "set ylabel offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.0\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:2:3:4:5 with vectors lc palette lw 1 notitle\n", filename_ptv);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);

    // return 0;
}

int main()
{
    /* 保存ディレクトリの設定 */
    string name_str;
    cout << "Case Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    /* ディレクトリの作成 */
    char dirname[3][100];
    sprintf(dirname[0], "%s/%s/PTV", dir_path, name);
    sprintf(dirname[1], "%s/%s/PTV/PTV_vector_dat", dir_path, name);
    sprintf(dirname[2], "%s/%s/PTV/PTV_vector_png", dir_path, name);

    mkdir(dirname[0], dirmode);
    mkdir(dirname[1], dirmode);
    mkdir(dirname[2], dirmode);

    /** PIV loop **/

    int i, j;

    for (i = 1; i < number - delta; i++)
    {
        j = i + delta;
        // j = i + 1;
        PTV(i, j, name);

        if (i < 100 - delta)
        {
            plot_ptv(i, name);
        }

        printf("PTV : %3d\n", i);
    }

    return 0;
}