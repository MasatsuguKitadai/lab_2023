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

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/** ディレクトリ **/
const char dir_path[] = "/mnt/d/workspace_HDD/03_numerical_simulation/";

/** 各種パラメータ **/
const int data = 800;                     // 画像の枚数 [-]
const int width_px = 800;                 // 画像の横幅 [px]
const int height_px = 800;                // 画像の縦幅 [px]
const int size_px = width_px * height_px; // 画像の画素数 [px]
const int delta_n = 10;                   // 対応させる時刻差 (枚)
const float grid_size = 10;               // 格子点の大きさ [px]

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
    {
        for (int j = 0; j < mesh_z; j++)
        {
            value_y[i][j] = 0;
            value_z[i][j] = 0;
            count_mesh[i][j] = 0;
        }
    }

    // 配列の初期化
    int counter = 0;

    int vector_num = 0;

    for (int j = 1; j < data - delta_n; j++)
    {
        // ファイルの読み取り
        char readfile[100];
        sprintf(readfile, "%s/%s/PTV/PTV_vector_dat/%d.dat", dir_path, name, j);

        float buf[10]; // 読み込み用のバッファ
        fp = fopen(readfile, "r");
        while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5], &buf[6], &buf[7], &buf[8])) != EOF)
        {
            // ベクトルの合計値の計算
            position_y = buf[0];
            position_z = buf[1];

            for (int i = 0; i < mesh_y; i++)
            {
                if (i * grid_size <= position_y && position_y < (i + 1) * grid_size)
                {
                    for (int k = 0; k < mesh_z; k++)
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

        fclose(fp);
        // data（データの使用枚数）に注意!!
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

    fp = fopen(writefile, "w");

    for (int i = 0; i < mesh_y; i++)
    {
        for (int j = 0; j < mesh_z; j++)
        {
            if (count_mesh[i][j] != 0)
            {
                // 平均値の算出
                value_y[i][j] = value_y[i][j] / count_mesh[i][j];
                value_z[i][j] = value_z[i][j] / count_mesh[i][j];
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

            fprintf(fp, "%.0f\t%.0f\t%f\t%f\t%f\n", position_y, position_z, value_y[i][j], value_z[i][j], v_value); // 資料画像用にベクトルの長さを誇張
        }
    }

    fclose(fp);

    /** Gnuplot **/

    // ファイル名
    char graphfile[100], graphtitle[100];
    sprintf(graphfile, "%s/%s/PTV/PTV_velocity_svg/velocity_%s.svg", dir_path, name, name);
    sprintf(graphtitle, "Vector");

    // 軸の設定

    // range x
    float x_min = 50;
    float x_max = width_px - 50;

    // range y
    float y_min = 50;
    float y_max = height_px - 50;

    // // range x
    // float x_min = 200;
    // float x_max = 600;

    // // range y
    // float y_min = 200;
    // float y_max = 600;

    // range color
    float cb_min = 0;
    float cb_max = 15;
    // float cb_max = 10;

    // label
    const char *xxlabel = "y [px]";
    const char *yylabel = "z [px]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 1000, 1000 font 'Times New Roman, 16'\n");
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