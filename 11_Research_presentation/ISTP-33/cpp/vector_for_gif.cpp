/******************************************************************************
PROGRAM : Velocity
AUTHER  : Masatsugu Kitadai
DATE    : 2023/05/24
******************************************************************************/
// 既存ライブラリ
#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <vector>
#include <algorithm>
#include <iostream>
#include <fstream>
#include <sstream>
using namespace std;

/** プロトタイプ宣言 **/
void Vector_to_Velocity(int n);
void gnuplot_ptv(int n);

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;
const float width_px = 800;  // 画像の横幅 [px]
const float height_px = 400; // 画像の縦幅 [px]
const float width_mm = 40;   // 実際の撮影範囲の横幅 [mm]
const float height_mm = 20;  // 実際の撮影範囲の縦幅 [mm]
const float fps = 800 / 10;

/******************************************************************************/

int main()
{
    mkdir("results/ptv", dirmode);
    for (int n = 1; n < 100; n++)
    {
        Vector_to_Velocity(n);
        gnuplot_ptv(n);
    }

    return 0;
}

void Vector_to_Velocity(int n)
{
    // ファイル名
    string infile_name = "data/data_ptv/" + to_string(n) + ".dat";
    string outfile_name = "results/ptv/" + to_string(n) + ".dat";

    ifstream infile(infile_name);   // ファイルの入力ストリーム
    ofstream outfile(outfile_name); // ファイルの出力ストリーム

    // ファイルが正しく開けたかをチェック
    if (!infile.is_open() || !outfile.is_open())
    {
        cerr << "Error opening files." << endl;
        return;
    }

    // それぞれの列にかける定数のベクトル (必要に応じて調整してください)
    vector<double> m = {width_mm / width_px, height_mm / height_px, width_mm / width_px, height_mm / height_px, width_mm / width_px * fps, height_mm / height_px * fps}; // 例として
    vector<double> p = {50 - width_mm / 2, 50 - height_mm / 2, 0, 0, 0, 0};                                                                                              // 例として

    string line;
    while (getline(infile, line))
    {
        istringstream iss(line);
        double value;
        vector<double> values;

        size_t columnIndex = 0;
        while (iss >> value)
        {
            if (columnIndex < m.size())
                value = value * m[columnIndex] + p[columnIndex];

            values.push_back(value);
            columnIndex++;
        }

        for (size_t i = 0; i < values.size(); ++i)
        {
            outfile << values[i];
            if (i < values.size() - 1)
                outfile << "\t";
        }
        outfile << "\n";
    }

    infile.close();
    outfile.close();
}

void gnuplot_ptv(int n)
{
    /** Gnuplot **/

    // ファイル名
    char readfile[100];
    char graphfile[100];
    sprintf(readfile, "results/ptv/%d.dat", n);
    sprintf(graphfile, "results/ptv/%d.svg", n);

    printf("READ FILE   : %s\n", readfile);
    printf("GRAPH FILE  : %s\n", graphfile);

    // 軸の設定

    // ベクトルの倍率
    const float times = 20.0;

    // range x
    const float x_min = 40 + 2.5;
    const float x_max = 60 - 2.5;

    // range y
    const float y_min = 40 + 2.5;
    const float y_max = 60 - 2.5;

    // label
    const char *xxlabel = "{/:Italic y} [mm]";
    const char *yylabel = "{/:Italic z} [mm]";
    const char *cblabel = "Velocity [mm/s]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 500, 500 font 'Times New Roman, 22'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.0f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s' offset 0.0, 0.5\n", xxlabel);
    fprintf(gp, "set ylabel '%s' offset 1.0, 0.0\n", yylabel);

    // 軸の数値位置
    fprintf(gp, "set xtics 2 offset 0.0, 0.2\n");
    fprintf(gp, "set ytics 2 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using ($1):($2):($3*%f):($4*%f) with vectors head filled lc 'black' lw 1 notitle\n", readfile, times, times);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}