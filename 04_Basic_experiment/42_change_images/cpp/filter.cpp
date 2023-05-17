/******************************************************************************
PROGRAM : 04_separate
AUTHER  : Masatsugu Kitadai
DATE    : 2022/8/24
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
void Filter(const char *name);

/*****************************************************************************/

#include "../hpp/settings.hpp"

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

    Filter(name);

    return 0;
}

/******************************************************************************
PROGRAM : Filter
DATE    : 2023/05/12
******************************************************************************/
void Filter(const char *name)
{
    char filename[10][100];
    float buf[10];

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/42_change_images/filter", dir_path, name);
    mkdir(dirname, dirmode);

    // bmp 1次元配列
    unsigned char binary[px_24_origin] = {0};
    unsigned char binary_r[px_8_origin] = {0}; // 赤色用
    unsigned char binary_g[px_8_origin] = {0}; // 緑色用
    unsigned char binary_b[px_8_origin] = {0}; // 青色用

    // 割合確認
    int counter[2];
    float sum_r[2];
    float sum_g[2];
    float sum_b[2];

    float ave_r[3];
    float ave_g[3];
    float ave_b[3];

    // 初期化
    for (int i = 0; i < 2; i++)
    {
        counter[i] = 0;
        sum_r[i] = 0;
        sum_g[i] = 0;
        sum_b[i] = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        ave_r[i] = 0;
        ave_g[i] = 0;
        ave_b[i] = 0;
    }

    // 青色
    for (int n = 1; n <= 100; n++)
    {
        sprintf(filename[0], "%s/%s/42_change_images/blue/data/blue_%04d.bmp", dir_path, name, n);

        // バイナリ読み込み
        Load_Bmp_24bit(filename[0], header_24bit, binary);
        printf("filter:Blue = %d\n", n);

        int tmp = 0;

        // 色ごとのデータの格納
        for (int i = 0; i < px_8_origin; i++)
        {
            binary_b[i] = binary[tmp];     // blue
            binary_g[i] = binary[tmp + 1]; // green
            binary_r[i] = binary[tmp + 2]; // red
            tmp += 3;
        }

        /** 混在成分の調査 **/

        // 青成分についての調査
        for (int i = 0; i < px_8_origin; i++)
        {
            if (binary_b[i] > 0)
            {
                sum_r[0] = sum_r[0] + binary_r[i];
                sum_g[0] = sum_g[0] + binary_g[i];
                sum_b[0] = sum_b[0] + binary_b[i];
                counter[0] = counter[0] + 1;
            }
        }
    }

    // 結果の表示
    ave_r[0] = sum_r[0] / sum_r[1] * 100; // R/R [%]
    ave_r[1] = sum_g[0] / sum_r[1] * 100; // G/R [%]
    ave_r[2] = sum_b[0] / sum_r[1] * 100; // B/R [%]

    ave_g[0] = sum_r[0] / sum_g[1] * 100; // R/G [%]
    ave_g[1] = sum_g[0] / sum_g[1] * 100; // G/G [%]
    ave_g[2] = sum_b[0] / sum_g[1] * 100; // B/G [%]

    ave_b[0] = sum_r[0] / sum_b[0] * 100; // R/B [%]
    ave_b[1] = sum_g[0] / sum_b[0] * 100; // G/B [%]
    ave_b[2] = sum_b[0] / sum_b[0] * 100; // B/B [%]

    // 値の一時保管
    buf[0] = ave_b[0];
    buf[1] = ave_b[1];
    buf[2] = ave_b[2];

    // 初期化
    for (int i = 0; i < 2; i++)
    {
        sum_r[i] = 0;
        sum_g[i] = 0;
        sum_b[i] = 0;
    }

    for (int i = 0; i < 3; i++)
    {
        ave_r[i] = 0;
        ave_g[i] = 0;
        ave_b[i] = 0;
    }

    // 緑色
    for (int n = 1; n < 100; n++)
    {
        sprintf(filename[1], "%s/%s/42_change_images/green/data/green_%04d.bmp", dir_path, name, n);

        // 初期化
        for (int i = 0; i < px_24_origin; i++)
        {
            binary[i] = 0;
        }

        for (int i = 0; i < px_8_origin; i++)
        {
            binary_r[i] = 0;
            binary_b[i] = 0;
            binary_g[i] = 0;
        }

        // バイナリ読み込み
        Load_Bmp_24bit(filename[1], header_24bit, binary);
        printf("filter:Green = %d\n", n);

        int tmp = 0;

        // 色ごとのデータの格納
        for (int i = 0; i < px_8_origin; i++)
        {
            binary_b[i] = binary[tmp];     // blue
            binary_g[i] = binary[tmp + 1]; // green
            binary_r[i] = binary[tmp + 2]; // red
            tmp += 3;
        }

        /** 混在成分の調査 **/

        // 青成分についての調査
        for (int i = 0; i < px_8_origin; i++)
        {
            if (binary_g[i] > 0)
            {
                sum_r[1] = sum_r[1] + binary_r[i];
                sum_g[1] = sum_g[1] + binary_g[i];
                sum_b[1] = sum_b[1] + binary_b[i];
                counter[1] = counter[1] + 1;
            }
        }
    }

    // 結果の表示
    ave_r[0] = sum_r[1] / sum_r[1] * 100; // R/R [%]
    ave_r[1] = sum_g[1] / sum_r[1] * 100; // G/R [%]
    ave_r[2] = sum_b[1] / sum_r[1] * 100; // B/R [%]

    ave_g[0] = sum_r[1] / sum_g[1] * 100; // R/G [%]
    ave_g[1] = sum_g[1] / sum_g[1] * 100; // G/G [%]
    ave_g[2] = sum_b[1] / sum_g[1] * 100; // B/G [%]

    ave_b[0] = sum_r[1] / sum_b[1] * 100; // R/B [%]
    ave_b[1] = sum_g[1] / sum_b[1] * 100; // G/B [%]
    ave_b[3] = sum_b[1] / sum_b[1] * 100; // B/B [%]

    // (1) Blue
    printf("/// BLUE  ///////////////////////////////////\n");
    printf("count : %d\n", counter[0]);
    printf("R/B =\t%.3f\t[%%]\n", buf[0]);
    printf("G/B =\t%.3f\t[%%]\n", buf[1]);
    printf("B/B =\t%.3f\t[%%]\n", buf[2]);

    // (2) Green
    printf("/// GREEN //////////////////////////////////\n");
    printf("count : %d\n", counter[1]);
    printf("R/G =\t%.3f\t[%%]\n", ave_g[0]);
    printf("G/G =\t%.3f\t[%%]\n", ave_g[1]);
    printf("B/G =\t%.3f\t[%%]\n", ave_g[2]);

    printf("////////////////////////////////////////////\n");

    // 書き出し
    sprintf(filename[2], "%s/%s/42_change_images/filter/filter.dat", dir_path, name);

    fp = fopen(filename[2], "w");

    fprintf(fp, "%f\t%f\t%f\n", buf[0], buf[1], buf[2]);
    fprintf(fp, "%f\t%f\t%f\n", ave_g[0], ave_g[1], ave_g[2]);

    fclose(fp);
}