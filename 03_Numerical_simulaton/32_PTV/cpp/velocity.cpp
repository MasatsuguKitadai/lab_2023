/******************************************************************************
PROGRAM : 12_PTV
AUTHER  : Masatsugu Kitadai
DATE    : 2022/12/13
******************************************************************************/
// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>

using namespace std;
#include <vector>
#include <algorithm>
#include <iostream>

// 自作設定ファイル
#include "../hpp/settings.hpp"
#include "../../parameters.hpp"
#include "../hpp/loadbmp_8bit.hpp"
#include "../hpp/loadbmp_24bit.hpp"

/*****************************************************************************/

const float grid_size = 10;

const int mesh_y = width / grid_size;
const int mesh_z = height / grid_size;
float value_y[mesh_y][mesh_z], value_z[mesh_y][mesh_z];
int count_mesh[mesh_y][mesh_z];
float position_y, position_z;

/******************************************************************************/

int main()
{
    for (i = 0; i < mesh_y; i++)
    {
        for (j = 0; j < mesh_z; j++)
        {
            value_y[i][j] = 0;
            value_z[i][j] = 0;
            count_mesh[i][j] = 0;
        }
    }

    // 配列の初期化
    counter = 0;

    int vector_num = 0;

    for (j = 1; j < number - delta; j++)
    {
        // ファイルの読み取り
        sprintf(filename[1], "result/03/ptv/vector/%d.dat", j);

        fp_2 = fopen(filename[1], "r");

        while ((fscanf(fp_2, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5], &buf[6], &buf[7], &buf[8])) != EOF)
        {
            // ベクトルの合計値の計算
            position_y = buf[0];
            position_z = buf[1];

            for (i = 0; i < mesh_y; i++)
            {
                if (i * grid_size <= position_y && position_y < (i + 1) * grid_size)
                {
                    for (k = 0; k < mesh_z; k++)
                    {
                        if (k * grid_size <= position_z && position_z < (k + 1) * grid_size)
                        {
                            value_y[i][k] = value_y[i][k] + buf[2];
                            value_z[i][k] = value_z[i][k] + buf[3];
                            count_mesh[i][k] = count_mesh[i][k] + 1;
                            vector_num += 1;
                            break;
                        }
                    }
                    break;
                }
            }
        }

        fclose(fp_2);
        // number（データの使用枚数）に注意!!
    }

    printf("Vector num : %d\n", vector_num);

    float v_y_value = 0;
    float v_z_value = 0;
    float v_value = 0;
    float all_ave_value = 0;
    float all_ave_y = 0;
    float all_ave_z = 0;

    counter = 0;

    sprintf(filename[0], "result/03/ptv/velocity/velocity.dat");

    fp = fopen(filename[0], "w");

    for (i = 0; i < mesh_y; i++)
    {
        for (j = 0; j < mesh_z; j++)
        {
            if (count_mesh[i][j] != 0)
            {
                // 平均値の算出
                value_y[i][j] = value_y[i][j] / count_mesh[i][j];
                value_z[i][j] = value_z[i][j] / count_mesh[i][j];

                // ベクトルの正規化
                v_value = sqrt(value_y[i][j] * value_y[i][j] + value_z[i][j] * value_z[i][j]);
            }
            else
            {
                value_y[i][j] = 0;
                value_z[i][j] = 0;
                v_value = 0;
            }

            // ベクトルの始点
            position_y = i * grid_size + (grid_size / 2);
            position_z = j * grid_size + (grid_size / 2);

            // fprintf(fp, "%.0f\t%.0f\t%f\t%f\t%f\n", position_y, position_z, value_y[i][j], value_z[i][j], v_value);
            fprintf(fp, "%.0f\t%.0f\t%f\t%f\t%f\n", position_y, position_z, value_y[i][j] * 3, value_z[i][j] * 3, v_value); // 資料画像用にベクトルの長さを誇張
        }
    }

    fclose(fp);

    /** Gnuplot **/

    // ファイル名
    const char filename_ptv[] = "result/03/ptv/velocity/velocity.dat";
    sprintf(graphname[0], "result/03/ptv/velocity/velocity.svg");
    sprintf(graphtitle[0], "Simulation Data [Vector]");

    // 軸の設定

    // range x
    // float x_min = 50;
    // float x_max = width - 50;

    // 資料画像用
    float x_min = 250;
    float x_max = 550;

    // range y
    float y_min = 50;
    float y_max = height - 50;

    // range color
    float cb_min = 0;
    // float cb_max = 15;
    float cb_max = 4;

    // label
    const char *xxlabel = "y [px]";
    const char *yylabel = "z [px]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    // fprintf(gp, "set terminal svg enhanced size 1600, 800 font 'Times New Roman, 24'\n");
    fprintf(gp, "set terminal svg enhanced size 800, 800 font 'Times New Roman, 24'\n");
    // fprintf(gp, "set terminal png enhanced size 2000, 1600 font 'Times New Roman, 24'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname[0]);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s'\n", graphtitle[0]);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.0\n");
    fprintf(gp, "set ylabel offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.0\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:2:3:4:5 with vectors lc palette lw 1 notitle\n", filename_ptv);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);

    return 0;
}