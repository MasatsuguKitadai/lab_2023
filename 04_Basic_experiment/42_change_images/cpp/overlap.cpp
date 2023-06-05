/******************************************************************************
PROGRAM : overlap
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
void Overlap(const char *name, const char *dat_set);

/*****************************************************************************/

#include "../hpp/settings.hpp"

// bmp 1次元配列
unsigned char binary_b[px_8];

// オーバーラップさせる枚数
const int overlap = 3;

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

    Overlap(name, data_set);

    return 0;
}

/******************************************************************************
PROGRAM : overlap
DATE    : 2023/05/12
******************************************************************************/
void Overlap(const char *name, const char *data_set)
{
    float buf[10];
    char filename[10][100];

    /** ディレクトリの作成 **/
    char dirname[100];
    sprintf(dirname, "%s/%s/42_change_images/%s/overlap_blue", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    for (int n = 1; n <= number - overlap; n++)
    {
        unsigned char binary_tmp[px_8] = {0};

        /** 画像のオーバーラップ **/
        for (int i = 0; i < overlap; i++)
        {
            // 読み込むファイル
            char filename_tmp[100];
            sprintf(filename_tmp, "%s/%s/42_change_images/%s/blue/%04d.bmp", dir_path, name, data_set, n + i);

            // バイナリの読み込み
            Load_Bmp_8bit(filename_tmp, header_8bit, binary_b);

            // オーバーラップ
            for (int j = 0; j < px_8; j++)
            {
                if (binary_b[j] > binary_tmp[j])
                {
                    binary_tmp[j] = binary_b[j];
                }
            }
        }

        /** BMPファイルの書き出し **/

        // blue
        sprintf(filename[2], "%s/%s/42_change_images/%s/overlap_blue/%04d.bmp", dir_path, name, data_set, n);
        fp = fopen(filename[2], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

        // 画像情報の書き込み
        fwrite(binary_tmp, sizeof(unsigned char), px_8, fp);
        fclose(fp);

        printf("overlap:%d\n", n);
    }
}