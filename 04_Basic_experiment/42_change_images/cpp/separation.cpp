/******************************************************************************
PROGRAM : split
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/21
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
void Split(const char *name, const char *dat_set);

/*****************************************************************************/

#include "../hpp/settings.hpp"

// bmp 1次元配列
unsigned char binary[px_24_origin];
unsigned char binary_tmp[px_8_origin];
unsigned char binary_r[px_8_origin];
unsigned char binary_g[px_8_origin];
unsigned char binary_b[px_8_origin];

// 計算用配列
int sum[px_24_origin];           // 1024 × 1024 = 1048576
unsigned char ave[px_24_origin]; // 1024 × 1024 = 1048576

/******************************************************************************
PROGRAM : main
DATE    : 2023/05/12
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

    Separation(name, data_set);

    return 0;
}

/******************************************************************************
PROGRAM : Split
DATE    : 2023/05/12
******************************************************************************/
void Separation(const char *name, const char *data_set)
{
    float buf[10];
    char filename[10][100];

    /** ディレクトリの作成 **/
    char dirname[100];
    sprintf(dirname, "%s/%s/42_change_images/%s/split_blue", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    sprintf(dirname, "%s/%s/42_change_images/%s/split_green", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    float filter[2][3]; // フィルタの配列

    // フィルタファイルの読み込み
    sprintf(filename[0], "%s/%s/42_change_images/filter/filter.dat", dir_path, name);

    fp = fopen(filename[0], "r");

    int tmp = 0;

    while ((fscanf(fp, "%f\t%f\t%f", &buf[0], &buf[1], &buf[2])) != EOF)
    {
        filter[tmp][0] = buf[0]; // R
        filter[tmp][1] = buf[1]; // G
        filter[tmp][2] = buf[2]; // B
        tmp += 1;
    }

    fclose(fp);

    // header の読み込み
    sprintf(filename[0], "./header/%dx%d_8bit.bmp", width_origin, height_origin);
    Load_Bmp_8bit(filename[0], header_8bit, binary_tmp);

    for (int n = 1; n <= number; n++)
    {
        sprintf(filename[1], "%s/%s/42_change_images/%s/data/data_%04d.bmp", dir_path, name, data_set, n);

        // バイナリ読み込み
        Load_Bmp_24bit(filename[1], header_24bit, binary);
        tmp = 0;

        // 色ごとのデータの格納
        for (int i = 0; i < px_8_origin; i++)
        {
            binary_b[i] = binary[tmp];     // blue
            binary_g[i] = binary[tmp + 1]; // green
            binary_r[i] = binary[tmp + 2]; // red
            tmp += 3;
        }

        for (int i = 0; i < px_8_origin; i++)
        {
            if (binary_b[i] != 0)
            {
                int value = binary_b[i] * filter[0][1] / 100;
                if (binary_g[i] - value < 0)
                {
                    binary_g[i] = 0;
                }
                else
                {
                    binary_g[i] = binary_g[i] - value;
                }
            }
        }

        /** BMPファイルの書き出し **/

        // blue
        sprintf(filename[2], "%s/%s/42_change_images/%s/split_blue/data_%04d.bmp", dir_path, name, data_set, n);
        fp = fopen(filename[2], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

        // 画像情報の書き込み
        fwrite(binary_b, sizeof(unsigned char), px_8_origin, fp);

        fclose(fp);

        // green
        sprintf(filename[3], "%s/%s/42_change_images/%s/split_green/data_%04d.bmp", dir_path, name, data_set, n);
        fp = fopen(filename[3], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

        // 画像情報の書き込み
        fwrite(binary_g, sizeof(unsigned char), px_8_origin, fp);

        fclose(fp);

        printf("Split:%d\n", n);
    }
}