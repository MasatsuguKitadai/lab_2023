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
void Stretch_image_Blue(const char *name, const char *data_set);
void Stretch_image_Green(const char *name, const char *data_set);

/*****************************************************************************/

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

    string data_set_str;
    cout << "Data Set:";
    cin >> data_set_str;
    const char *data_set = data_set_str.c_str();

    Stretch_image_Blue(name, data_set);
    Stretch_image_Green(name, data_set);

    return 0;
}

/******************************************************************************
PROGRAM : Stretch_image
DATE    : 2023/05/15
******************************************************************************/
void Stretch_image_Blue(const char *name, const char *data_set)
{
    char filename[10][200];
    float buf[10] = {0};

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/42_change_images/%s/blue", dir_path, name, data_set);
    mkdir(dirname, dirmode);

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

    // キャリブレーション用の式の格納
    sprintf(filename[1], "%s/%s/41_calibration/curve_fit/curve_fit_0.0mm.dat", dir_path, name);

    int tmp = 0;

    fp = fopen(filename[1], "r");

    while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5], &buf[6], &buf[7], &buf[8], &buf[9])) != EOF)
    {
        a[tmp] = buf[0];
        b[tmp] = buf[1];
        c[tmp] = buf[2];
        d[tmp] = buf[3];
        e[tmp] = buf[4];
        f[tmp] = buf[5];
        g[tmp] = buf[6];
        h[tmp] = buf[7];
        o[tmp] = buf[8];
        p[tmp] = buf[9];
        tmp += 1;
    }

    fclose(fp);

    // ヘッダの読み込み
    sprintf(filename[3], "header/%dx%d_8bit.bmp", width, height);
    Load_Bmp_8bit(filename[3], header_8bit, binary_stretch);

    for (int n = 1; n <= number; n++)
    {
        // 変換用画像の取得
        unsigned char header_8bit_tmp[1072] = {0};
        sprintf(filename[0], "%s/%s/42_change_images/%s/split_blue/data_%04d.bmp", dir_path, name, data_set, n);
        Load_Bmp_8bit(filename[0], header_8bit_tmp, binary);

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
        }

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

        // 一次元配列への格納
        int k = 0;
        for (j = 0; j < height; j++)
            for (i = 0; i < width; i++)
            {
                binary_stretch[k] = binary_2d_new[i][j];
                k = k + 1;
            }

        // 書き出すグラフの名前
        sprintf(filename[4], "%s/%s/42_change_images/%s/blue/%04d.bmp", dir_path, name, data_set, n);

        fp = fopen(filename[4], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

        // 画像情報の書き込み
        fwrite(binary_stretch, sizeof(unsigned char), px_8, fp);

        fclose(fp);

        printf("Blue:%d\n", n);
    }
}

/******************************************************************************
PROGRAM : Stretch_image
DATE    : 2023/05/15
******************************************************************************/
void Stretch_image_Green(const char *name, const char *data_set)
{
    char filename[10][200];
    float buf[10] = {0};

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/42_change_images/%s/green", dir_path, name, data_set);
    mkdir(dirname, dirmode);

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

    // キャリブレーション用の式の格納
    sprintf(filename[1], "%s/%s/41_calibration/curve_fit/curve_fit_2.5mm.dat", dir_path, name);
    printf("%s\n", filename[1]);

    int tmp = 0;

    fp = fopen(filename[1], "r");

    while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5], &buf[6], &buf[7], &buf[8], &buf[9])) != EOF)
    {
        a[tmp] = buf[0];
        b[tmp] = buf[1];
        c[tmp] = buf[2];
        d[tmp] = buf[3];
        e[tmp] = buf[4];
        f[tmp] = buf[5];
        g[tmp] = buf[6];
        h[tmp] = buf[7];
        o[tmp] = buf[8];
        p[tmp] = buf[9];
        tmp += 1;
    }

    fclose(fp);

    // ヘッダの読み込み
    sprintf(filename[3], "header/%dx%d_8bit.bmp", width, height);
    Load_Bmp_8bit(filename[3], header_8bit, binary_stretch);

    for (int n = 1; n <= number; n++)
    {
        // 変換用画像の取得
        unsigned char header_8bit_tmp[1072] = {0};
        sprintf(filename[0], "%s/%s/42_change_images/%s/split_green/data_%04d.bmp", dir_path, name, data_set, n);
        Load_Bmp_8bit(filename[0], header_8bit_tmp, binary);

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
        }

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

        // 一次元配列への格納
        int k = 0;
        for (j = 0; j < height; j++)
            for (i = 0; i < width; i++)
            {
                binary_stretch[k] = binary_2d_new[i][j];
                k = k + 1;
            }

        // 書き出すグラフの名前
        sprintf(filename[4], "%s/%s/42_change_images/%s/green/%04d.bmp", dir_path, name, data_set, n);

        fp = fopen(filename[4], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

        // 画像情報の書き込み
        fwrite(binary_stretch, sizeof(unsigned char), px_8, fp);

        fclose(fp);

        printf("Green:%d\n", n);
    }
}