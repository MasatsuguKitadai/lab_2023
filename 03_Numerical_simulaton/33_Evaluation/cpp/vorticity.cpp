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
#include "../hpp/settings.hpp"

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

const int mesh_y = width_px / grid_size;
const int mesh_z = height_px / grid_size;
float value_y[mesh_y][mesh_z], value_z[mesh_y][mesh_z], vorticity[mesh_y][mesh_z];
float position_y[mesh_y][mesh_z], position_z[mesh_y][mesh_z];

// ファイル名
char filename_ptv[100];

/******************************************************************************/

int main()
{
    /* 保存ディレクトリの設定 */
    string name_str;
    cout << "Case Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    for (int i = 0; i < mesh_y; i++)
        for (int j = 0; j < mesh_z; j++)
        {
            value_y[i][j] = 0;
            value_z[i][j] = 0;
            vorticity[i][j] = 0;
            position_y[i][j] = 0;
            position_z[i][j] = 0;
        }

    // ファイルの読み取り
    char readfile[100];
    sprintf(readfile, "%s/%s/Evaluation/data/velocity_correct.dat", dir_path, name);
    float buf[10]; // 読み込み用のバッファ
    fp = fopen(readfile, "r");
    int tmp_y = 0;
    int tmp_z = 0;
    while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4])) != EOF)
    {
        if (tmp_z == height_px / grid_size)
        {
            tmp_y += 1;
            tmp_z = 0;
        }

        position_y[tmp_y][tmp_z] = buf[0];
        position_z[tmp_y][tmp_z] = buf[1];
        value_y[tmp_y][tmp_z] = buf[2];
        value_z[tmp_y][tmp_z] = buf[3];
        tmp_z += 1;
    }
    fclose(fp);

    // 間隔
    const float offset = width_mm / width_px * grid_size / 2;
    const float delta_x = grid_size * width_mm / width_px;

    /* 渦度 */
    for (int i = 1; i < mesh_y - 1; i++)
        for (int j = 1; j < mesh_z - 1; j++)
        {
            vorticity[i][j] = (value_z[i + 1][j] - value_z[i - 1][j]) / delta_x - (value_y[i][j + 1] - value_y[i][j - 1]) / delta_x;
        }

    /* 渦度の書き出し */
    char writefile[100];
    sprintf(writefile, "%s/%s/Evaluation/data/vorticity_correct.dat", dir_path, name);
    fp = fopen(writefile, "w");
    for (int i = 1; i < mesh_y; i++)
    {
        for (int j = 1; j < mesh_z; j++)
        {
            fprintf(fp, "%f\t%f\t%f\t%f\t%f\n", position_y[i][j] - offset, position_z[i][j] - offset, value_y[i][j], value_z[i][j], vorticity[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    /* 渦度プロファイルの書き出し */
    sprintf(writefile, "%s/%s/Evaluation/data/vorticity_profile_correct.dat", dir_path, name);
    fp = fopen(writefile, "w");
    for (int i = 0; i < mesh_y; i++)
        for (int j = 0; j < mesh_z; j++)
        {
            if (position_z[i][j] == height_px / 2)
            {
                fprintf(fp, "%f\t%f\t%f\n", position_y[i][j] - offset, position_z[i][j] - offset, vorticity[i][j]);
            }
        }
    fclose(fp);

    /* 渦度 */

    /** Gnuplot **/
    char graphfile[100], graphtitle[100];
    sprintf(graphfile, "%s/%s/Evaluation/data/vorticity.svg", dir_path, name);
    sprintf(graphtitle, "Vorticity");

    sprintf(writefile, "%s/%s/Evaluation/data/vorticity_correct.dat", dir_path, name);

    // 軸の設定
    // range x
    const float x_min = width_shot_center - 7.5;
    const float x_max = width_shot_center + 7.5;

    // // range y
    const float y_min = height_shot_center - 7.5;
    const float y_max = height_shot_center + 7.5;

    // range color
    float cb_min = -0.5;
    float cb_max = 0.5;

    // label
    const char *xxlabel = "{/:Italic y} [pixel]";
    const char *yylabel = "{/:Italic z} [pixel]";
    const char *cblabel = "Vorticity [-]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 600, 550 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.0f'\n");
    fprintf(gp, "set format cb '%%.1f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set view map\n");

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'  offset 0.0, 1.2\n", xxlabel);
    fprintf(gp, "set ylabel '%s'  offset 0.0, 0.0\n", yylabel);
    fprintf(gp, "set cblabel '%s' offset -1.0, 0.0\n", cblabel);

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.8\n");
    fprintf(gp, "set ytics offset 0.5, 0.0\n");
    fprintf(gp, "set cbtics 0.1 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "splot '%s' using 1:2:5 with pm3d, '%s' using 1:2:(0):($3*2.0):($4*2.0):(0) with vectors head filled lt 2 lc 'black' lw 1 notitle\n", writefile, readfile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);

    return 0;
}