/******************************************************************************
PROGRAM : stretch_image
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/17
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
void Binarization(const char *name);
void Stretch_image(const char *name);
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[]);
void Load_Bmp_24bit(const char file_name[], unsigned char header[], unsigned char binary[]);

/*****************************************************************************/

#include "../hpp/settings.hpp"

/******************************************************************************/
// gnuplot
char graphname[2][100];
char graphtitle[100];
char xxlabel[100] = "x [pixel]";
char yylabel[100] = "y [pixel]";
char zzlabel[2][100];
double x_max = width;
double x_min = 0;
double y_max = height;
double y_min = 0;
double cb_max = 150;
double cb_min = -50;

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

    Binarization(name);
    Stretch_image(name);

    return 0;
}

/******************************************************************************
PROGRAM : Binarization
概要：校正画像の二値化処理を行う
******************************************************************************/
void Binarization(const char *name)
{
    /** BMPファイルの読み取り **/

    // bmp 1次元配列
    unsigned char binary_8bit[px_8_origin];   // 1色
    unsigned char binary_24bit[px_24_origin]; // 3色

    char filename[3][100];

    sprintf(filename[0], "header/%dx%d_8bit.bmp", width_origin, height_origin);
    sprintf(filename[1], "%s/%s/41_calibration/model/model.bmp", dir_path, name);
    sprintf(filename[2], "%s/%s/41_calibration/model/model_8bit.bmp", dir_path, name);

    printf("%s\n", filename[1]);

    // バイナリ読み込み
    Load_Bmp_8bit(filename[0], header_8bit, binary_8bit);
    Load_Bmp_24bit(filename[1], header_24bit, binary_24bit);

    /** 配列整理 **/

    for (int i = 0; i < px_8_origin; i++)
    {
        // Red
        // k = 3 * i + 2;

        // Green
        int x = 3 * i + 1;

        // // Blue
        // k = 3 * i;

        // 二値化の分岐
        if (binary_24bit[x] < border_model)
        {
            binary_8bit[i] = 0;
        }
        else
        {
            binary_8bit[i] = 255;
        }
    }

    /** BMPファイルの書き出し **/

    // overlap
    fp = fopen(filename[2], "wb");

    // ヘッダー情報の書き込み
    fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

    // 画像情報の書き込み
    fwrite(binary_8bit, sizeof(unsigned char), px_8_origin, fp);

    fclose(fp);
}

/******************************************************************************
PROGRAM : Stretch_image
DATE    : 2023/05/15
******************************************************************************/
void Stretch_image(const char *name)
{
    char filename[10][200];
    float buf[10] = {0};

    /** 変数宣言 **/
    double a[2], b[2], c[2], d[2], e[2], f[2], g[2], h[2], o[2], p[2];
    double x, y;
    int x_buf, y_buf;
    double delta_x, delta_y;

    // bmp 1次元配列
    unsigned char binary[px_8_origin] = {0};
    unsigned char binary_stretch[px_8] = {0};
    unsigned char binary_2d[width_origin][height_origin] = {0};
    unsigned char binary_2d_new[width][height] = {0};

    // 校正用画像の取得
    sprintf(filename[0], "%s/%s/41_calibration/model/model_8bit.bmp", dir_path, name);
    Load_Bmp_8bit(filename[0], header_8bit, binary);

    // 2次元配列への格納
    int i = 0;
    int j = 0;

    for (int k = 0; k < px_8_origin; k++)
    {
        if (i == width_origin)
        {
            i = 0;
            j = j + 1;
        }

        binary_2d[i][j] = binary[k];
        i = i + 1;

        // printf("[%d][%d]\t2d = %d\tdata = %d\n", i, j, binary_2d[i][j], binary[k]);
    }

    double distance; // 参照点の奥行間距離

    // キャリブレーション用の式の格納
    sprintf(filename[1], "%s/%s/41_calibration/curve_fit/curve_fit_0.0mm.dat", dir_path, name);
    printf("%s\n", filename[1]);

    i = 0;

    fp = fopen(filename[1], "r");

    while (fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5], &buf[6], &buf[7], &buf[8], &buf[9]) != EOF)
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
        i += 1;
    }

    fclose(fp);

    // 参照位置の計算
    for (i = 0; i < width; i++)
        for (j = 0; j < height; j++)
        {
            // 元の画像における座標
            x = a[0] * i * i * i + b[0] * j * j * j + c[0] * i * i * j + d[0] * i * j * j + e[0] * i * i + f[0] * j * j + g[0] * i * j + h[0] * i + o[0] * j + p[0]; // x方向
            y = a[1] * i * i * i + b[1] * j * j * j + c[1] * i * i * j + d[1] * i * j * j + e[1] * i * i + f[1] * j * j + g[1] * i * j + h[1] * i + o[1] * j + p[1]; // y方向
            delta_x = x - i;
            delta_y = y - j;
            x_buf = x;
            y_buf = y;

            // 平均値の計算
            binary_2d_new[i][j] = (binary_2d[x_buf][y_buf] + binary_2d[x_buf + 1][y_buf] + binary_2d[x_buf][y_buf + 1] + binary_2d[x_buf + 1][y_buf + 1]) / 4;
        }

    // ヘッダの読み込み
    sprintf(filename[3], "header/%dx%d_8bit.bmp", width, height);
    Load_Bmp_8bit(filename[3], header_8bit, binary_stretch);

    // 一次元配列への格納
    int k = 0;
    for (j = 0; j < height; j++)
    {
        for (i = 0; i < width; i++)
        {
            binary_stretch[k] = binary_2d_new[i][j];
            k = k + 1;
        }
    }

    // 書き出すグラフの名前
    sprintf(filename[4], "%s/%s/41_calibration/model/model_stretch.bmp", dir_path, name);

    fp = fopen(filename[4], "wb");

    // ヘッダー情報の書き込み
    fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

    // 画像情報の書き込み
    fwrite(binary_stretch, sizeof(unsigned char), px_8, fp);

    fclose(fp);
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