/******************************************************************************
PROGRAM : functions.h
AUTHER  : Masatsugu Kitadai
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
using namespace std;

/******************************************************************************
FUNCTION : Gaussian
概要：1次元の正規分布を与える
 IN ：x：調べる値，mu：平均値，sigma：分散の二乗根
OUT ：X：確率変数
******************************************************************************/
float Gaussian(float x, float mu, float sigma)
{
    // 円周率 pi
    const float pi = 4 * atan(1.0);

    // ガウス分布に従った値を返す
    float X = exp(-1 * (x - mu) * (x - mu) / (2 * sigma * sigma));

    return X;
}

/******************************************************************************
FUNCTION : Gaussian_2D
概要：2次元の正規分布を与える
 IN ：x：x方向の調べる値，mu_x：x方向の平均値，sigma_x：x方向の分散の二乗根
      y：y方向の調べる値，mu_y：y方向の平均値，sigma_y：y方向の分散の二乗根
OUT ：X：確率変数
******************************************************************************/
float Gaussian_2D(float x, float y, float mu_x, float mu_y, float sigma)
{
    // 円周率 pi
    const float pi = 4 * atan(1.0);

    // ガウス分布に従った値を返す
    float X = exp(-1 * ((x - mu_x) * (x - mu_x) + (y - mu_y) * (y - mu_y)) / (2 * sigma * sigma));

    return X;
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
FUNCTION : Load_bmp_8bit
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

/******************************************************************************
FUNCTION : Lagrange_Interpolation
概要：ラグランジュの二次補完を行う
 IN ：x：補完対象の値，x_data：補完に用いるx軸データ(3点)，y_data：補完に用いるy軸データ(3点)
OUT ：float / ラグランジュ補完の値を与える
******************************************************************************/
float Lagrange_Interpolation(float x, const vector<float> &x_data, const vector<float> &y_data)
{
    /* ラグランジュ補完の計算 */

    int n = x_data.size();
    float result = 0.0;

    for (int i = 0; i < n; i++)
    {
        float term = 1.0;
        for (int j = 0; j < n; j++)
        {
            if (j != i)
            {
                term *= (x - x_data[j]) / (x_data[i] - x_data[j]);
            }
        }
        result += y_data[i] * term;
    }

    return result;
}

/******************************************************************************
FUNCTION : Find_Closest_Value
概要：ラグランジュの二次補完を行う
 IN ：x：対象の値，x_data：最近値を求めるx軸データ
OUT ：int：１番近いデータのインデックス
******************************************************************************/
int Find_Closest_Value(float x, const vector<float> &x_data)
{
    int n = x_data.size();
    if (n == 0)
    {
        return -1; // エラー処理
    }

    int index = 0;                          // 配列の番号
    float minDistance = abs(x - x_data[0]); //

    for (int i = 1; i < n; i++)
    {
        // 距離の計算
        float distance = abs(x - x_data[i]);

        // 値の更新
        if (distance < minDistance)
        {
            index = i;
            minDistance = distance;
        }
    }

    return index;
}

/******************************************************************************
FUNCTION : Progress_meter
概要     ：進捗割合を表示する
******************************************************************************/
int Progress_meter(const char program_name[], int i, int max, int progress_count)
{
    /* メーターの表示 */
    if (i == 0)
    {
        printf("\n【%s】\n", program_name);
        printf("0    10   20   30   40   50   60   70   80   90  100 [%%]\n"
               "|----|----|----|----|----|----|----|----|----|----|\n"
               "*");
        fflush(stdout);
        progress_count += 1;
    }

    /* 進捗率の計算 */
    const float progress_ratio = (float)i / (float)max * 50.0; // [%]
    if (progress_ratio > progress_count)
    {
        printf("*");
        fflush(stdout);
        progress_count += 1;
    }

    /* 完了 */
    if (i == max - 1)
    {
        printf("*\n\n");
    }

    return progress_count;
}