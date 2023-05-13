/******************************************************************************
PROGRAM : binarization
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/11
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
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[]);
void Load_Bmp_24bit(const char file_name[], unsigned char header[], unsigned char binary[]);

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

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s", dir_path, name);
    mkdir(dirname, dirmode);

    Binarization(name);

    return 0;
}

/******************************************************************************
PROGRAM : Binarization
概要：校正画像の二値化処理を行う
******************************************************************************/

void Binarization(const char *name)
{
    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/41_binarization", dir_path, name);
    mkdir(dirname, dirmode);

    /** BMPファイルの読み取り **/

    // bmp 1次元配列
    unsigned char binary_8bit[px_8_origin];   // 1色
    unsigned char binary_24bit[px_24_origin]; // 3色

    char filename[3][100];

    sprintf(filename[0], "header/%dx%d_8bit.bmp", width_origin, height_origin);
    sprintf(filename[1], "%s/data/%s/calibration/calibration_0001.bmp", dir_path, name);
    sprintf(filename[2], "%s/%s/binarization/calibration_8bit.bmp", dir_path, name);

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
        if (binary_24bit[x] < border)
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
PROGRAM : Ditect_threhold
概要：大津の二値化法によってしきい値を見つける
******************************************************************************/
int Ditect_threhold(unsigned char binary_8bit[])
{
    /** ヒストグラムの計算 **/
    int histgram[256] = {0}; // 8bit ヒストグラム用配列 [-]
    for (int i = 0; i < px_8_origin; i++)
    {
        if (binary_8bit[i] == i)
        {
            histgram[i] += 1;
        }
    }

    /** 確立の計算 **/
    float probability[256] = {0}; // 各輝度値の確率 [-]
    for (int i = 0; i < px_8_origin; i++)
    {
        probability[i] = histgram[i] / px_8_origin;
    }

    /** クラスごとの計算 **/
    for (int i = 0; i < px_8_origin; i++)
    {
        float sum_0 = 0;
        float sum_1 = 0;

        // クラス分け
        for (int j = 0; j < px_8_origin; j++)
        {
            if (j <= i)
            {
                sum_0 += histgram[j];
            }
            else
            {
                sum_1 += histgram[j];
            }
        };

        // クラス平均値の計算
        float average_0 = average_0 / px_8_origin;
        float average_1 = average_1 / px_8_origin;

        // クラス確率の計算
        float probability_0 = average_0 / px_8_origin;
        float probability_1 = average_1 / px_8_origin;
    }

    float threhold = 0;

    return threhold;
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