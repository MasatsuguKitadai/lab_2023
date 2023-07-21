/******************************************************************************
PROGRAM : 01_PIV_sub
AUTHER  : Masatsugu Kitadai
DATE    : 2022/5/27
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
void Background(const char *name, const char *data_set);
void Background_blue(const char *name, const char *data_set);
void Background_green(const char *name, const char *data_set);

/*****************************************************************************/

#include "../hpp/settings.hpp"

// bmp 1次元配列
unsigned char binary[px_24_origin] = {0};

// 計算用配列
int sum[px_24_origin] = {0};           // 1024 × 1024 = 1048576
unsigned char ave[px_24_origin] = {0}; // 1024 × 1024 = 1048576

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

    const char *blue = "blue";
    const char *green = "green";

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/42_change_images/%s", dir_path, name, blue);
    mkdir(dirname, dirmode);

    sprintf(dirname, "%s/%s/42_change_images/%s", dir_path, name, green);
    mkdir(dirname, dirmode);

    // Background(name, blue);
    Background(name, green);
    return 0;
}

/******************************************************************************
PROGRAM : Background
概要：校正画像の二値化処理を行う
******************************************************************************/
void Background(const char *name, const char *data_set)
{
    char dirname[10][100];
    char filename[10][100];

    /** ディレクトリの作成 **/
    sprintf(dirname[0], "%s/%s/42_change_images/%s/background", dir_path, name, data_set);
    mkdir(dirname[0], dirmode);

    sprintf(dirname[1], "%s/%s/42_change_images/%s/data", dir_path, name, data_set);
    mkdir(dirname[1], dirmode);

    /** BMPファイルの読み取り **/

    // 配列の初期化
    for (int i = 0; i < px_24_origin; i++)
    {
        sum[i] = 0;
        ave[i] = 0;
    }

    /** 配列の合計値の算出 **/

    for (int i = 1; i <= 100; i++)
    {
        sprintf(filename[0], "%s/data/%s/%s/data_%04d.bmp", dir_path, name, data_set, i);

        // バイナリ読み込み
        Load_Bmp_24bit(filename[0], header_24bit, binary);

        for (int i = 0; i < px_24_origin; i++)
        {
            sum[i] += binary[i];
        }

        printf("Background : [%4d]\n", i);
    }

    /** 配列の平均値の算出 **/

    for (int i = 0; i < px_24_origin; i++)
    {
        ave[i] = sum[i] / 100;
    }

    /** BMPファイルの書き出し **/
    sprintf(filename[1], "%s/%s/42_change_images/%s/background/background.bmp", dir_path, name, data_set);
    printf("%s\n", filename[1]);

    fp = fopen(filename[1], "wb");

    // ヘッダー情報の書き込み
    fwrite(header_24bit, sizeof(unsigned char), 54, fp);

    // 画像情報の書き込み
    fwrite(ave, sizeof(unsigned char), px_24_origin, fp);

    fclose(fp);

    /** 背景処理の適用 **/

    // 背景画像の読み込み

    for (int i = 1; i <= 4000; i++)
    {
        sprintf(filename[2], "%s/data/%s/%s/data_%04d.bmp", dir_path, name, data_set, i);

        // 初期化
        for (int j = 0; j < px_24_origin; j++)
        {
            binary[j] = 0;
        }

        // バイナリ読み込み
        Load_Bmp_24bit(filename[2], header_24bit, binary);

        // 引き算
        for (int j = 0; j < px_24_origin; j++)
        {
            if (binary[j] - ave[j] < 0)
            {
                binary[j] = 0;
            }
            else
            {
                binary[j] = binary[j] - ave[j];
            }
        }

        // 背景補正
        for (int j = 0; j < px_24_origin; j++)
        {
            if (binary[j] <= 30)
            {
                binary[j] = 0;
            }
        }

        // BMPファイルの書き出し
        sprintf(filename[3], "%s/%s/42_change_images/%s/data/data_%04d.bmp", dir_path, name, data_set, i);

        fp = fopen(filename[3], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_24bit, sizeof(unsigned char), 54, fp);

        // 画像情報の書き込み
        fwrite(binary, sizeof(unsigned char), px_24_origin, fp);

        fclose(fp);

        printf("Change : [%4d]\n", i);
    }
}

/******************************************************************************
PROGRAM : Background_blue
概要：校正画像の二値化処理を行う
******************************************************************************/
void Background_blue(const char *name, const char *data_set)
{
    char dirname[10][100];
    char filename[10][100];

    /** ディレクトリの作成 **/
    sprintf(dirname[0], "%s/%s/42_change_images/%s/background", dir_path, name, data_set);
    mkdir(dirname[0], dirmode);

    sprintf(dirname[1], "%s/%s/42_change_images/%s/data", dir_path, name, data_set);
    mkdir(dirname[1], dirmode);

    /** BMPファイルの読み取り **/

    // 配列の初期化
    for (int i = 0; i < px_24_origin; i++)
    {
        sum[i] = 0;
        ave[i] = 0;
    }

    /** 配列の合計値の算出 **/

    for (int i = 1; i <= 100; i++)
    {
        sprintf(filename[0], "%s/data/%s/%s/blue_%04d.bmp", dir_path, name, data_set, i);

        // バイナリ読み込み
        Load_Bmp_24bit(filename[0], header_24bit, binary);

        for (int i = 0; i < px_24_origin; i++)
        {
            sum[i] += binary[i];
        }

        printf("Background : [%4d]\n", i);
    }

    /** 配列の平均値の算出 **/

    for (int i = 0; i < px_24_origin; i++)
    {
        ave[i] = sum[i] / 100;
    }

    /** BMPファイルの書き出し **/
    sprintf(filename[1], "%s/%s/42_change_images/%s/background/background.bmp", dir_path, name, data_set);
    printf("%s\n", filename[1]);

    fp = fopen(filename[1], "wb");

    // ヘッダー情報の書き込み
    fwrite(header_24bit, sizeof(unsigned char), 54, fp);

    // 画像情報の書き込み
    fwrite(ave, sizeof(unsigned char), px_24_origin, fp);

    fclose(fp);

    /** 背景処理の適用 **/

    // 背景画像の読み込み

    for (int i = 1; i <= 800; i++)
    {
        sprintf(filename[2], "%s/data/%s/%s/blue_%04d.bmp", dir_path, name, data_set, i);

        // 初期化
        for (int j = 0; j < px_24_origin; j++)
        {
            binary[j] = 0;
        }

        // バイナリ読み込み
        Load_Bmp_24bit(filename[2], header_24bit, binary);

        // 引き算
        for (int j = 0; j < px_24_origin; j++)
        {
            if (binary[j] - ave[j] < 0)
            {
                binary[j] = 0;
            }
            else
            {
                binary[j] = binary[j] - ave[j];
            }
        }

        // 背景補正
        for (int j = 0; j < px_24_origin; j++)
        {
            if (binary[j] <= 30)
            {
                binary[j] = 0;
            }
        }

        // BMPファイルの書き出し
        sprintf(filename[3], "%s/%s/42_change_images/%s/data/blue_%04d.bmp", dir_path, name, data_set, i);

        fp = fopen(filename[3], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_24bit, sizeof(unsigned char), 54, fp);

        // 画像情報の書き込み
        fwrite(binary, sizeof(unsigned char), px_24_origin, fp);

        fclose(fp);

        printf("Change : [%4d]\n", i);
    }
}

/******************************************************************************
PROGRAM : Background_green
概要：校正画像の二値化処理を行う
******************************************************************************/
void Background_green(const char *name, const char *data_set)
{
    char dirname[10][100];
    char filename[10][100];

    /** ディレクトリの作成 **/
    sprintf(dirname[0], "%s/%s/42_change_images/%s/background", dir_path, name, data_set);
    mkdir(dirname[0], dirmode);

    sprintf(dirname[1], "%s/%s/42_change_images/%s/data", dir_path, name, data_set);
    mkdir(dirname[1], dirmode);

    /** BMPファイルの読み取り **/

    // 配列の初期化
    for (int i = 0; i < px_24_origin; i++)
    {
        sum[i] = 0;
        ave[i] = 0;
    }

    /** 配列の合計値の算出 **/

    for (int i = 1; i <= 100; i++)
    {
        sprintf(filename[0], "%s/data/%s/%s/green_%04d.bmp", dir_path, name, data_set, i);

        // バイナリ読み込み
        Load_Bmp_24bit(filename[0], header_24bit, binary);

        for (int i = 0; i < px_24_origin; i++)
        {
            sum[i] += binary[i];
        }

        printf("Background : [%4d]\n", i);
    }

    /** 配列の平均値の算出 **/

    for (int i = 0; i < px_24_origin; i++)
    {
        ave[i] = sum[i] / 100;
    }

    /** BMPファイルの書き出し **/
    sprintf(filename[1], "%s/%s/42_change_images/%s/background/background.bmp", dir_path, name, data_set);
    printf("%s\n", filename[1]);

    fp = fopen(filename[1], "wb");

    // ヘッダー情報の書き込み
    fwrite(header_24bit, sizeof(unsigned char), 54, fp);

    // 画像情報の書き込み
    fwrite(ave, sizeof(unsigned char), px_24_origin, fp);

    fclose(fp);

    /** 背景処理の適用 **/

    // 背景画像の読み込み

    for (int i = 1; i <= 800; i++)
    {
        sprintf(filename[2], "%s/data/%s/%s/green_%04d.bmp", dir_path, name, data_set, i);

        // 初期化
        for (int j = 0; j < px_24_origin; j++)
        {
            binary[j] = 0;
        }

        // バイナリ読み込み
        Load_Bmp_24bit(filename[2], header_24bit, binary);

        // 引き算
        for (int j = 0; j < px_24_origin; j++)
        {
            if (binary[j] - ave[j] < 0)
            {
                binary[j] = 0;
            }
            else
            {
                binary[j] = binary[j] - ave[j];
            }
        }

        // 背景補正
        for (int j = 0; j < px_24_origin; j++)
        {
            if (binary[j] <= 30)
            {
                binary[j] = 0;
            }
        }

        // BMPファイルの書き出し
        sprintf(filename[3], "%s/%s/42_change_images/%s/data/green_%04d.bmp", dir_path, name, data_set, i);

        fp = fopen(filename[3], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_24bit, sizeof(unsigned char), 54, fp);

        // 画像情報の書き込み
        fwrite(binary, sizeof(unsigned char), px_24_origin, fp);

        fclose(fp);

        printf("Change : [%4d]\n", i);
    }
}