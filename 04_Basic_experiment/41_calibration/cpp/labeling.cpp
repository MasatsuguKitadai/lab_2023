/******************************************************************************
PROGRAM : labeling
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/15
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>
using namespace std;

FILE *fp, *fp_2, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/** プロトタイプ宣言 **/
void Labeling(const char *name);
void Gnuplot_Loop(const char *name, int m);
void Gnuplot(const char *name);
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[]);
void Load_Bmp_24bit(const char file_name[], unsigned char header[], unsigned char binary[]);

/******************************************************************************/

#include "../hpp/settings.hpp"

/******************************************************************************
PROGRAM : main
DATE    : 2023/05/15
******************************************************************************/
int main()
{
    /* データ名の読み取り */
    string name_str;
    cout << "Data Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    Labeling(name);

    return 0;
}

/******************************************************************************
PROGRAM : labling
DATE    : 2023/05/15
******************************************************************************/
void Labeling(const char *name)
{
    char filename[10][200];

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/41_calibration/labeling", dir_path, name);
    mkdir(dirname, dirmode);

    sprintf(dirname, "%s/%s/41_calibration/labeling/dat", dir_path, name);
    mkdir(dirname, dirmode);

    sprintf(dirname, "%s/%s/41_calibration/labeling/png", dir_path, name);
    mkdir(dirname, dirmode);

    // 8bit.bmp
    unsigned char ary_calibration[px_8_origin] = {0};
    int ary_label[px_8_origin] = {0};
    int ary_buf[20] = {0};

    // バッファ
    float buf[10];

    float sum[2];
    float ave[2];
    int position;
    int area[1000] = {0};
    int min;
    int tmp = 0;

    // 校正画像の読み込み
    sprintf(filename[1], "%s/%s/41_calibration/binarization/calibration_8bit.bmp", dir_path, name);
    Load_Bmp_8bit(filename[1], header_8bit, ary_calibration);

    /** ラベリング **/

    int num = 0; // ラベリングの番号
    int label = 0;

    // ラベル番号の初期化
    int n = 0;

    for (int i = 0; i < px_8_origin; i++)
    {
        // 変数の初期化
        label = 0;
        tmp = 0;

        if (ary_calibration[i] != 0)
        {
            // エリア内のラベルの確認
            for (int j = -2; j < 0; j++)
            {
                for (int k = -2; k < 2; k++)
                {
                    // 調べる座標の計算
                    position = i + width_origin * j + k;

                    // 座標のオーバーフローの回避
                    if (position < 0)
                    {
                        position = 0;
                    }
                    else if (position > px_8_origin)
                    {
                        position = px_8_origin;
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
                for (int j = -2; j < 0; j++)
                {
                    for (int k = -2; k < 2; k++)
                    {
                        // 調べる座標の計算
                        position = i + width_origin * j + k;

                        // 座標のオーバーフローの回避
                        if (position < 0)
                        {
                            position = 0;
                        }
                        else if (position > px_8_origin)
                        {
                            position = px_8_origin;
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
    for (int i = 0; i < px_8_origin; i++)
    {
        label = ary_label[i];

        for (int j = 0; j < n; j++)
        {
            if (label == j)
            {
                area[j] = area[j] + 1;
            }
        }
    }

    // カウント用メモリの初期化
    int count = 1;

    // 閾値より高い面積のラベルを取得
    for (int i = 1; i < n; i++)
    {
        if (area[i] > 20)
        {
            buf[count] = i;
            count += 1;
        }
    }

    // ラベルの再張替え
    for (int i = 1; i < count; i++)
    {
        for (int j = 0; j < px_8_origin; j++)
        {
            if (ary_label[j] == buf[i])
            {
                ary_label[j] = i;
            }
        }
    }

    // 配列の初期化
    for (int i = 0; i < 1000; i++)
    {
        area[i] = 0;
    }

    // 面積の再計算
    for (int i = 0; i < px_8_origin; i++)
    {
        for (int j = 0; j < n; j++)
        {
            if (ary_label[i] == j)
            {
                area[j] = area[j] + 1;
            }
        }
    }

    // n の再カウント
    tmp = 0;

    for (int i = 1; i < n; i++)
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

    // グラフの背景
    sprintf(filename[2], "%s/%s/41_calibration/binarization/calibration_8bit.png", dir_path, name);

    for (int m = 1; m <= n; m++)
    {
        // ファイルの書き出し
        sprintf(filename[3], "%s/%s/41_calibration/labeling/dat/labeling_%d.dat", dir_path, name, m);
        fp = fopen(filename[3], "w");

        for (int j = 0; j < height_origin; j++)
        {
            for (int i = 0; i < width_origin; i++)
            {
                position = width_origin * j + i; // 調べる座標

                if (ary_label[position] == m)
                {
                    fprintf(fp, "%d\t%d\n", i, j);
                }
            }
        }

        fclose(fp);

        /** gnuplot **/
        Gnuplot_Loop(name, m);
        printf("in progress...\t%d\n", m);
    }

    // 一度に排出
    sprintf(filename[3], "%s/%s/41_calibration/labeling/labeling_gnuplot.dat", dir_path, name);
    fp = fopen(filename[3], "w");

    // 各座標ごとに書き出し
    sprintf(filename[4], "%s/%s/41_calibration/labeling/labeling.dat", dir_path, name);
    fp_2 = fopen(filename[4], "w");

    for (int j = 0; j < height_origin; j++)
    {
        for (int i = 0; i < width_origin; i++)
        {
            position = width_origin * j + i; // 調べる座標

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
    Gnuplot(name);
}

/******************************************************************************
PROGRAM : Gnuplot
概要    ：相間平面の画像を作成する
******************************************************************************/
void Gnuplot_Loop(const char *name, int m)
{
    /** ファイルの書き出しと描画 **/

    // 軸の設定

    // range x
    float x_min = 0;
    float x_max = width_origin;

    // range y
    float y_min = 0;
    float y_max = height_origin;

    // label
    const char *xxlabel = "x [px]";
    const char *yylabel = "y [px]";

    /** Gnuplot **/

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    char filename_1[100], filename_2[100];
    sprintf(filename_1, "%s/%s/41_calibration/binarization/calibration_8bit.png", dir_path, name);
    sprintf(filename_2, "%s/%s/41_calibration/labeling/dat/labeling_%d.dat", dir_path, name, m);

    char graphname[100];
    char graphtitle[100];
    sprintf(graphname, "%s/%s/41_calibration/labeling/png/labeling_%d.png", dir_path, name, m);
    sprintf(graphtitle, "labeling plane [%2d]", m);

    fprintf(gp, "set terminal pngcairo enhanced size 1000, 800 font 'Times New Roman, 20'\n");
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
    fprintf(gp, "plot '%s' binary filetype=png with rgbimage, '%s' using 1:2 with points  lc 'red' pt 5 ps 0.5 notitle\n", filename_1, filename_2);
    fflush(gp); // Clean up Data
    pclose(gp);
}

/******************************************************************************
PROGRAM : Gnuplot
概要    ：相間平面の画像を作成する
******************************************************************************/
void Gnuplot(const char *name)
{
    /** ファイルの書き出しと描画 **/

    // 軸の設定

    // range x
    float x_min = 0;
    float x_max = width_origin;

    // range y
    float y_min = 0;
    float y_max = height_origin;

    // label
    const char *xxlabel = "x [px]";
    const char *yylabel = "y [px]";

    /** Gnuplot **/

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    char filename_1[100], filename_2[100];
    sprintf(filename_1, "%s/%s/41_calibration/binarization/calibration_8bit.png", dir_path, name);
    sprintf(filename_2, "%s/%s/41_calibration/labeling/labeling_gnuplot.dat", dir_path, name);

    char graphname[100];
    char graphtitle[100];
    sprintf(graphname, "%s/%s/41_calibration/labeling/labeling.png", dir_path, name);
    sprintf(graphtitle, "Labeling plane");

    fprintf(gp, "set terminal pngcairo enhanced size 1000, 800 font 'Times New Roman, 20'\n");
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
    fprintf(gp, "plot '%s' binary filetype=png with rgbimage, '%s' using 1:2 with points  lc 'red' pt 5 ps 0.5 notitle\n", filename_1, filename_2);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot
    pclose(gp);
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

    // printf("data length = %d\n", i);

    fclose(fp); // ファイルを閉じる
}