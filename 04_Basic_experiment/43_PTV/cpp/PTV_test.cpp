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
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/** プロトタイプ宣言 **/
void PTV(int num, const char *name, const char *data_set);
void plot_ptv(int num, const char *name, const char *data_set);
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[]);

/*****************************************************************************/

#include "../hpp/settings.hpp"
int px = width_px * height_px;
int cal_area[w2][w2];

int delta_n2;

/******************************************************************************/

int main()
{
    /* 保存ディレクトリの設定 */
    const char *name = "20230517";
    const char *data_set = "delta_001";

    cout << "Delta:";
    cin >> delta_n2;
    cout << "n=" << delta_n2 << endl;

    /* ディレクトリの作成 */
    char dirname[3][100];
    sprintf(dirname[0], "%s/%s/43_PTV/%s_n=%d", dir_path, name, data_set, delta_n2);
    sprintf(dirname[1], "%s/%s/43_PTV/%s_n=%d/PTV_vector_dat", dir_path, name, data_set, delta_n2);
    sprintf(dirname[2], "%s/%s/43_PTV/%s_n=%d/PTV_vector_svg", dir_path, name, data_set, delta_n2);
    mkdir(dirname[0], dirmode);
    mkdir(dirname[1], dirmode);
    mkdir(dirname[2], dirmode);

    /** PIV loop **/
    int i, j;

    for (i = 1; i < number - delta_n2; i++)
    {
        j = i + delta_n2;
        // j = i + 1;
        printf("PTV : %3d\t", i);

        PTV(i, name, data_set);

        // if (i < 100 - delta_n2)
        // {
        //     plot_ptv(i, name, data_set);
        // }
    }

    return 0;
}

void PTV(int num, const char *name, const char *data_set)
{
    /** 変数の設定 **/

    // ファイル名
    char filename_ptv[100];
    char filename_ptv_2[100];
    char filename_ptv_3[100];
    char filename[3][100];

    sprintf(filename_ptv, "%s/%s/42_change_images/%s/blue/%04d.bmp", dir_path, name, data_set, num);
    sprintf(filename_ptv_2, "%s/%s/42_change_images/%s/green/%04d.bmp", dir_path, name, data_set, num + delta_n2);
    sprintf(filename_ptv_3, "%s/%s/43_PTV/%s_n=%d/PTV_vector_dat/%04d.dat", dir_path, name, data_set, delta_n2, num);

    // 一般の変数
    int i, j, k, l, n, m;
    int h, w;
    int vector_num = 0;

    /** 粒子座標の読み込み **/
    float x[500], y[500];

    // 配列の初期化
    for (i = 0; i < 500; i++)
    {
        x[i] = 0;
        y[i] = 0;
    }

    sprintf(filename[0], "%s/%s/43_PTV/%s/labeling_position_dat/%04d.dat", dir_path, name, data_set, num);

    fp = fopen(filename[0], "r");

    float buf[10];

    i = 0;
    while ((fscanf(fp, "%f\t%f\t%f", &buf[0], &buf[1], &buf[2])) != EOF)
    {
        x[i] = buf[0];
        y[i] = buf[1];
        i = i + 1;
    }

    fclose(fp);

    int counter = i;

    /** BMPファイルの読み取り **/

    // image 1次元配列
    unsigned char ary[px];
    unsigned char image_0[width_px][height_px];
    unsigned char image_1[width_px][height_px];

    // (1) 8bit ファイルの読み込み
    Load_Bmp_8bit(filename_ptv, header_8bit, ary);

    // (x,y)配列に変換
    h = 0;
    w = 0;

    for (i = 0; i < px; i++)
    {
        if (w == width_px)
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
        if (w == width_px)
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

    // ベクトルの算出
    int position_w[2]; // 粒子位置(x)
    int position_h[2]; // 粒子位置(y)
    float vx, vy;
    float vx_value, vy_value;
    float v_value, angle;
    unsigned char win_area[w1][w1];
    unsigned char cal_area[w2][w2];

    /** 相関係数の算出準備 (0) **/
    int start_x, start_y;
    fp = fopen(filename_ptv_3, "w");
    for (k = 1; k < counter + 1; k++)
    {

        // 画像中心の座標
        position_w[0] = w2 / 2;
        position_h[0] = w2 / 2;

        position_w[1] = w2 / 2;
        position_h[1] = w2 / 2;

        // x が 0 以下になる
        if (x[k] - w1 / 2 < 0)
        {
            continue;
        }
        else if (x[k] + w1 / 2 >= width_px)
        {
            continue;
        }
        else
        {
            start_x = x[k] - w1 / 2;
        }

        // y が 0 以下になる
        if (y[k] - w1 / 2 < 0)
        {
            continue;
        }
        else if (y[k] + w1 / 2 >= height_px)
        {
            continue;
        }
        else
        {
            start_y = y[k] - w1 / 2;
        }

        // 配列への格納
        for (i = 0; i < w1; i++)
            for (j = 0; j < w1; j++)
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
        for (i = 0; i < w1; i++)
            for (j = 0; j < w1; j++)
            {
                sum[0] = win_area[i][j] + sum[0];
            }

        // 濃度地の平均
        ave[0] = sum[0] / (w1 * w1);

        /** 相関係数の算出準備 (1) **/

        // 初期化
        R = 0;
        R_tmp = 0;
        buf[0] = 0;
        buf[1] = 0;
        buf[2] = 0;
        buf[3] = 0;

        // x が 0 以下になる
        if (x[k] - w2 / 2 < 0)
        {
            continue;
        }
        else if (x[k] + w2 / 2 >= width_px)
        {
            continue;
        }
        else
        {
            start_x = x[k] - w2 / 2;
        }

        // y が 0 以下になる
        if (y[k] - w2 / 2 < 0)
        {
            continue;
        }
        // y が 1024以上になる
        else if (y[k] + w2 / 2 >= height_px)
        {
            continue;
        }
        else
        {
            start_y = y[k] - w2 / 2;
        }

        // 解析用の格納
        for (i = 0; i < w2; i++)
            for (j = 0; j < w2; j++)
            {
                cal_area[i][j] = image_1[i + start_x][j + start_y];
            }

        // サブピクセル解析
        float R_sub[w2 - w1][w2 - w1];
        int index_x = 0;
        int index_y = 0;

        // 配列の初期化
        for (i = 0; i <= w2 - w1; i++)
            for (j = 0; j <= w2 - w1; j++)
            {
                R_sub[i][j] = 0;
            }

        for (y_ptv = 0; y_ptv <= w2 - w1; y_ptv++)
            for (x_ptv = 0; x_ptv <= w2 - w1; x_ptv++)
            {
                sum[1] = 0;

                // 合計値・平均値の算出
                for (i = 0; i < w1; i++)
                    for (j = 0; j < w1; j++)
                    {
                        sum[1] = cal_area[i + x_ptv][j + y_ptv] + sum[1];
                    }

                ave[1] = sum[1] / (w1 * w1);

                // 相関係数 R の算出

                float cal[10];

                for (i = 0; i < 10; i++)
                {
                    cal[i] = 0;
                }

                for (i = 0; i < w1; i++)
                {
                    for (j = 0; j < w1; j++)
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
                    position_w[1] = x_ptv + (w1 / 2);
                    position_h[1] = y_ptv + (w1 / 2);
                }
            }

        vx = position_w[1] - position_w[0];
        vy = position_h[1] - position_h[0];

        if (w2 - w1 > index_x && index_x > 0 && w2 - w1 > index_y && index_y > 0 && R_sub[index_x][index_y] != 0)
        {
            // サブピクセル精度での特定
            float x_sbpx = log(R_sub[index_x - 1][index_y] / R_sub[index_x + 1][index_y]) / (2 * log(R_sub[index_x - 1][index_y] * R_sub[index_x + 1][index_y] / (R_sub[index_x][index_y] * R_sub[index_x][index_y])));
            float y_sbpx = log(R_sub[index_x][index_y - 1] / R_sub[index_x][index_y + 1]) / (2 * log(R_sub[index_x][index_y - 1] * R_sub[index_x][index_y + 1] / (R_sub[index_x][index_y] * R_sub[index_x][index_y])));

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
        v_value = sqrt(vx * vx + vy * vy);
        fprintf(fp, "%.1f\t%.1f\t%lf\t%lf\t%lf\t%lf\n", x[k], y[k], vx, vy, v_value, R);
        vector_num += 1;
    }

    fclose(fp);

    printf("Number of vector = %d\n", vector_num);

    // return 0;
}

void plot_ptv(int num, const char *name, const char *data_set)
{
    /** Gnuplot **/

    // ファイル名
    char filename_ptv[100];
    char graphname[100];
    char imagename[150];
    char graphtitle[100];

    sprintf(filename_ptv, "%s/%s/43_PTV/%s_n=%d/PTV_vector_dat/%04d.dat", dir_path, name, data_set, delta_n2, num);
    sprintf(graphname, "%s/%s/43_PTV/%s_n=%d/PTV_vector_svg/%04d.svg", dir_path, name, data_set, delta_n2, num);
    sprintf(graphtitle, "PTV");

    // 軸の設定

    // range x
    float x_min = 0;
    float x_max = width_px;

    // range y
    float y_min = 0;
    float y_max = height_px;

    // range color
    float cb_min = 0;
    float cb_max = w2 / 2;

    // label
    const char *xxlabel = "y [px]";
    const char *yylabel = "z [px] ";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 800, 600 font 'Times New Roman, 20'\n");
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

/******************************************************************************
FUNCTION : Load_bmp_8bit
概要：8bitのbmp画像を配列に格納する
 IN ：file_name：読み込むファイル名，header：ヘッダーの格納用配列，binary：輝度値の格納用配列
OUT ：void / header, binary配列に値を格納する
******************************************************************************/
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[])
{
    FILE *fp;              // ファイルポインタの宣言
    int i;                 // ループ用変数
    const int size = 1078; // 画像サイズによって変動する可能性有り

    // bmpファイルをバイナリモードで読み取り
    fp = fopen(file_name, "rb");

    // 画像ファイルが見つからない場合のエラー処理
    if (fp == NULL)
    {
        printf("Not found : %s \n", file_name);
        exit(-1);
    }

    // ヘッダ情報の読み込み
    for (i = 0; i < size; i++)
    {
        header[i] = fgetc(fp);
    }

    // 画像がビットマップで無い場合のエラー処理
    if (!(header[0] == 'B' && header[1] == 'M'))
    {
        printf("Not BMP file : %s \n", file_name);
        exit(-1);
    }

    int tmp; // 輝度値の一時的な格納用変数
    i = 0;

    // 輝度値読み込み
    while ((tmp = fgetc(fp)) != EOF)
    {
        binary[i] = tmp;
        i = i + 1;
    }

    fclose(fp); // ファイルを閉じる
}
