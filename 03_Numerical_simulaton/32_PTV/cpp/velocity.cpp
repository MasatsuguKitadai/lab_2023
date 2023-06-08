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
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;
#include "../hpp/settings.hpp"

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

const int mesh_y = width_px / grid_size;
const int mesh_z = height_px / grid_size;
float value_y[mesh_y][mesh_z], value_z[mesh_y][mesh_z];
int count_mesh[mesh_y][mesh_z];
float position_y, position_z;

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
    sprintf(dirname[0], "%s/%s/PTV/PTV_velocity_dat", dir_path, name);
    sprintf(dirname[1], "%s/%s/PTV/PTV_velocity_svg", dir_path, name);
    mkdir(dirname[0], dirmode);
    mkdir(dirname[1], dirmode);

    for (int i = 0; i < mesh_y; i++)
        for (int j = 0; j < mesh_z; j++)
        {
            value_y[i][j] = 0;
            value_z[i][j] = 0;
            count_mesh[i][j] = 0;
        }

    // 配列の初期化
    int counter = 0;
    int vector_num = 0;

    for (int k = 1; k < data_num - delta_n; k++)
    {
        // ファイルの読み取り
        char readfile[100];
        sprintf(readfile, "%s/%s/PTV/PTV_vector_dat/%d.dat", dir_path, name, k);

        float buf[10]; // 読み込み用のバッファ
        fp = fopen(readfile, "r");
        while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5])) != EOF)
        {
            // ベクトルの合計値の計算
            position_y = buf[0];
            position_z = buf[1];

            if (border_max >= buf[5] && buf[5] >= border_min)
            {
                for (int i = 0; i < mesh_y; i++)
                {
                    if (i * grid_size - 5 <= position_y && position_y < i * grid_size + 5)
                    {
                        for (int j = 0; j < mesh_z; j++)
                        {
                            if (j * grid_size - 5 <= position_z && position_z < j * grid_size + 5)
                            {
                                value_y[i][j] = value_y[i][j] + buf[2];
                                value_z[i][j] = value_z[i][j] + buf[3];
                                count_mesh[i][j] = count_mesh[i][j] + 1;
                                vector_num += 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        fclose(fp);
        // data_num（データの使用枚数）に注意!!
    }

    printf("Vector num : %d\n", vector_num);

    float v_y_value = 0;
    float v_z_value = 0;
    float v_value = 0;
    float all_ave_value = 0;
    float all_ave_y = 0;
    float all_ave_z = 0;

    counter = 0;

    char writefile[100];
    sprintf(writefile, "%s/%s/PTV/PTV_velocity_dat/velocity_%s.dat", dir_path, name, name);

    const float time = delta_n / shutter_speed;

    fp = fopen(writefile, "w");

    for (int i = 0; i < mesh_y; i++)
    {
        for (int j = 0; j < mesh_z; j++)
        {
            if (count_mesh[i][j] != 0)
            {
                // 平均値の算出
                value_y[i][j] = value_y[i][j] / count_mesh[i][j] * width_mm / width_px;               // y方向の移動量 [mm]
                value_z[i][j] = value_z[i][j] / count_mesh[i][j] * width_mm / width_px;               // z方向の移動量 [mm]
                v_value = sqrt(value_y[i][j] * value_y[i][j] + value_z[i][j] * value_z[i][j]) / time; // 全体の速度 [mm/s]

                // 誤ベクトル処理
                if (v_value >= 2.5)
                {
                    value_y[i][j] = 0;
                    value_z[i][j] = 0;
                    v_value = 0;
                }
            }
            else
            {
                value_y[i][j] = 0;
                value_z[i][j] = 0;
                v_value = 0;
            }

            // ベクトルの始点
            position_y = i * grid_size * (float)width_mm / width_px + (width_shot_center - width_mm / 2);
            position_z = j * grid_size * (float)height_mm / height_px + (height_shot_center - height_mm / 2);
            fprintf(fp, "%f\t%f\t%f\t%f\t%f\n", position_y, position_z, value_y[i][j] * (width_px / width_mm), value_z[i][j] * (width_px / width_mm), v_value); // 資料画像用にベクトルの長さを誇張
        }
    }

    fclose(fp);

    /** Gnuplot **/

    // ファイル名
    char graphfile[100], graphtitle[100];
    sprintf(graphfile, "%s/%s/PTV/PTV_velocity_svg/velocity_%s.svg", dir_path, name, name);
    sprintf(graphtitle, "Velocity [mm/s]");

    // 軸の設定

    // range x
    // float x_min = 5;
    // float x_max = 35;

    // range y
    // float y_min = 5;
    // float y_max = 35;

    // // range x
    const float x_min = width_shot_center - 7.5;
    const float x_max = width_shot_center + 7.5;

    // // range y
    const float y_min = height_shot_center - 7.5;
    const float y_max = height_shot_center + 7.5;

    // range color
    float cb_min = 0;
    float cb_max = 2.5;
    // float cb_max = 10;

    // label
    const char *xxlabel = "y [mm]";
    const char *yylabel = "z [mm]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    // fprintf(gp, "set terminal svg enhanced size 1000, 1000 font 'Times New Roman, 16'\n");
    fprintf(gp, "set terminal svg enhanced size 500, 500 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s'\n", graphtitle);

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
    fprintf(gp, "plot '%s' using 1:2:3:4:5 with vectors lc palette lw 1 notitle\n", writefile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);

    return 0;
}