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
#include "../hpp/settings.hpp"

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/** プロトタイプ宣言 **/
void PTV(const char image_name_1[], const char image_name_2[], const char labeling[], const char writefile[]);
void plot_ptv(int num, const char *name);
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[]);

/******************************************************************************/

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
    sprintf(dirname[2], "%s/%s/PTV/PTV_vector_svg", dir_path, name);

    mkdir(dirname[0], dirmode);
    mkdir(dirname[1], dirmode);
    mkdir(dirname[2], dirmode);

    /** PIV loop **/
    char image_name_1[100], image_name_2[100], labeling[100], writefile[100];

    for (int n = 1; n < data_num - delta_n; n++)
    {
        sprintf(image_name_1, "%s/%s/LLS_1/particle_image_bmp/%d.bmp", dir_path, name, n);
        sprintf(image_name_2, "%s/%s/LLS_2/particle_image_bmp/%d.bmp", dir_path, name, n + delta_n);
        sprintf(labeling, "%s/%s/LLS_1/labeling_position_dat/%d.dat", dir_path, name, n);
        sprintf(writefile, "%s/%s/PTV/PTV_vector_dat/%d.dat", dir_path, name, n);
        PTV(image_name_1, image_name_2, labeling, writefile);
        cout << "PTV = " << n << endl;

        if (n <= 100)
        {
            plot_ptv(n, name);
        }
    }

    return 0;
}

/******************************************************************************
FUNCTION : PTV
概要：PTV処理を行う
******************************************************************************/
void PTV(const char image_name_1[], const char image_name_2[], const char labeling[], const char writefile[])
{
    /** 配列の定義 **/
    unsigned char binary_1[size_px];            // 1枚目の画像の一次元配列
    unsigned char binary_2[size_px];            // 2枚目の画像の一次元配列
    unsigned char image_1[width_px][height_px]; // 1枚目の画像の二次元配列
    unsigned char image_2[width_px][height_px]; // 2枚目の画像の二次元配列
    vector<int> label_x;                        // ラベリングした粒子位置 [px]
    vector<int> label_y;                        // ラベリングした粒子位置 [px]

    /** 対象の画像ファイルを読み込む **/
    Load_Bmp_8bit(image_name_1, header_8bit, binary_1);
    Load_Bmp_8bit(image_name_2, header_8bit, binary_2);

    /** 二次元配列への格納 **/
    int w_tmp = 0;
    int h_tmp = 0;
    for (int i = 0; i < size_px; i++)
    {
        if (i != 0 && i % width_px == 0)
        {
            w_tmp = 0;
            h_tmp += 1;
        }

        image_1[w_tmp][h_tmp] = binary_1[i];
        image_2[w_tmp][h_tmp] = binary_2[i];
        w_tmp += 1;
    }

    /** ラベリング後の粒子位置の読み込み **/
    float x_tmp, y_tmp, num;
    fp = fopen(labeling, "r");
    while ((fscanf(fp, "%f\t%f\t%f", &x_tmp, &y_tmp, &num)) != EOF)
    {
        label_x.push_back(x_tmp);
        label_y.push_back(y_tmp);
    }
    fclose(fp);

    /** 相関係数の算出 **/
    fp = fopen(writefile, "w");

    for (int n = 0; n < label_x.size(); n++)
    {
        /// B画像の格納 //////////////////////////////////////////////////////////////////////
        /** PTV解析時のエラー回避：x方向 **/
        int start_x;
        int min_tmp = label_x[n] - w2 / 2;
        int max_tmp = label_x[n] + w2 / 2;

        if (min_tmp < 0 || width_px <= max_tmp)
        {
            continue; // 計算不可の場合は次の粒子像へ
        }
        else
        {
            start_x = label_x[n] - w1 / 2;
        }

        /** PTV解析時のエラー回避：y方向 **/
        int start_y;
        min_tmp = label_y[n] - w2 / 2;
        max_tmp = label_y[n] + w2 / 2;

        if (min_tmp < 0 || height_px <= max_tmp)
        {
            continue; // 計算不可の場合は次の粒子像へ
        }
        else
        {
            start_y = label_y[n] - w1 / 2;
        }

        /** 計算用の配列への格納 **/
        unsigned char image_w1[w1][w1];
        float sum_w1;

        for (int w = 0; w < w1; w++)
            for (int h = 0; h < w1; h++)
            {
                image_w1[w][h] = image_1[w + start_x][h + start_y];
                sum_w1 += image_w1[w][h];
            }

        // 平均値の計算
        const float ave_w1 = sum_w1 / w1 * w1;

        /// G画像の格納 //////////////////////////////////////////////////////////////////////

        /** 計算用の配列への格納 (計算領域全体) **/
        unsigned char image_w2[w2][w2];
        start_x = label_x[n] - w2 / 2;
        start_y = label_y[n] - w2 / 2;

        for (int w = 0; w < w2; w++)
            for (int h = 0; h < w2; h++)
            {
                image_w2[w][h] = image_2[w + start_x][h + start_y];
            }

        /// 相関係数の算出 //////////////////////////////////////////////////////////////////////

        /** サブピクセル解析用の配列 **/
        float R_max = 0;
        float R_sub[w2 - w1][w2 - w1];
        int index_x = 0;
        int index_y = 0;
        float distance_x, distance_y;

        /** 相関係数の計算 **/
        for (int x = 0; x < w2 - w1; x++)
            for (int y = 0; y < w2 - w1; y++)
            {
                // 平均値の計算
                float sum_w2 = 0;
                for (int w = 0; w < w1; w++)
                    for (int h = 0; h < w1; h++)
                    {
                        sum_w2 += image_w2[w + x][h + y];
                    }
                const float ave_w2 = sum_w2 / w1 * w1;

                // 計算用バッファの用意
                float cal[10];
                for (int i = 0; i < 10; i++)
                {
                    cal[i] = 0;
                }

                for (int w = 0; w < w1; w++)
                    for (int h = 0; h < w1; h++)
                    {
                        cal[0] = image_w1[w][h] - ave_w1;
                        cal[1] = image_w2[w + x][h + y] - ave_w2;
                        cal[2] += cal[0] * cal[1];
                        cal[3] += cal[0] * cal[0];
                        cal[4] += cal[1] * cal[1];
                    }

                cal[5] = sqrt(cal[3]);
                cal[6] = sqrt(cal[4]);

                // 算出時のエラー回避
                if (cal[5] * cal[6] == 0)
                {
                    R_sub[x][y] = 0;
                }
                else
                {
                    R_sub[x][y] = cal[2] / (cal[5] * cal[6]);
                }

                // 最も高い相関係数を取得
                if (R_sub[x][y] >= R_max)
                {
                    index_x = x;
                    index_y = y;
                    R_max = R_sub[x][y];
                    distance_x = x + (w1 / 2);
                    distance_y = y + (w1 / 2);
                }
            }

        // 移動量ベクトルの計算
        float vx = distance_x - (w2 / 2);
        float vy = distance_y - (w2 / 2);

        /** サブピクセル補完 **/
        if (w2 - w1 > index_x && index_x > 0 && w2 - w1 > index_y && index_y > 0 && R_sub[index_x][index_y] != 0)
        {
            float x_sbpx = log(R_sub[index_x - 1][index_y] / R_sub[index_x + 1][index_y]) / (2 * log(R_sub[index_x - 1][index_y] * R_sub[index_x + 1][index_y] / (R_sub[index_x][index_y] * R_sub[index_x][index_y])));
            float y_sbpx = log(R_sub[index_x][index_y - 1] / R_sub[index_x][index_y + 1]) / (2 * log(R_sub[index_x][index_y - 1] * R_sub[index_x][index_y + 1] / (R_sub[index_x][index_y] * R_sub[index_x][index_y])));

            // エラー回避
            if (isnan(x_sbpx) == false)
            {
                vx += x_sbpx;
            }

            // エラー回避
            if (isnan(y_sbpx) == false)
            {
                vy += y_sbpx;
            }
        }

        // ベクトルの計算
        if (R_max <= 0.90)
        {
            vx = 0;
            vy = 0;
            float v_value = 0;
        }
        else
        {
            float v_value = sqrt(vx * vx + vy * vy);
            fprintf(fp, "%d\t%d\t%f\t%f\t%f\t%f\n", label_x[n], label_y[n], vx, vy, v_value, R_max);
        }
    }

    fclose(fp);
}

/******************************************************************************
FUNCTION : plot_ptv
概要：PTV処理を行う
******************************************************************************/
void plot_ptv(int num, const char *name)
{
    /** Gnuplot **/

    // ファイル名
    char filename_ptv[100];
    char graphname[100];
    char imagename[150];
    char graphtitle[100];

    sprintf(filename_ptv, "%s/%s/PTV/PTV_vector_dat/%d.dat", dir_path, name, num);
    sprintf(graphname, "%s/%s/PTV/PTV_vector_svg/%d.svg", dir_path, name, num);
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