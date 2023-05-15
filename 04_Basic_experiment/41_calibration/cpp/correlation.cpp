/******************************************************************************
PROGRAM : correlation
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/14
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>
using namespace std;

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/** プロトタイプ宣言 **/
void Ditect_calibration_points(const char *name);
void Gnuplot(const char *name);
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[]);
void Load_Bmp_24bit(const char file_name[], unsigned char header[], unsigned char binary[]);

/*****************************************************************************/

#include "../hpp/settings.hpp"

/******************************************************************************
PROGRAM : main
概要    ：
******************************************************************************/
int main()
{
    /* データ名の読み取り */
    string name_str;
    cout << "Data Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s", dir_path, name);
    mkdir(dirname, dirmode);

    Ditect_calibration_points(name);
    Gnuplot(name);

    return 0;
}

/******************************************************************************
PROGRAM : Ditect_calibration_points
概要    ： 校正画像の校正点を取得する
******************************************************************************/
void Ditect_calibration_points(const char *name)
{
    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/41_calibration/correlation", dir_path, name);
    mkdir(dirname, dirmode);

    /** (1) 円形画像の読み込み **/

    // dot.bmp
    const int size = 40;
    const int px_dot = size * size;
    unsigned char ary_dot[px_dot];

    // 参照dot画像の読み込み
    const char dotfile[] = "header/dot.bmp";
    Load_Bmp_8bit(dotfile, header_8bit, ary_dot);

    /** (2) 校正用画像の読み込み と 相関係数の計算 **/

    // 8bit.bmp
    unsigned char ary_calibration[px_8_origin];
    float sum[2];
    float ave[2];
    float R = 0;
    float cal[10];
    int position[2];

    // 校正画像の読み込み
    char imgfile[100];
    sprintf(imgfile, "%s/%s/41_calibration/binarization/calibration_8bit.bmp", dir_path, name);
    Load_Bmp_8bit(imgfile, header_8bit, ary_calibration);

    /** 相互相関係数の算出 **/

    // 配列の初期化
    for (int i = 0; i < 2; i++)
    {
        sum[i] = 0;
        ave[i] = 0;
    }

    // 濃度値の総和
    for (int i = 0; i < px_dot; i++)
    {
        sum[0] = ary_dot[i] + sum[0];
    }

    // 濃度値の平均
    ave[0] = sum[0] / px_dot;

    /** 相関係数の算出準備 (1) **/

    // 相互相関平面の書き出し
    char filename[100];
    sprintf(filename, "%s/%s/41_calibration/correlation/correlation.dat", dir_path, name);
    fp = fopen(filename, "w");

    for (int y = 0; y < height_origin - calibration_size; y++)
    {
        for (int x = 0; x < width_origin - calibration_size; x++)
        {
            sum[1] = 0;

            // 合計値・平均値の算出
            for (int j = 0; j < calibration_size; j++)
            {
                for (int i = 0; i < calibration_size; i++)
                {
                    position[0] = width_origin * (y + j) + (x + i);
                    sum[1] = ary_calibration[position[0]] + sum[1];
                }
            }

            ave[1] = sum[1] / (calibration_size * calibration_size);

            // 相関係数 R の算出

            // 配列の初期化
            for (int i = 0; i < 10; i++)
            {
                cal[i] = 0;
            }

            for (int i = 0; i < calibration_size; i++)
            {
                for (int j = 0; j < calibration_size; j++)
                {
                    position[0] = calibration_size * j + i;         // dot 画像の位置
                    position[1] = width_origin * (y + j) + (x + i); // 校正画像の位置

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

            fprintf(fp, "%d\t%d\t%lf\n", x + calibration_size / 2, y + calibration_size / 2, R);
        }

        fprintf(fp, "\n");
    }

    fclose(fp);
}

/******************************************************************************
PROGRAM : Gnuplot
概要    ：相間平面の画像を作成する
******************************************************************************/
void Gnuplot(const char *name)
{
    /** Gnuplot **/
    char datafile[100];
    char graphfile[100];
    sprintf(datafile, "%s/%s/41_calibration/correlation/correlation.dat", dir_path, name);
    sprintf(graphfile, "%s/%s/41_calibration/correlation/correlation.png", dir_path, name);
    const char graphtitle[] = "Correlation plane";

    // 軸の設定

    // range x
    float x_min = 0;
    float x_max = width_origin;

    // range y
    float y_min = 0;
    float y_max = height_origin;

    // range color
    float cb_min = -1;
    float cb_max = 1;

    // label
    const char *xxlabel = "x [pixel]";
    const char *yylabel = "y [pixel]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal pngcairo enhanced size 1000, 800 font 'Times New Roman, 20'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    fprintf(gp, "set pm3d map\n");
    fprintf(gp, "unset ztics \n");

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
    fprintf(gp, "set ylabel offset -1.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.0\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "splot '%s' using 1:2:3 with pm3d lc palette notitle\n", datafile);

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

/******************************************************************************
FUNCTION : Load_bmp_24bit
概要：8bitのbmp画像を配列に格納する
 IN ：file_name：読み込むファイル名，header：ヘッダーの格納用配列，binary：輝度値の格納用配列
OUT ：void / header, binary配列に値を格納する
******************************************************************************/
void Load_Bmp_24bit(const char file_name[], unsigned char header[], unsigned char binary[])
{
    FILE *fp;            // ファイルポインタの宣言
    int i;               // ループ用変数
    const int size = 54; // 画像サイズによって変動する可能性有り

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