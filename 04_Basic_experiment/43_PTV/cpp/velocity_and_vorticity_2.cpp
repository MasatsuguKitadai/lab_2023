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

/** プロトタイプ宣言 **/
void Vorticity(const char *name, const char *data_set, int n);
void Plot_Vorticity(const char *name, const char *data_set, int n);

const float sec = 1.0;

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/******************************************************************************/

int main()
{
    /* 保存ディレクトリの設定 */
    string name_str;
    cout << "Data Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    string data_set_str;
    cout << "Data Set:";
    cin >> data_set_str;
    const char *data_set = data_set_str.c_str();

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/43_PTV/%s/PTV_vorticity_dat", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    sprintf(dirname, "%s/%s/43_PTV/%s/PTV_vorticity_svg", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    // Vorticity(name, data_set, 0);
    // Plot_Vorticity(name, data_set, 0);

    // Vorticity(name, data_set, 1);
    // Plot_Vorticity(name, data_set, 1);

    // Vorticity(name, data_set, 2);
    // Plot_Vorticity(name, data_set, 2);

    // Vorticity(name, data_set, 3);
    Plot_Vorticity(name, data_set, 3);

    // for (int n = 0; n < number / (frame_rate * sec) - 1; n++)
    // {
    //     printf("n = %d\n", n);
    //     Vorticity(name, data_set, n);
    //     Plot_Vorticity(name, data_set, n);
    // }

    return 0;
}

/******************************************************************************
FUNCTION：Velocity
概要    ：
******************************************************************************/
void Vorticity(const char *name, const char *data_set, int n)
{
    // 配列の初期化
    const int mesh_y = width_px / grid_size;
    const int mesh_z = height_px / grid_size;
    float value_y[mesh_y][mesh_z] = {0};
    float value_z[mesh_y][mesh_z] = {0};
    float vorticity[mesh_y][mesh_z] = {0};
    float r[mesh_y][mesh_z] = {0};
    int count_mesh[mesh_y][mesh_z] = {0};
    float position_y, position_z;

    for (int j = n * frame_rate * sec + 1; j <= (n + 1) * frame_rate * sec; j++)
    {
        // ファイルの読み取り
        char readfile[100];
        sprintf(readfile, "%s/%s/43_PTV/%s_n=10/PTV_vector_dat/%04d.dat", dir_path, name, data_set, j);

        float buf[10]; // 読み込み用のバッファ
        fp = fopen(readfile, "r");
        while ((fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5])) != EOF)
        {
            // ベクトルの合計値の計算
            position_y = buf[0];
            position_z = buf[1];

            if (border_max >= buf[5] && buf[5] >= border_min)
            {
                for (int i = 0; i <= mesh_y; i++)
                {
                    if (i * grid_size - grid_size / 2 <= position_y && position_y < i * grid_size + grid_size / 2)
                    {
                        for (int k = 0; k <= mesh_z; k++)
                        {
                            if (k * grid_size - grid_size / 2 <= position_z && position_z < k * grid_size + grid_size / 2)
                            {
                                value_y[i][k] += buf[2];
                                value_z[i][k] += buf[3];
                                r[i][k] += buf[5];
                                count_mesh[i][k] += 1;
                                break;
                            }
                        }
                        break;
                    }
                }
            }
        }

        fclose(fp);
    }

    /* アベレージ */
    const float t = delta_n / shutter_speed;
    for (int i = 0; i <= mesh_y; i++)
        for (int j = 0; j <= mesh_z; j++)
        {
            if (count_mesh[i][j] != 0)
            {
                // 平均値の算出
                value_y[i][j] = value_y[i][j] / count_mesh[i][j] * width_mm / width_px;              // y方向の移動量 [mm]
                value_z[i][j] = value_z[i][j] / count_mesh[i][j] * height_mm / height_px;            // z方向の移動量 [mm]
                float v_value = sqrt(value_y[i][j] * value_y[i][j] + value_z[i][j] * value_z[i][j]); // 全体の速度 [mm/s]

                // 誤ベクトル処理
                if (100 < v_value)
                {
                    value_y[i][j] = 0;
                    value_z[i][j] = 0;
                }
            }
            else
            {
                value_y[i][j] = 0;
                value_z[i][j] = 0;
            }
        }

    /* 渦度 */
    for (int i = 1; i < mesh_y - 1; i++)
    {
        for (int j = 1; j < mesh_z - 1; j++)
        {
            vorticity[i][j] = (value_z[i + 1][j] - value_z[i - 1][j]) / grid_size - (value_y[i][j + 1] - value_y[i][j - 1]) / grid_size;
        }
    }

    /* 渦度の書き出し */
    char writefile[100];
    sprintf(writefile, "%s/%s/43_PTV/%s/PTV_vorticity_dat/vorticity_%d.dat", dir_path, name, data_set, n);
    fp = fopen(writefile, "w");
    for (int i = 1; i < mesh_y - 1; i++)
    {
        for (int j = 1; j < mesh_z - 1; j++)
        {
            // ベクトルの始点
            position_y = i * grid_size * (float)width_mm / width_px;
            position_z = j * grid_size * (float)height_mm / height_px;
            fprintf(fp, "%f\t%f\t%f\t%f\t%f\n", position_y, position_z, value_y[i][j], value_z[i][j], vorticity[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    printf("check wri\n");
}

/******************************************************************************
FUNCTION：Plot
概要    ：
******************************************************************************/
void Plot_Vorticity(const char *name, const char *data_set, int n)
{
    /** Gnuplot **/

    // ファイル名
    char graphfile[100], graphtitle[100];
    sprintf(graphfile, "%s/%s/43_PTV/%s/PTV_vorticity_svg/vorticity_%d.svg", dir_path, name, data_set, n);
    sprintf(graphtitle, "Vorticity");

    char writefile[100];
    sprintf(writefile, "%s/%s/43_PTV/%s/PTV_vorticity_dat/vorticity_%d.dat", dir_path, name, data_set, n);

    // 軸の設定

    const float x_min = 0;
    const float x_max = 100;

    // // range y
    const float y_min = 0;
    const float y_max = 40;

    // range color
    float cb_min = -0.05;
    float cb_max = 0.05;

    // label
    const char *xxlabel = "{/:Italic y} [mm]";
    const char *yylabel = "{/:Italic z} [mm]";
    const char *cblabel = "Vorticity [-]";

    /** Gnuplot **/

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 800, 400 font 'Times New Roman, 16'\n");
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
    fprintf(gp, "splot '%s' using 1:2:5 with pm3d notitle\n", writefile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}