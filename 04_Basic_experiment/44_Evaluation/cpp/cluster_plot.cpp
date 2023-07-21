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
void plot(const char *name, const char *data_set);
void Create_file(const char *name, const char *data_set, const char *model);
void plot_cluster(const char *name, const char *data_set, const char *model);
void plot_cluster_2(const char *name, const char *data_set, const char *model);

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

    string model_str;
    cout << "Model Name:";
    cin >> model_str;
    const char *model = model_str.c_str();

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/44_Evaluation/%s/timeline/%s", dir_path, name, data_set, model);
    mkdir(dirname, dirmode);

    plot_cluster(name, data_set, model);
    Create_file(name, data_set, model);
    plot_cluster_2(name, data_set, model);

    return 0;
}

/******************************************************************************
PROGRAM : create_file
概要：
******************************************************************************/
void Create_file(const char *name, const char *data_set, const char *model)
{

    char readfile[100];
    sprintf(readfile, "%s/%s/44_Evaluation/%s/timeline/%s.dat", dir_path, name, data_set, model);

    vector<float> t;
    vector<float> x;
    vector<float> y;
    vector<float> label;

    float t_tmp, x_tmp, y_tmp, label_tmp;

    fp = fopen(readfile, "r");
    while ((fscanf(fp, "%f\t%f\t%f\t%f", &t_tmp, &x_tmp, &y_tmp, &label_tmp)) != EOF)
    {
        t.push_back(t_tmp);
        x.push_back(x_tmp);
        y.push_back(y_tmp);
        label.push_back(label_tmp);
    }
    fclose(fp);

    int label_max = *max_element(label.begin(), label.end()); // 最大のラベル値の取得
    for (int i = 0; i < 100; i++)
    {
        char writefile[100];
        sprintf(writefile, "%s/%s/44_Evaluation/%s/timeline/%s/%d.dat", dir_path, name, data_set, model, i);
        fp = fopen(writefile, "w");
        for (int j = 0; j < label.size(); j++)
        {
            if (i == label[j])
            {
                fprintf(fp, "%f\t%.0f\t%.0f\n", t[j], x[j], y[j]);
            }
        }
        fclose(fp);
    }
}

/******************************************************************************
PROGRAM : plot_cluster
概要：
******************************************************************************/
void plot_cluster(const char *name, const char *data_set, const char *model)
{

    // 軸の設定

    // // range x
    const float x_min = 0.0;
    const float x_max = width_px;

    // // range y
    const float y_min = 0.0;
    const float y_max = height_px;

    // range color
    const float cb_min = 0.0;
    const float cb_max = 4500;

    const float z_min = 0;
    const float z_max = 80 / frame_rate;

    // label
    const char *xxlabel = "{/:Italic y} [pixel]";
    const char *yylabel = "{/:Italic z} [pixel] ";
    const char *zzlabel = "{/:Italic t} [s] ";

    /** Gnuplot **/
    char graphfile[100], readfile[100];
    sprintf(readfile, "%s/%s/44_Evaluation/%s/timeline/%s.dat", dir_path, name, data_set, model);
    sprintf(graphfile, "%s/%s/44_Evaluation/%s/timeline/%s.svg", dir_path, name, data_set, model);

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
    fprintf(gp, "set palette rgb 22,13,-31\n");

    fprintf(gp, "set format x '%%.0f'\n");
    fprintf(gp, "set format y '%%.0f'\n");
    fprintf(gp, "set format z '%%.2f'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
    fprintf(gp, "set zrange [%.3f:%.3f]\n", z_min, z_max);
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

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
    fprintf(gp, "splot '%s' using 2:3:1:4 with points ps 0.2 pt 7 lc palette notitle\n", readfile);
    // fprintf(gp, "splot '%s' using 2:3:1 with pm3d notitle\n", readfile);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}

/******************************************************************************
PROGRAM : plot_cluster
概要：
******************************************************************************/
void plot_cluster_2(const char *name, const char *data_set, const char *model)
{
    for (int n = 0; n < 100; n++)
    {
        // 軸の設定

        // range x
        const float x_min = 0.0;
        const float x_max = width_px;

        // range y
        const float y_min = 0.0;
        const float y_max = height_px;

        // range z
        const float z_min = 0;
        const float z_max = number / frame_rate;

        // label
        const char *xxlabel = "{/:Italic y} [pixel]";
        const char *yylabel = "{/:Italic z} [pixel] ";
        const char *zzlabel = "{/:Italic t} [s] ";

        /** Gnuplot **/
        char graphfile[100], readfile[100];
        sprintf(readfile, "%s/%s/44_Evaluation/%s/timeline/%s/%d.dat", dir_path, name, data_set, model, n);
        sprintf(graphfile, "%s/%s/44_Evaluation/%s/timeline/%s/%d.png", dir_path, name, data_set, model, n);

        // 軸の設定

        // Gnuplot 呼び出し
        if ((gp = popen("gnuplot", "w")) == NULL)
        {
            printf("gnuplot is not here!\n");
            exit(0); // gnuplotが無い場合、異常ある場合は終了
        }

        fprintf(gp, "set terminal png enhanced size 800, 800 font 'Times New Roman, 12'\n");

        // 出力ファイル
        fprintf(gp, "set output '%s'\n", graphfile);

        // 非表示
        fprintf(gp, "unset key\n");
        fprintf(gp, "set palette rgb 22,13,-31\n");

        fprintf(gp, "set format x '%%.0f'\n");
        fprintf(gp, "set format y '%%.0f'\n");
        fprintf(gp, "set format z '%%.2f'\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
        // fprintf(gp, "set zrange [%.3f:%.3f]\n", z_min, z_max);

        // 軸ラベル
        fprintf(gp, "set xlabel '%s'\n", xxlabel);
        fprintf(gp, "set ylabel '%s'\n", yylabel);
        fprintf(gp, "set zlabel '%s'\n", zzlabel);

        // 軸のラベル位置
        fprintf(gp, "set xlabel offset 0.0, 0.0\n");
        fprintf(gp, "set ylabel offset 0.0, 0.0\n");
        fprintf(gp, "set zlabel offset 0.0, 0.0\n");

        // 軸の数値位置
        fprintf(gp, "set xtics offset 0.0, -0.5\n");
        fprintf(gp, "set ytics offset 1.0, 0.0\n");
        fprintf(gp, "set ztics 0.01 offset 0.0, 0.0\n");

        // グラフの出力
        fprintf(gp, "splot '%s' using 2:3:1 with points ps 0.5 pt 7 notitle\n", readfile);

        fflush(gp); // Clean up Data

        fprintf(gp, "exit\n"); // Quit gnuplot

        pclose(gp);
    }
}