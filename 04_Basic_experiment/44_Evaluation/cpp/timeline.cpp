/******************************************************************************
PROGRAM : Timeline
AUTHER  : Masatsugu Kitadai
DATE    : 2023/06/28
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

/* プロトタイプ宣言 */
void Timeline(const char *name, const char *data_set);
void plot(const char *name, const char *data_set);

/******************************************************************************/
int main()
{
    /* 保存ディレクトリの設定 */
    string name_str;
    string data_set_str;

    cout << "Data Name:";
    cin >> name_str;
    const char *name = name_str.c_str();

    cout << "Data Set:";
    cin >> data_set_str;
    const char *data_set = data_set_str.c_str();

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/44_Evaluation/%s/timeline", dir_path, name, data_set);
    mkdir(dirname, dirmode);

    // Timeline(name, data_set);
    plot(name, data_set);

    return 0;
}

/******************************************************************************
PROGRAM : Time_line
概要：
******************************************************************************/
void Timeline(const char *name, const char *data_set)
{
    /* 粒子位置の読み込み */
    char readfile[100], writefile[100];
    int x_tmp, y_tmp, buf;
    int progress_counter = 0;
    const char *program_name = "Timeline";

    // 初期ファイルの作成
    sprintf(writefile, "%s/%s/44_Evaluation/%s/timeline/timeline.dat", dir_path, name, data_set);
    fp = fopen(writefile, "w");
    fclose(fp);

    for (int i = 1; i <= number; i++)
    {
        // 変数宣言
        vector<int> x;
        vector<int> y;

        sprintf(readfile, "%s/%s/43_PTV/%s/labeling_position_dat/%04d.dat", dir_path, name, data_set, i);

        fp = fopen(readfile, "r");
        while ((fscanf(fp, "%d\t%d\t%d", &x_tmp, &y_tmp, &buf)) != EOF)
        {
            x.push_back(x_tmp);
            y.push_back(y_tmp);
        }
        fclose(fp);

        // 粒子位置の書き出し
        fp = fopen(writefile, "a");
        for (int j = 0; j < x.size(); j++)
        {
            fprintf(fp, "%f\t%d\t%d\n", i / frame_rate, x[j], y[j]);
        }
        fclose(fp);

        /* メモリの解放 */
        x.clear();
        x.shrink_to_fit();
        y.clear();
        y.shrink_to_fit();

        /* 進捗表示 */
        progress_counter = Progress_meter(program_name, i - 1, number, progress_counter);
    }
}

/******************************************************************************
PROGRAM : plot
概要：
******************************************************************************/
void plot(const char *name, const char *data_set)
{
    // 軸の設定

    // // range x
    const float x_min = 0.0;
    const float x_max = width_px;

    // // range y
    const float y_min = 0.0;
    const float y_max = height_px;

    const float z_min = 0;
    const float z_max = 0.1;

    // label
    const char *xxlabel = "{/:Italic y} [pixel]";
    const char *yylabel = "{/:Italic z} [pixel] ";
    const char *zzlabel = "{/:Italic t} [s] ";

    /** Gnuplot **/
    char graphfile[100], readfile[100];
    sprintf(readfile, "%s/%s/44_Evaluation/%s/timeline/timeline.dat", dir_path, name, data_set);
    sprintf(graphfile, "%s/%s/44_Evaluation/%s/timeline/timeline.svg", dir_path, name, data_set);

    // 軸の設定

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 800, 800 font 'Times New Roman, 16'\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphfile);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
    fprintf(gp, "set zrange [%.3f:%.3f]\n", z_min, z_max);
    // fprintf(gp, "set cbrange['%.3f':'%.3f']\n", z_min, z_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);
    fprintf(gp, "set zlabel '%s'\n", zzlabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.0\n");
    fprintf(gp, "set ylabel offset 0.0, 0.0\n");
    fprintf(gp, "set zlabel offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics 100 offset 0.0, -0.5\n");
    fprintf(gp, "set ytics 50 offset 1.0, 0.0\n");
    fprintf(gp, "set ztics 0.01 offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "splot '%s' using 2:3:1 with points ps 0.2 pt 7 notitle\n", readfile);
    // fprintf(gp, "splot '%s' using 2:3:1 with pm3d notitle\n", readfile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}
