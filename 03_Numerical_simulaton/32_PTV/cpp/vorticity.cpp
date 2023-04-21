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
float value_y[mesh_y][mesh_z], value_z[mesh_y][mesh_z], vorticity[mesh_y][mesh_z];
int count_mesh[mesh_y][mesh_z];
float position_y, position_z;

// ファイル名
char filename_ptv[100];

// range x
float x_min = 50;
float x_max = width - 50;

// range y
float y_min = 50;
float y_max = height - 50;

// range color
float cb_min = -0.5;
float cb_max = 0.5;

// label
const char *xxlabel = "y [px]";
const char *yylabel = "z [px]";

/******************************************************************************/

int main()
{
    for (i = 0; i < mesh_y; i++)
    {
        for (j = 0; j < mesh_z; j++)
        {
            value_y[i][j] = 0;
            value_z[i][j] = 0;
            vorticity[i][j] = 0;
            count_mesh[i][j] = 0;
        }
    }

    // 配列の初期化
    counter = 0;

    for (j = 1; j < number - delta; j++)
    {
        // ファイルの読み取り
        sprintf(filename[0], "result/03/ptv/vector/%d.dat", j);

        fp_2 = fopen(filename[0], "r");

        while ((fscanf(fp_2, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5], &buf[6], &buf[7], &buf[8])) != EOF)
        {
            // ベクトルの合計値の計算
            position_y = buf[0];
            position_z = buf[1];

            for (i = 0; i < mesh_y; i++)
            {
                if (i * grid_size - 5 <= position_y && position_y < i * grid_size + 5)
                {
                    for (k = 0; k < mesh_z; k++)
                    {
                        if (k * grid_size - 5 <= position_z && position_z < k * grid_size + 5)
                        {
                            value_y[i][k] = value_y[i][k] + buf[2];
                            value_z[i][k] = value_z[i][k] + buf[3];
                            count_mesh[i][k] = count_mesh[i][k] + 1;
                            break;
                        }
                    }
                    break;
                }
            }
        }

        fclose(fp_2);

        // printf("[%d] - [%4d]\n", j);
    }

    float v_y_value = 0;
    float v_z_value = 0;
    float all_ave_value = 0;
    float all_ave_y = 0;
    float all_ave_z = 0;

    counter = 0;

    /* アベレージ */
    for (i = 0; i < mesh_y; i++)
    {
        for (j = 0; j < mesh_z; j++)
        {
            if (count_mesh[i][j] != 0)
            {
                // 平均値の算出
                value_y[i][j] = value_y[i][j] / count_mesh[i][j];
                value_z[i][j] = value_z[i][j] / count_mesh[i][j];
            }
            else
            {
                value_y[i][j] = 0;
                value_z[i][j] = 0;
            }
        }
    }

    /* 渦度 */
    for (i = 0; i < mesh_y; i++)
    {
        for (j = 0; j < mesh_z; j++)
        {
            vorticity[i][j] = (value_z[i + 1][j] - value_z[i - 1][j]) / grid_size - (value_y[i][j + 1] - value_y[i][j - 1]) / grid_size;
        }
    }

    /* 渦度の書き出し */
    sprintf(filename[1], "result/03/ptv/vorticity/vorticity.dat");
    fp = fopen(filename[1], "w");
    for (i = 0; i < mesh_y; i++)
    {
        for (j = 0; j < mesh_z; j++)
        {
            // ベクトルの始点
            position_y = i * grid_size;
            position_z = j * grid_size;

            fprintf(fp, "%.0f\t%.0f\t%f\t%f\t%f\n", position_y, position_z, value_y[i][j], value_z[i][j], vorticity[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    /* 渦度プロファイルの書き出し */
    sprintf(filename[2], "result/03/ptv/vorticity/vorticity_profile.dat");
    fp = fopen(filename[2], "w");
    for (i = 0; i < mesh_y; i++)
        for (j = 0; j < mesh_z; j++)
        {
            // ベクトルの始点
            position_y = i * grid_size;
            position_z = j * grid_size;

            if (position_z == height / 2)
            {
                fprintf(fp, "%.0f\t%.0f\t%f\n", position_y, position_z, vorticity[i][j]);
            }
        }
    fclose(fp);

    /* 渦度 */

    /** Gnuplot **/

    sprintf(filename_ptv, "result/03/ptv/vorticity/vorticity.dat");
    sprintf(graphname[0], "result/03/ptv/vorticity/vorticity.svg");
    sprintf(graphtitle[0], "Simulation Data [Vorticity]");

    // 軸の設定

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 1600, 800 font 'Times New Roman, 24'\n");
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
    fprintf(gp, "set view map\n");

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
    fprintf(gp, "splot '%s' using 1:2:5 with pm3d notitle\n", filename_ptv);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);

    return 0;
}