/******************************************************************************
PROGRAM : labeling_for_ptv
AUTHER  : Masatsugu Kitadai
DATE    : 2022/12/1
******************************************************************************/

// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <time.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

#include "../hpp/settings.hpp"

/** プロトタイプ宣言 **/
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[]);

/******************************************************************************/

int main()
{
    /* 保存ディレクトリの設定 */
    string name_str;
    cout << "Case Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    /* ディレクトリの作成 */
    char dirname[2][100];
    sprintf(dirname[0], "%s/%s/LLS_1/labeling_position_dat", dir_path, name);
    sprintf(dirname[1], "%s/%s/LLS_1/labeling_image_png", dir_path, name);
    mkdir(dirname[0], dirmode);
    mkdir(dirname[1], dirmode);

    // 8bit.bmp
    unsigned char ary[size_px];
    int ary_label[size_px];
    int ary_buf[20];

    float sum[2];
    float ave[2];
    int position;
    int area[1000];
    int count = 0;
    int min;
    int tmp = 0;

    int maximum_x[1000], maximum_y[1000];
    int minimum_x[1000], minimum_y[1000];

    // 最大・最小の座標
    float point_x[4][1000];
    float point_y[4][1000];

    // 中心座標
    int x[1000], y[1000];

    // 計算用のバッファ
    float buf[100];

    // 軸の設定

    // range x
    float x_min = 0;
    float x_max = width_px;

    // range y
    float y_min = 0;
    float y_max = height_px;

    // label
    const char *xxlabel = "x [px]";
    const char *yylabel = "y [px]";

    int num = 0; // ラベリングの番号
    int label = 0;

    for (int m = 1; m <= data_num; m++)
    {
        // 粒子画像の読み込み
        char readfile[100];
        sprintf(readfile, "%s/%s/LLS_1/particle_image_bmp/%d.bmp", dir_path, name, m);
        Load_Bmp_8bit(readfile, header_8bit, ary);

        /** ラベリング **/

        // 配列の可視化
        for (int i = 0; i < size_px; i++)
        {
            ary_label[i] = 0;
        }

        for (int i = 0; i < 100; i++)
        {
            buf[i] = 0;
        }

        for (int i = 0; i < 1000; i++)
        {
            area[i] = 0;
        }

        // ラベル番号の初期化
        int n = 0;

        for (int i = 0; i < size_px; i++)
        {
            // 変数の初期化
            label = 0;
            tmp = 0;

            if (ary[i] >= 20)
            // if (ary[i] != 0)
            {
                // エリア内のラベルの確認
                for (int j = -1; j <= 0; j++)
                {
                    for (int k = -1; k <= 1; k++)
                    {
                        // 調べる座標の計算
                        position = i + width_px * j + k;

                        // 座標のオーバーフローの回避
                        if (position < 0)
                        {
                            position = 0;
                        }
                        else if (position > size_px)
                        {
                            position = size_px;
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
                    for (int j = -1; j <= 0; j++)
                    {
                        for (int k = -1; k <= 1; k++)
                        {
                            // 調べる座標の計算
                            position = i + width_px * j + k;

                            // 座標のオーバーフローの回避
                            if (position < 0)
                            {
                                position = 0;
                            }
                            else if (position > size_px)
                            {
                                position = size_px;
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

        // 面積の計算
        for (int i = 0; i < size_px; i++)
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
        int count_1 = 1;
        int count_2 = 0;

        // 不必要なラベル番号
        int trash[100];

        // 配列初期化
        for (int i = 0; i < 100; i++)
        {
            trash[i] = 0;
        }

        // 閾値より高い面積のラベルを取得
        for (int i = 1; i <= n; i++)
        {
            if (area[i] > 5)
            {
                buf[count_1] = i;
                count_1 += 1;
            }
            else
            {
                trash[count_2] = i;
                count_2 += 1;
            }
        }

        // 移し替え
        count = count_1;

        // 必要ないラベル番号を排除
        for (int i = 0; i < count_2; i++)
            for (int j = 0; j < size_px; j++)
            {
                if (trash[i] == ary_label[j])
                {
                    ary_label[j] = 0;
                }
            }

        // ラベルの再張替え
        for (int i = 1; i < count; i++)
            for (int j = 0; j < size_px; j++)
            {
                if (ary_label[j] == buf[i])
                {
                    ary_label[j] = i;
                }
            }

        // 配列の初期化
        for (int i = 0; i < 1000; i++)
        {
            area[i] = 0;
        }

        // 面積の再計算
        for (int i = 0; i < size_px; i++)
            for (int j = 0; j < n; j++)
            {
                if (ary_label[i] == j)
                {
                    area[j] = area[j] + 1;
                }
            }

        // n の再カウント
        tmp = 0;

        for (int i = 1; i <= n; i++)
        {
            if (area[i] > 5)
            {
                // 面積を持つ数の確認
                tmp = tmp + 1;
                // printf("area[%d] = %d\n", i, area[i]);
            }
        }

        // n の書き換え
        n = tmp;

        // 配列の初期化
        for (int i = 0; i < 1000; i++)
        {
            x[i] = 0;
            y[i] = 0;
        }

        float x_tmp, y_tmp;

        /** 配列への格納 **/
        for (int i = 1; i <= n; i++)
        {
            float x_sum = 0;
            float y_sum = 0;
            int count_label = 0;

            for (int j = 0; j < size_px; j++)
            {
                if (i == ary_label[j])
                {
                    x_tmp = j % width_px;
                    y_tmp = j / width_px;

                    // 合計
                    x_sum = x_sum + x_tmp;
                    y_sum = y_sum + y_tmp;

                    count_label = count_label + 1;
                }
            }

            x[i] = x_sum / count_label;
            y[i] = y_sum / count_label;
        }

        /** ファイルの書き出しと描画 **/

        // 各座標ごとに書き出し
        char writefile[100];
        sprintf(writefile, "%s/%s/LLS_1/labeling_position_dat/%d.dat", dir_path, name, m);
        fp = fopen(writefile, "w");

        for (int i = 1; i <= n; i++)
        {
            position = x[i] + width_px * y[i];
            if (position < size_px && position >= 0)
            {
                fprintf(fp, "%d\t%d\t%d\n", x[i], y[i], position);
            }
        }

        fclose(fp);

        if (m <= 100)
        {
            // Gnuplot 呼び出し
            if ((gp = popen("gnuplot", "w")) == NULL)
            {
                printf("gnuplot is not here!\n");
                exit(0); // gnuplotが無い場合、異常ある場合は終了
            }

            /** Gnuplot **/
            char graphname[100], graphtitle[100], backimage[100];
            sprintf(graphname, "%s/%s/LLS_1/labeling_image_png/%d.png", dir_path, name, m);
            sprintf(backimage, "%s/%s/LLS_1/particle_image_png/%d.png", dir_path, name, m);
            sprintf(graphtitle, "labeling plane [%03d]", m);

            // fprintf(gp, "set terminal png enhanced size 800, 800 font 'Times New Roman, 20'\n");
            fprintf(gp, "set terminal png enhanced size 800, 500 font 'Times New Roman, 20'\n");
            fprintf(gp, "set size ratio -1\n");

            // 出力ファイル
            fprintf(gp, "set output '%s'\n", graphname);

            // 非表示
            fprintf(gp, "unset key\n");

            // 軸の範囲
            fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
            fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

            // グラフタイトル
            fprintf(gp, "set title '%s' offset 0.0, -0.5\n", graphtitle);

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
            // fprintf(gp, "plot '%s' using 1:2 with points lc 'red' pt 1 ps 1 notitle\n", readfile[2]);
            fprintf(gp, "plot '%s' binary filetype=png with rgbimage, '%s' using 1:2 with points lc 'red' pt 1 ps 1 notitle\n", backimage, writefile);

            fflush(gp); // Clean up Data

            fprintf(gp, "exit\n"); // Quit gnuplot

            pclose(gp);
        }

        printf("Labeling blue\t%4d\tn = %d\n", m, n);
    }

    return 0;
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

    fclose(fp); // ファイルを閉じる
}