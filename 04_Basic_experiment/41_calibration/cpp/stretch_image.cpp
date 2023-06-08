/******************************************************************************
PROGRAM : stretch_image
AUTHER  : Masatsugu Kitadai
DATE    : 2022/11/17
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>
using namespace std;

FILE *fp, *gp;
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;

/** プロトタイプ宣言 **/
void Stretch_image(const char *name);
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[]);
void Load_Bmp_24bit(const char file_name[], unsigned char header[], unsigned char binary[]);

/*****************************************************************************/

#include "../hpp/settings.hpp"

/******************************************************************************/
// gnuplot
char graphname[2][100];
char graphtitle[100];
char xxlabel[100] = "x [px]";
char yylabel[100] = "y [px]";
char zzlabel[2][100];
double x_max = width;
double x_min = 0;
double y_max = height;
double y_min = 0;
double cb_max = 150;
double cb_min = -50;

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

    Stretch_image(name);

    return 0;
}

/******************************************************************************
PROGRAM : Stretch_image
DATE    : 2023/05/15
******************************************************************************/
void Stretch_image(const char *name)
{
    char filename[10][200];
    float buf[10] = {0};

    /* ディレクトリの作成 */
    char dirname[100];
    sprintf(dirname, "%s/%s/41_calibration/stretch_image", dir_path, name);
    mkdir(dirname, dirmode);

    /** 変数宣言 **/
    double a[2], b[2], c[2], d[2], e[2], f[2], g[2], h[2], o[2], p[2];
    double x, y;
    int x_buf, y_buf;
    double delta_x, delta_y;

    // bmp 1次元配列
    unsigned char binary[px_8_origin] = {0};
    unsigned char binary_stretch[px_8] = {0};
    unsigned char binary_2d[width_origin][height_origin] = {0};
    unsigned char binary_2d_new[width][height] = {0};

    // 校正用画像の取得
    sprintf(filename[0], "%s/%s/41_calibration/binarization/calibration_8bit.bmp", dir_path, name);
    Load_Bmp_8bit(filename[0], header_8bit, binary);

    // 2次元配列への格納
    int i = 0;
    int j = 0;

    for (int k = 0; k < px_8_origin; k++)
    {
        if (i == width_origin)
        {
            i = 0;
            j = j + 1;
        }

        binary_2d[i][j] = binary[k];
        i = i + 1;

        // printf("[%d][%d]\t2d = %d\tdata = %d\n", i, j, binary_2d[i][j], binary[k]);
    }

    double distance; // 参照点の奥行間距離

    for (int n = 0; n < 3; n++)
    {

        // キャリブレーション用の式の格納
        sprintf(filename[1], "%s/%s/41_calibration/curve_fit/curve_fit_%1.1fmm.dat", dir_path, name, 2.5 * n);
        printf("%s\n", filename[1]);

        i = 0;

        fp = fopen(filename[1], "r");

        while (fscanf(fp, "%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f\t%f", &buf[0], &buf[1], &buf[2], &buf[3], &buf[4], &buf[5], &buf[6], &buf[7], &buf[8], &buf[9]) != EOF)
        {
            a[i] = buf[0];
            b[i] = buf[1];
            c[i] = buf[2];
            d[i] = buf[3];
            e[i] = buf[4];
            f[i] = buf[5];
            g[i] = buf[6];
            h[i] = buf[7];
            o[i] = buf[8];
            p[i] = buf[9];
            i += 1;
        }

        fclose(fp);

        // 変換後の座標書き出し用ファイル
        sprintf(filename[2], "%s/%s/41_calibration/stretch_image/coordinate_%1.1fmm.dat", dir_path, name, 2.5 * n);

        fp = fopen(filename[2], "w");

        // 参照位置の計算
        for (i = 0; i < width; i++)
        {
            for (j = 0; j < height; j++)
            {
                // 元の画像における座標
                x = a[0] * i * i * i + b[0] * j * j * j + c[0] * i * i * j + d[0] * i * j * j + e[0] * i * i + f[0] * j * j + g[0] * i * j + h[0] * i + o[0] * j + p[0]; // x方向
                y = a[1] * i * i * i + b[1] * j * j * j + c[1] * i * i * j + d[1] * i * j * j + e[1] * i * i + f[1] * j * j + g[1] * i * j + h[1] * i + o[1] * j + p[1]; // y方向
                delta_x = x - i;
                delta_y = y - j;

                fprintf(fp, "%d\t%d\t%f\t%f\t%f\t%f\n", i, j, x, y, delta_x, delta_y);

                x_buf = x;
                y_buf = y;

                // 平均値の計算
                binary_2d_new[i][j] = (binary_2d[x_buf][y_buf] + binary_2d[x_buf + 1][y_buf] + binary_2d[x_buf][y_buf + 1] + binary_2d[x_buf + 1][y_buf + 1]) / 4;
            }

            fprintf(fp, "\n");
        }

        fclose(fp);

        // ヘッダの読み込み
        sprintf(filename[3], "header/%dx%d_8bit.bmp", width, height);
        Load_Bmp_8bit(filename[3], header_8bit, binary_stretch);

        // 一次元配列への格納
        int k = 0;
        for (j = 0; j < height; j++)
        {
            for (i = 0; i < width; i++)
            {
                binary_stretch[k] = binary_2d_new[i][j];
                k = k + 1;
            }
        }

        // 書き出すグラフの名前
        sprintf(filename[4], "%s/%s/41_calibration/stretch_image/coordinate_%1.1fmm.bmp", dir_path, name, 2.5 * n);

        fp = fopen(filename[4], "wb");

        // ヘッダー情報の書き込み
        fwrite(header_8bit, sizeof(unsigned char), 1078, fp);

        // 画像情報の書き込み
        fwrite(binary_stretch, sizeof(unsigned char), px_8, fp);

        fclose(fp);

        /** Gnuplot **/
        sprintf(zzlabel[0], "delta x [px]");
        sprintf(zzlabel[1], "delta y [px]");

        // Gnuplot 呼び出し
        if ((gp = popen("gnuplot", "w")) == NULL)
        {
            printf("gnuplot is not here!\n");
            exit(0); // gnuplotが無い場合、異常ある場合は終了
        }

        sprintf(graphtitle, "Correlation plane : %1.1f mm", n * 2.5);
        sprintf(graphname[0], "%s/%s/41_calibration/stretch_image/%1.1fmm_x.png", dir_path, name, n * 2.5);
        sprintf(graphname[1], "%s/%s/41_calibration/stretch_image/%1.1fmm_y.png", dir_path, name, n * 2.5);

        sprintf(filename[5], "%s/%s/41_calibration/get_peaks/peak_positions_%1.1fmm.dat", dir_path, name, 2.5 * n);

        fprintf(gp, "set terminal pngcairo enhanced size 800, 800 font 'Times New Roman, 16'\n");
        fprintf(gp, "set size ratio -1\n");

        // カラーマップの設定
        fprintf(gp, "set pm3d\n");

        // 非表示
        fprintf(gp, "unset key\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
        fprintf(gp, "set zrange [%.3f:%.3f]\n", cb_min, cb_max);

        // グラフタイトル
        fprintf(gp, "set title '%s' offset 0.0, -1.0\n", graphtitle);

        // fprintf(gp, "set palette rgb 22,13,-31\n");

        fprintf(gp, "set palette defined(-3 'blue', 0 'white', 1 'red')\n");

        // 軸ラベル
        fprintf(gp, "set xlabel '%s'\n", xxlabel);
        fprintf(gp, "set ylabel '%s'\n", yylabel);

        // 軸のラベル位置
        fprintf(gp, "set xlabel offset -1.0, -1.0\n");
        fprintf(gp, "set ylabel offset 1.0, -1.0\n");

        // 軸の数値位置
        fprintf(gp, "set xtics offset -1.0, -1.0\n");
        fprintf(gp, "set ytics offset 1.0, -1.0\n");

        // 出力ファイルの設定とグラフの出力

        // 色付け
        fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);
        fprintf(gp, "set colorbox user origin 0.9, 0.4 size 0.03, 0.45\n");

        fprintf(gp, "set ztics 50\n");
        fprintf(gp, "set output '%s'\n", graphname[0]);
        fprintf(gp, "splot '%s' using 1:2:5 with pm3d lc palette title '%s', '%s' using 3:4:5 with points lc 'green' ps 1.5 pt 7 title 'Points'\n", filename[2], zzlabel[0], filename[5]);

        fprintf(gp, "set ztics 50\n");
        fprintf(gp, "set output '%s'\n", graphname[1]);
        fprintf(gp, "splot '%s' using 1:2:6 with pm3d lc palette title '%s', '%s' using 3:4:6 with points lc 'green' ps 1.5 pt 7 title 'Points'\n", filename[2], zzlabel[1], filename[5]);

        fflush(gp); // Clean up Data

        fprintf(gp, "exit\n"); // Quit gnuplot

        pclose(gp);
    }

    // 重ね合わせたグラフの表示

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    sprintf(graphtitle, "Correlation plane : ALL");
    sprintf(graphname[0], "%s/%s/41_calibration/stretch_image/all_x.png", dir_path, name);
    sprintf(graphname[1], "%s/%s/41_calibration/stretch_image/all_y.png", dir_path, name);

    sprintf(filename[6], "%s/%s/41_calibration/stretch_image/coordinate_%1.1fmm.dat", dir_path, name, 0.0);
    sprintf(filename[7], "%s/%s/41_calibration/stretch_image/coordinate_%1.1fmm.dat", dir_path, name, 2.5);
    sprintf(filename[8], "%s/%s/41_calibration/stretch_image/coordinate_%1.1fmm.dat", dir_path, name, 5.0);

    fprintf(gp, "set terminal pngcairo enhanced size 800, 800 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio -1\n");

    // カラーマップの設定
    fprintf(gp, "set pm3d\n");

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
    fprintf(gp, "set zrange [%.3f:%.3f]\n", cb_min, cb_max);

    // グラフタイトル
    fprintf(gp, "set title '%s' offset 0.0, -1.0\n", graphtitle);

    // fprintf(gp, "set palette rgb 22,13,-31\n");

    fprintf(gp, "set palette defined(-3 'blue', 0 'white', 1 'red')\n");

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset -1.0, -1.0\n");
    fprintf(gp, "set ylabel offset 1.0, -1.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset -1.0, -1.0\n");
    fprintf(gp, "set ytics offset 1.0, -1.0\n");

    // 出力ファイルの設定とグラフの出力

    // 色付け
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);
    fprintf(gp, "set colorbox user origin 0.9, 0.4 size 0.03, 0.45\n");

    fprintf(gp, "set ztics 50\n");
    fprintf(gp, "set output '%s'\n", graphname[0]);
    fprintf(gp, "splot '%s' using 1:2:5 with pm3d lc palette notitle, '%s' using 1:2:5 with pm3d lc palette notitle, '%s' using 1:2:5 with pm3d lc palette notitle\n", filename[6], filename[7], filename[8]);

    fprintf(gp, "set ztics 50\n");
    fprintf(gp, "set output '%s'\n", graphname[1]);
    fprintf(gp, "splot '%s' using 1:2:6 with pm3d lc palette notitle, '%s' using 1:2:6 with pm3d lc palette notitle, '%s' using 1:2:6 with pm3d lc palette notitle \n", filename[6], filename[7], filename[8]);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}

/******************************************************************************
FUNCTION : Load_bmp_8bit
概要：8bitのbmp画像を配列に格納する
 IN ：file_name：読み込むファイル名，header：ヘッダーの格納用配列，binary：輝度値の格納用配列
OUT ：void / header, binary配列に値を格納する
******************************************************************************/
void Load_Bmp_8bit(const char file_name[], unsigned char header[], unsigned char binary[])
{
    FILE *fp;              // ファイルポインタの宣言
    int i;                 // ループ用変数
    const int size = 1078; // 画像サイズによって変動する可能性有り

    // bmpファイルをバイナリモードで読み取り
    fp = fopen(file_name, "rb");

    // 画像ファイルが見つからない場合のエラー処理
    if (fp == NULL)
    {
        printf("Not found : %s \n", file_name);
        exit(-1);
    }

    // ヘッダ情報の読み込み
    for (i = 0; i < size; i++)
    {
        header[i] = fgetc(fp);
    }

    // 画像がビットマップで無い場合のエラー処理
    if (!(header[0] == 'B' && header[1] == 'M'))
    {
        printf("Not BMP file : %s \n", file_name);
        exit(-1);
    }

    int tmp; // 輝度値の一時的な格納用変数
    i = 0;

    // 輝度値読み込み
    while ((tmp = fgetc(fp)) != EOF)
    {
        binary[i] = tmp;
        i = i + 1;
    }

    // printf("data length = %d\n", i);

    fclose(fp); // ファイルを閉じる
}

/******************************************************************************
FUNCTION : Load_bmp_24bit
概要：8bitのbmp画像を配列に格納する
 IN ：file_name：読み込むファイル名，header：ヘッダーの格納用配列，binary：輝度値の格納用配列
OUT ：void / header, binary配列に値を格納する
******************************************************************************/
void Load_Bmp_24bit(const char file_name[], unsigned char header[], unsigned char binary[])
{
    FILE *fp;            // ファイルポインタの宣言
    int i;               // ループ用変数
    const int size = 54; // 画像サイズによって変動する可能性有り

    // bmpファイルをバイナリモードで読み取り
    fp = fopen(file_name, "rb");

    // 画像ファイルが見つからない場合のエラー処理
    if (fp == NULL)
    {
        printf("Not found : %s \n", file_name);
        exit(-1);
    }

    // ヘッダ情報の読み込み
    for (i = 0; i < size; i++)
    {
        header[i] = fgetc(fp);
    }

    // 画像がビットマップで無い場合のエラー処理
    if (!(header[0] == 'B' && header[1] == 'M'))
    {
        printf("Not BMP file : %s \n", file_name);
        exit(-1);
    }

    int tmp; // 輝度値の一時的な格納用変数
    i = 0;

    // 輝度値読み込み
    while ((tmp = fgetc(fp)) != EOF)
    {
        binary[i] = tmp;
        i = i + 1;
    }

    // printf("data length = %d\n", i);

    fclose(fp); // ファイルを閉じる
}