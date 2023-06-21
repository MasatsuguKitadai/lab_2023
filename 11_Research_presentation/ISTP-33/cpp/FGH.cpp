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
using namespace std;

/** プロトタイプ宣言 **/
void Get_FGH();
int Find_Closest_Value(float x, const vector<float> &x_data);
float Lagrange_Interpolation(float x, const vector<float> &x_data, const vector<float> &y_data);
void gnuplot_1();
void gnuplot_2();

/*****************************************************************************/

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/******************************************************************************/

int main()
{
    Get_FGH();
    gnuplot_1();

    return 0;
}

/******************************************************************************
FUNCTION : Get_FGH
概要：参考文献による流れの再現
 IN ：seconds：計算する時刻 [s]
OUT ：void /  x[]，y[]，z[] 配列に値を格納する
******************************************************************************/
void Get_FGH()
{
    /* 関数の読み込み */
    const char read_file[] = "csv/FGH.csv"; // ファイル名
    vector<float> Zeta;                     // 参考文献からの値 [-]
    vector<float> F;                        // 参考文献からの値 [-]
    vector<float> G;                        // 参考文献からの値 [-]
    vector<float> H;                        // 参考文献からの値 [-]
    double zeta, f, g, h;

    fp = fopen(read_file, "r");

    // 画像ファイルが見つからない場合のエラー処理
    if (fp == NULL)
    {
        printf("Not found : %s \n", read_file);
        exit(-1);
    }

    // ファイルの読み込み
    while (fscanf(fp, "%lf,%lf,%lf,%lf", &zeta, &f, &g, &h) != EOF)
    {
        Zeta.push_back(zeta);
        F.push_back(f);
        G.push_back(g);
        H.push_back(h);
    }
    fclose(fp);

    // 書き出しファイルの設定
    const char file[] = "data/FGH.dat";
    fp = fopen(file, "w");

    for (int i = 0; i <= 120; i++)
    {
        float zeta_tmp = (float)i / 10.0;

        // 関数表の採用
        int index = Find_Closest_Value(zeta_tmp, Zeta);

        // 補完用データの作成
        vector<float> Zeta_lagrange;
        vector<float> F_lagrange;
        vector<float> G_lagrange;
        vector<float> H_lagrange;

        for (int j = -1; j <= 1; j++)
        {
            Zeta_lagrange.push_back(Zeta[index + j]);
            F_lagrange.push_back(F[index + j]);
            G_lagrange.push_back(G[index + j]);
            H_lagrange.push_back(H[index + j]);
        }

        // 関数値の格納
        float F_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, F_lagrange);
        float G_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, G_lagrange);
        float H_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, H_lagrange);

        fprintf(fp, "%.1f\t%f\t%f\t%f\n", zeta_tmp, F_tmp, G_tmp, H_tmp);
    }

    fclose(fp);
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
FUNCTION : gnuplot_1
概要：参考文献による流れの再現
******************************************************************************/
void gnuplot_1()
{
    /** Gnuplot **/

    // ファイル名
    const char readfile[] = "data/FGH.dat";
    const char graphfile[] = "results/FGH.svg";

    printf("READ FILE   : %s\n", readfile);
    printf("GRAPH FILE  : %s\n", graphfile);

    // 軸の設定

    // range x
    const float x_min = 0.0;
    const float x_max = 12.0;

    // range y
    const float y_min = -1.0;
    const float y_max = 2.0;

    // label
    const char *xxlabel = "{/Symbol z} [-]";
    const char *yylabel = "{/:Italic F}, {/:Italic G}, {/:Italic H} [-]";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg size 600, 400 font 'Times New Roman, 18'\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // グラフのマージン
    fprintf(gp, "set lmargin 5\n");
    fprintf(gp, "set size ratio 0.5\n");

    // 凡例の設定
    fprintf(gp, "set key right outside font 'Times New Roman, 16'\n");

    // 軸の表記桁数の指定
    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.1f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.8\n");
    fprintf(gp, "set ylabel offset 2.5, 0.0\n");
    fprintf(gp, "set ticslevel 0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics 1.0 offset 0.0, 0.3 font 'Times New Roman, 16'\n");
    fprintf(gp, "set ytics 0.5 offset 0.3, 0.0 font 'Times New Roman, 16'\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:2 with lines lw 1 lc 'blue' title 'F',", readfile); // LLS(1) の描画
    fprintf(gp, "'%s' using 1:3 with lines lw 1 lc 'green' title 'G',", readfile);     // LLS(2) の描画
    fprintf(gp, "'%s' using 1:4 with lines lw 1 lc 'red' title 'H'\n", readfile);      // LLS(2) の描画

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}