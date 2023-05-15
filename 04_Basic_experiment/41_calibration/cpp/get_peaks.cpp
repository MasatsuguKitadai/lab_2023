/******************************************************************************
PROGRAM : get_peaks
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/17
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <iostream>
using namespace std;

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/** プロトタイプ宣言 **/
void Get_peaks(const char *name);

/*****************************************************************************/

#include "../hpp/settings.hpp"

/******************************************************************************
PROGRAM : main
DATE    : 2023/05/15
******************************************************************************/
int main()
{
    /* データ名の読み取り */
    string name_str;
    cout << "Data Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    Get_peaks(name);

    return 0;
}

/******************************************************************************
PROGRAM : Get_peaks
DATE    : 2023/05/15
******************************************************************************/
void Get_peaks(const char *name)
{
    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/41_calibration/get_peaks", dir_path, name);
    mkdir(dirname, dirmode);

    // バッファ
    float buf[10];
    float cal[10];
    char filename[10][200];

    // 配列
    float ary_correlation[px_8_origin] = {0};
    float ary_label[px_8_origin] = {0};

    // 相関係数ファイルの読み込み
    sprintf(filename[0], "%s/%s/41_calibration/correlation/correlation.dat", dir_path, name);

    int tmp = 0;

    fp = fopen(filename[0], "r");

    while ((fscanf(fp, "%f\t%f\t%f", &buf[0], &buf[1], &buf[2])) != EOF)
    {
        ary_correlation[tmp] = buf[2];

        if (buf[0] == width_origin - calibration_size / 2 - 1)
        {
            char string_buf[100];
            fgets(string_buf, 100, fp);
        }

        tmp += 1;
    }

    // printf("correlation = %d\n", i);
    fclose(fp);

    // ラベリングファイルの読み込み
    sprintf(filename[1], "%s/%s/41_calibration/labeling/labeling.dat", dir_path, name);

    tmp = 0;

    float max = 0;
    fp = fopen(filename[1], "r");

    while ((fscanf(fp, "%f\t%f\t%f", &buf[0], &buf[1], &buf[2])) != EOF)
    {
        ary_label[tmp] = buf[2];
        tmp += 1;

        if (buf[0] == width_origin - calibration_size / 2 - 1)
        {
            char string_buf[100];
            fgets(string_buf, 100, fp);
        }

        // ラベルの最大値の取得
        if (buf[2] > max)
        {
            max = buf[2];
        }
    }

    fclose(fp);

    // ラベルの最大値の確保
    int label = max;
    vector<pair<int, int>> v;

    printf("label = %d\n", label);

    // 各エリアのピーク値の取得

    for (int i = 0; i < label; i++)
    {
        float peak = 0;
        int x = 0;
        int y = 0;

        for (int j = 0; j < px_8_origin; j++)
        {
            // ピーク値を持つ座標の取得
            if (ary_label[j] == i + 1)
            {
                // 配列の位置調整
                cal[0] = j % width_origin - calibration_size / 2; // 余り
                cal[1] = (width_origin - calibration_size) * (int(j / width_origin) - calibration_size / 2);
                int position = cal[0] + cal[1];

                if (ary_correlation[position] > peak)
                {
                    peak = ary_correlation[position]; //
                    x = j % width_origin;             // 余りがx座標になる
                    y = j / width_origin;             // 少数以下は切り捨て
                }
            }
        }

        v.push_back({x, y});

        // printf("[%2d] :\t%3d\t%3d\t%f\n", i + 1, v[i].first, v[i].second, peak);
    }

    // 並び替え
    sort(v.begin(), v.end());

    // 値の確認
    for (int i = 0; i < v.size(); i++)
    {
        printf("[%d]\t%d\t%d\n", i + 1, v[i].first, v[i].second);
    }

    int initial;
    int stretch_x, stretch_y;
    int delta_x, delta_y;

    vector<pair<int, int>> v_buf;

    // ファイルの書き出し
    for (int i = 0; i < 3; i++)
    {
        sprintf(filename[2], "%s/%s/41_calibration/get_peaks/peak_positions_%1.1fmm.dat", dir_path, name, 2.5 * i);

        fp = fopen(filename[2], "w");

        // 初期値
        initial = points * i;

        for (int j = 0; j < column; j++)
        {
            for (int k = 0; k < points; k++)
            {
                tmp = j * points * 3 + k + initial;
                v_buf.push_back({v[tmp].second, v[tmp].first});
            }

            stretch_x = (width / (column - 1)) * j;

            for (int k = 0; k < v_buf.size(); k++)
            {
                sort(v_buf.begin(), v_buf.end());

                // 正しい y 座標
                stretch_y = (height / (points - 1)) * k;

                // 座標差
                delta_x = v_buf[k].second - stretch_x;
                delta_y = v_buf[k].first - stretch_y;

                fprintf(fp, "%d\t%d\t%d\t%d\t%d\t%d\n", v_buf[k].second, v_buf[k].first, stretch_x, stretch_y, delta_x, delta_y);
            }

            // ベクターの初期化
            v_buf.erase(v_buf.begin(), v_buf.end());
            // printf("v_buf = %ld\n", v_buf.size());
        }

        fclose(fp);
    }

    // すべての座標の書き出し
    sprintf(filename[2], "%s/%s/41_calibration/get_peaks/peak_positions.dat", dir_path, name);

    fp = fopen(filename[2], "w");

    for (int i = 0; i < label; i++)
    {
        fprintf(fp, "%d\t%d\n", v[i].first, v[i].second);
    }

    fclose(fp);

    /** Gnuplot **/

    // グラフの背景
    sprintf(filename[3], "%s/%s/41_calibration/binarization/calibration_8bit.png", dir_path, name);

    // 軸の設定

    // range x
    float x_min = 0;
    float x_max = width_origin;

    // range y
    float y_min = 0;
    float y_max = height_origin;

    // label
    const char *xxlabel = "x [px]";
    const char *yylabel = "y [px]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    for (int i = 0; i < 3; i++)
    {
        char graphname[100];
        char graphtitle[100];

        sprintf(filename[2], "%s/%s/41_calibration/get_peaks/peak_positions_%1.1fmm.dat", dir_path, name, 2.5 * i);
        sprintf(graphname, "%s/%s/41_calibration/get_peaks/peak_positions_%1.1fmm.png", dir_path, name, i * 2.5);
        sprintf(graphtitle, "Peak positions : %1.1f [mm]", i * 2.5);

        fprintf(gp, "set terminal pngcairo enhanced size 1000, 800 font 'Times New Roman, 20'\n");
        // fprintf(gp, "set terminal svg enhanced size 1000, 1000 font 'Times New Roman, 24' \n");
        fprintf(gp, "set size ratio -1\n");

        // 出力ファイル
        fprintf(gp, "set output '%s'\n", graphname);

        // 非表示
        fprintf(gp, "unset key\n");
        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

        // グラフタイトル
        fprintf(gp, "set title '%s'\n", graphtitle);

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
        fprintf(gp, "plot '%s' binary filetype=png with rgbimage, '%s' using 1:2 with points  lc 'red' pt 7 ps 1 notitle\n", filename[3], filename[2]);
        // fprintf(gp, "splot '%s' with pm3d lc palette notitle\n", filename);

        fflush(gp); // Clean up Data
    }

    fprintf(gp, "exit\n"); // Quit gnuplot
    pclose(gp);
}