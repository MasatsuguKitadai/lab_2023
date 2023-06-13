/******************************************************************************
PROGRAM : class.h
AUTHER  : Masatsugu Kitadai
******************************************************************************/

#include <stdio.h>
#include <stdlib.h>
#include <math.h>
#include <time.h>
#include <sys/stat.h>
#include <vector>
#include <iostream>
#include <iomanip>
#include <sstream>
using namespace std;

const float pi = 4 * atan(1.0); // 円周率

/* 保存ディレクトリの設定 */
// const string main_path = "/mnt/d/kitadai/03_numerical_simulation/"; // SSD ワークステーション
// const string main_path = "/mnt/g/workspace_SSD/03_numerical_simulation/"; // SSD ワークステーション
const string main_path = "/mnt/e/workspace_SSD/03_numerical_simulation/"; // SSD
// const string main_path = "/mnt/d/workspace_HDD/03_numerical_simulation/"; // HDD 家のPC

/* 主要パラメータ */
const float deg = 10;          // 壁面の回転速度 [deg/s]
const int num_per_image = 500; // 1枚あたりに映り込む粒子数 [個]

/* 流れの条件 */
const float flow_speed = 8.5;       // 流速 [mm/s]
const float omega = pi / 180 * deg; // 角速度
const float nu = 1.004;             // 動粘性係数

// 各種パラメータ
const float time_max = 1.0;        // シミュレーションする時間 [s]
const int shutter_speed = 800;     // 撮影速度 [1/s]
const float intensity_max = 250.0; // 粒子の最大輝度値 [-]
// const float r_particle = 1.0;      // 粒子半径 [mm]
const float r_particle = 0.2;      // 粒子半径 [mm]
const float camera_angle = 55;     // カメラの設置角度 [°]
const float angle_of_view = 30;    // カメラ(レンズ)の視野角 [°]
const float camera_position = 400; // カメラの設置位置 [mm] (水槽の中心からどの程度カメラが離れているか)
const float focal_length = 25;     // レンズの焦点距離 [mm]
const float magnification = 2.5;   // 拡大率 [-]

/* 生成する画像の設定 */
const int width_px = 800;                     // 画像の横幅 [px]
const int height_px = 400;                    // 画像の縦幅 [px]
const int binary_size = width_px * height_px; // バイナリデータの大きさ [-]
const int header_size = 1078;                 // 8bit bmp のヘッダーファイル

/* 仮想上の校正ブロックの設定 */
const float offset_x = 7.0; // LLS(1) の設置位置

/* 仮想上の撮影画像の設定 */
const float width_mm = 40;           // 実際の撮影範囲の横幅 [mm]
const float height_mm = 20;          // 実際の撮影範囲の縦幅 [mm]
const float width_shot_center = 50;  // y方向の撮影中心 [mm]
const float height_shot_center = 50; // z方向の撮影中心 [mm]

/* LLSの位置設定 */
const float lls_distance = 0.010;                           // LLS間距離 [mm]
const float lls_1_position = 7.0;                           // 前方のllsの位置 [mm]
const float lls_1_thickness = 0.003;                        // 前方のllsの厚み [mm]
const float lls_2_position = lls_1_position + lls_distance; // 後方のllsの位置 [mm]
const float lls_2_thickness = lls_1_thickness * 3.0;        // 後方のllsの厚み [mm]
// const float lls_2_thickness = lls_1_thickness; // 後方のllsの厚み [mm]

/* 粒子の生成範囲の設定 */
const float range_x_max = lls_2_position + 0.05; // x方向の粒子生成範囲　[mm]
const float range_x_min = lls_2_position - 1.0;  // x方向の粒子生成範囲　[mm]
// const float range_x_min = -250 * 0.003; // x方向の粒子生成範囲　[mm]

const float range_x = range_x_max - range_x_min; // x方向の粒子生成範囲　[mm]
const float range_y = width_mm / 2.0 * sqrt(2);  // y方向の粒子生成範囲　[mm]
const float range_z = width_mm / 2.0 * sqrt(2);  // z方向の粒子生成範囲　[mm]

/* 粒子の生成量の設定 */
// const float density_particle = num_per_image / (width_mm * height_mm * lls_1_thickness); // 粒子密度 [個/mm^2]
const float density_particle = num_per_image / (width_mm * height_mm * lls_1_thickness);       // 粒子密度 [個/mm^2]
const int num_particle = density_particle * range_x * width_mm * sqrt(2) * width_mm * sqrt(2); // 生成する粒子数 [-]

/* 校正板 */
const int point_x = 3;
const int point_y = 5;
const int point_z = 11;
const int num_calibration = point_x * point_y * point_z;

/******************************************************************************/

FILE *fp, *fp_r, *fp_w, *gp;
mode_t dir_mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;
const string header_path = "header/800x400_8bit.bmp";        // 使用するヘッダーファイル
const string header_path_color = "header/800x400_24bit.bmp"; // 使用するヘッダーファイル

/******************************************************************************/

/** LLSについて **/
class Parameters
{
public:
    float position;                  // LLSの位置 [mm]
    float thickness;                 // LLSの厚み [mm]
    float sigma;                     // LLSの分散 [-]
    float max;                       // LLSの最大位置 [mm]
    float min;                       // LLSの最小位置 [mm]
    string name;                     // クラスの名称
    string graph_file_tank;          // yz平面グラフの名前
    string graph_file_camera;        // yz平面グラフの名前
    string dat_file_tank;            // datファイルの名前
    string dat_file_camera;          // datファイルの名前
    string dat_file_screen;          // datファイルの名前
    string bmp_file;                 // datファイルの名前
    vector<vector<float>> intensity; // 粒子像の輝度値
    string dat_file_grid_tank;       // datファイルの名前
    string dat_file_grid_camera;     // datファイルの名前
    string dat_file_grid_screen;     // datファイルの名前

    /******************************************************************************
    FUNCTION : Cal_Sigma
    概要：輝度値の与える範囲を計算
    ******************************************************************************/
    void Cal()
    {
        // 3sigmaの範囲に収まるように輝度値を計算
        sigma = thickness / 2 / 3; // [-]

        // LLSの最大位置と最小位置を計算
        max = position + thickness / 2; // [mm]
        min = position - thickness / 2; // [mm]
    }
};

/** 各方向の成分について **/
class Axis
{
public:
    float range_min; // 粒子位置の下限 [mm]
    float range_max; // 粒子位置の上限 [mm]
    float range;     // 粒子の生成範囲 [mm]

    vector<float> initial{};         // 初期位置 [mm]
    vector<float> position_tank{};   // 水槽座標系の粒子位置 [mm]
    vector<float> position_camera{}; // カメラ座標系の粒子位置 [mm]
    vector<float> position_screen{}; // スクリーン座標系の粒子位置 [mm]
    vector<string> data;             // 座標の統合した文字列

    /******************************************************************************
    FUNCTION : Cal_Range
    概要：粒子の生成範囲を計算
    ******************************************************************************/
    void
    Cal_Range()
    {
        range = range_max - range_min;
    }

    /******************************************************************************
    FUNCTION : Resize_Vector
    概要：int particle の数だけ粒子の初期位置を乱数で与える
     IN ：number：データ生成の数
    OUT ：void / initial 配列に値を格納する
    ******************************************************************************/
    void Resize_Vector(int number)
    {
        // ベクターのサイズ変更
        initial.resize(number);
        position_tank.resize(number);
        position_camera.resize(number);
        position_screen.resize(number);
    }

    /******************************************************************************
    FUNCTION : Initial_position
    概要：int particle の数だけ粒子の初期位置を乱数で与える
     IN ：number：データ生成の数
    OUT ：void / initial 配列に値を格納する
    ******************************************************************************/
    void Initial_Position(int number)
    {
        // 乱数による初期位置の設定
        for (size_t i = 0; i < number; i++)
        {
            initial[i] = (float)rand() / RAND_MAX * range + range_min;
            position_tank[i] = initial[i];
        }
    }
};

/* Gnuplot について */
class Gnuplot
{
private:
    FILE *fp, *gp;

public:
    float seconds;      // 対応する時刻 [s]
    float x_max, x_min; // x方向軸の範囲
    float y_max, y_min; // y方向軸の範囲
    float z_max, z_min; // z方向軸の範囲
    string dir_path;

    /******************************************************************************
    FUNCTION : Plot_3d_Calibration
    概要：3次元グラフで粒子位置をsvgファイルに描画
     IN ：void
    OUT ：void / 指定のファイル名に書き出し
    ******************************************************************************/
    void Plot_3d_Calibration(string file_path, string graph_path)
    {
        // グラフのパスの指定
        const char *file_path_c = file_path.c_str();   // str型からchar型への変換
        const char *graph_path_c = graph_path.c_str(); // str型からchar型への変換

        // Gnuplot 呼び出し
        if ((gp = popen("gnuplot", "w")) == NULL)
        {
            printf("gnuplot is not here!\n");
            exit(0); // gnuplotが無い場合、異常ある場合は終了
        }

        fprintf(gp, "set terminal svg size 800, 800 font 'Times New Roman, 24'\n");

        // 出力ファイル
        fprintf(gp, "set output '%s'\n", graph_path_c);

        // グラフのマージン
        fprintf(gp, "set lmargin 5\n");
        fprintf(gp, "set size ratio 1\n");

        // 非表示
        fprintf(gp, "unset key\n");

        // 視点の変更
        fprintf(gp, "set view 60, 30, 1, 1\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
        fprintf(gp, "set zrange [%.3f:%.3f]\n", z_min, z_max);

        // タイトル
        fprintf(gp, "set title 'Calibration Block'\n");
        fprintf(gp, "set title offset -2.0, 0.0\n");

        // 軸ラベル
        fprintf(gp, "set xlabel 'x [mm]'\n");
        fprintf(gp, "set ylabel 'y [mm]'\n");
        fprintf(gp, "set zlabel 'z [mm]'\n");

        // 軸のラベル位置
        fprintf(gp, "set xlabel offset 2.0, 0.5\n");
        fprintf(gp, "set ylabel offset -6.0, 0.0\n");
        fprintf(gp, "set zlabel offset 0.5, 0.0\n");
        fprintf(gp, "set ticslevel 0\n");

        // 軸の数値位置
        fprintf(gp, "set xtics 0.05 offset 0.0, 0.0\n");
        fprintf(gp, "set ytics 20 offset 0.0, 0.0\n");
        fprintf(gp, "set ztics 20 offset 0.0, 0.0\n");

        // グラフの出力
        fprintf(gp, "splot '%s' using 1:2:3 with points lc 'red' ps 0.2 pt 7 notitle\n", file_path_c); // 粒子位置の描画

        fflush(gp); // Clean up Data

        fprintf(gp, "exit\n"); // Quit gnuplot

        pclose(gp);
    }

    /******************************************************************************
    FUNCTION : Plot_3d_Tank
    概要：3次元グラフで粒子位置をsvgファイルに描画
     IN ：void
    OUT ：void / 指定のファイル名に書き出し
    ******************************************************************************/

    void Plot_3d_Tank(string file_path, string graph_path, string grid_path_1, string grid_path_2)
    {
        // グラフのパスの指定
        const char *file_path_c = file_path.c_str();     // str型からchar型への変換
        const char *graph_path_c = graph_path.c_str();   // str型からchar型への変換
        const char *grid_path_1_c = grid_path_1.c_str(); // str型からchar型への変換
        const char *grid_path_2_c = grid_path_2.c_str(); // str型からchar型への変換

        // Gnuplot 呼び出し
        if ((gp = popen("gnuplot", "w")) == NULL)
        {
            printf("gnuplot is not here!\n");
            exit(0); // gnuplotが無い場合、異常ある場合は終了
        }

        fprintf(gp, "set terminal png size 800, 800 font 'Times New Roman, 22'\n");

        // 出力ファイル
        fprintf(gp, "set output '%s'\n", graph_path_c);

        // グラフのマージン
        fprintf(gp, "set lmargin 5\n");
        fprintf(gp, "set size ratio 1\n");

        // 非表示
        fprintf(gp, "unset key\n");

        // 視点の変更
        fprintf(gp, "set view 60, 30, 1, 1\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);
        fprintf(gp, "set zrange [%.3f:%.3f]\n", z_min, z_max);

        // タイトル
        fprintf(gp, "set title 't = %1.3f [s]'\n", seconds);
        fprintf(gp, "set title offset -2.0, -1.0\n");

        // 軸ラベル
        fprintf(gp, "set xlabel 'x [mm]'\n");
        fprintf(gp, "set ylabel 'y [mm]'\n");
        fprintf(gp, "set zlabel 'z [mm]'\n");

        // 軸のラベル位置
        fprintf(gp, "set xlabel offset 0.0, -1.0\n");
        fprintf(gp, "set ylabel offset 1.0, 0.0\n");
        fprintf(gp, "set zlabel offset 0.5, 0.0\n");
        fprintf(gp, "set ticslevel 0\n");

        // 軸の数値位置
        fprintf(gp, "set xtics 0.05 offset 0.0, -0.5\n");
        fprintf(gp, "set ytics 20 offset 1.0, 0.0\n");
        fprintf(gp, "set ztics 20 offset 0.0, 0.0\n");

        // グラフの出力
        fprintf(gp, "splot '%s' using 1:2:3 with points lc 'grey50' ps 0.2 pt 7 notitle, ", file_path_c); // 粒子位置の描画
        fprintf(gp, "'%s' using 1:2:3 with lines lw 3 lc 'blue' notitle, ", grid_path_1_c);               // LLS(1) の描画
        fprintf(gp, "'%s' using 1:2:3 with lines lw 3 lc 'green' notitle\n", grid_path_2_c);              // LLS(2) の描画

        fflush(gp); // Clean up Data

        fprintf(gp, "exit\n"); // Quit gnuplot

        pclose(gp);
    }

    /******************************************************************************
    FUNCTION : Plot_yz_Tank
    概要：3次元グラフで粒子位置をsvgファイルに描画
     IN ：file_path：読み込むファイルの名前，graoh_path：書き出すグラフの名前
    OUT ：void / 指定のファイル名に書き出し
    ******************************************************************************/
    void Plot_yz_Tank(string file_path, string graph_path, string grid_path)
    {
        const char *file_path_c = file_path.c_str();   // str型からchar型への変換
        const char *graph_path_c = graph_path.c_str(); // str型からchar型への変換
        const char *grid_path_c = grid_path.c_str();   // str型からchar型への変換

        // Gnuplot 呼び出し
        if ((gp = popen("gnuplot", "w")) == NULL)
        {
            printf("gnuplot is not here!\n");
            exit(0); // gnuplotが無い場合、異常ある場合は終了
        }

        fprintf(gp, "set terminal svg size 500, 500 font 'Times New Roman, 20'\n");
        fprintf(gp, "set size ratio 1\n");

        // 出力ファイル
        fprintf(gp, "set output '%s'\n", graph_path_c);

        // 非表示
        fprintf(gp, "unset key\n");

        // 視点の変更
        fprintf(gp, "set view 90, 90, 1, 1\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", y_min, y_max);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", z_min, z_max);

        // タイトル
        fprintf(gp, "set title 't = %1.3f [s]'\n", seconds);

        // 軸ラベル
        fprintf(gp, "set xlabel 'y [mm]'\n");
        fprintf(gp, "set ylabel 'z [mm]'\n");

        // 軸のラベル位置
        fprintf(gp, "set xlabel offset 0.0, 0.0\n");
        fprintf(gp, "set ylabel offset 0.0, 0.0\n");
        fprintf(gp, "set ticslevel 0\n");

        // 軸の数値位置
        // fprintf(gp, "set xtics 20 offset 0.0, 0.0\n");
        // fprintf(gp, "set ytics 20 offset 0.0, 0.0\n");

        // グラフの出力
        fprintf(gp, "plot '%s' using 2:3 with points lc 'grey50' ps 0.3 pt 7 notitle, ", file_path_c);
        fprintf(gp, "'%s' using 2:3 with lines lc 'red' lw 3 notitle\n", grid_path_c); // LLS(1) の描画

        fflush(gp); // Clean up Data

        fprintf(gp, "exit\n"); // Quit gnuplot

        pclose(gp);
    }

    /******************************************************************************
    FUNCTION : Plot_3d_Camera
    概要：3次元グラフで粒子位置をsvgファイルに描画
     IN ：void
    OUT ：void / 指定のファイル名に書き出し
    ******************************************************************************/
    void Plot_3d_Camera(string file_path, string graph_path, string grid_path_1, string grid_path_2)
    {
        // グラフのパスの指定
        const char *file_path_c = file_path.c_str();     // str型からchar型への変換
        const char *graph_path_c = graph_path.c_str();   // str型からchar型への変換
        const char *grid_path_1_c = grid_path_1.c_str(); // str型からchar型への変換
        const char *grid_path_2_c = grid_path_2.c_str(); // str型からchar型への変換

        // Gnuplot 呼び出し
        if ((gp = popen("gnuplot", "w")) == NULL)
        {
            printf("gnuplot is not here!\n");
            exit(0); // gnuplotが無い場合、異常ある場合は終了
        }

        fprintf(gp, "set terminal svg size 800, 800 font 'Times New Roman, 24'\n");

        // 出力ファイル
        fprintf(gp, "set output '%s'\n", graph_path_c);

        // グラフのマージン
        fprintf(gp, "set lmargin 5\n");
        fprintf(gp, "set size ratio 1\n");

        // 非表示
        fprintf(gp, "unset key\n");

        // 視点の変更
        fprintf(gp, "set view 60, 345, 1, 1\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", y_min - width_shot_center, y_max - width_shot_center);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", camera_position - 50, camera_position + 50);
        fprintf(gp, "set zrange [%.3f:%.3f]\n", z_min - height_shot_center, z_max - height_shot_center);

        // タイトル
        fprintf(gp, "set title 't = %1.3f [s]'\n", seconds);
        fprintf(gp, "set title offset -2.0, 0.0\n");

        // 軸ラベル
        fprintf(gp, "set xlabel 'x_{c} [mm]'\n");
        fprintf(gp, "set ylabel 'z_{c} [mm]'\n");
        fprintf(gp, "set zlabel 'y_{c} [mm]'\n");

        // 軸のラベル位置
        fprintf(gp, "set xlabel offset 0.0, 0.5\n");
        fprintf(gp, "set ylabel offset 39.0, 3.0\n");
        fprintf(gp, "set zlabel offset 0.5, 0.0\n");
        fprintf(gp, "set ticslevel 0\n");

        // 軸の数値位置
        fprintf(gp, "set xtics 0.05 offset 0.0, 0.0\n");
        fprintf(gp, "set ytics 20 offset 25.0, 1.8\n");
        fprintf(gp, "set ztics 20 offset 0.0, 0.0\n");

        // グラフの出力
        fprintf(gp, "splot '%s' using 1:3:2 with points lc 'grey50' ps 0.2 pt 7 notitle, ", file_path_c); // 粒子位置の描画
        fprintf(gp, "'%s' using 1:3:2 with lines lw 3 lc 'blue' notitle, ", grid_path_1_c);               // LLS(1) の描画
        fprintf(gp, "'%s' using 1:3:2 with lines lw 3 lc 'green' notitle\n", grid_path_2_c);              // LLS(2) の描画

        fflush(gp); // Clean up Data

        fprintf(gp, "exit\n"); // Quit gnuplot

        pclose(gp);
    }

    /******************************************************************************
    FUNCTION : Plot_xy_Camera
    概要：3次元グラフで粒子位置をsvgファイルに描画
     IN ：file_path：読み込むファイルの名前，graoh_path：書き出すグラフの名前
    OUT ：void / 指定のファイル名に書き出し
    ******************************************************************************/
    void Plot_xy_Camera(string file_path, string graph_path, string grid_path)
    {
        const char *file_path_c = file_path.c_str();   // str型からchar型への変換
        const char *graph_path_c = graph_path.c_str(); // str型からchar型への変換
        const char *grid_path_c = grid_path.c_str();   // str型からchar型への変換

        // Gnuplot 呼び出し
        if ((gp = popen("gnuplot", "w")) == NULL)
        {
            printf("gnuplot is not here!\n");
            exit(0); // gnuplotが無い場合、異常ある場合は終了
        }

        fprintf(gp, "set terminal svg size 500, 500 font 'Times New Roman, 20'\n");
        fprintf(gp, "set size ratio 1\n");

        // 出力ファイル
        fprintf(gp, "set output '%s'\n", graph_path_c);

        // 非表示
        fprintf(gp, "unset key\n");

        // 視点の変更
        fprintf(gp, "set view 90, 90, 1, 1\n");

        // 軸の範囲
        fprintf(gp, "set xrange [%.3f:%.3f]\n", y_min - width_shot_center, y_max - width_shot_center);
        fprintf(gp, "set yrange [%.3f:%.3f]\n", z_min - height_shot_center, z_max - height_shot_center);

        // タイトル
        fprintf(gp, "set title 't = %1.3f [s]'\n", seconds);

        // 軸ラベル
        fprintf(gp, "set xlabel 'x [mm]'\n");
        fprintf(gp, "set ylabel 'y [mm]'\n");

        // 軸のラベル位置
        fprintf(gp, "set xlabel offset 0.0, 0.0\n");
        fprintf(gp, "set ylabel offset 0.0, 0.0\n");
        fprintf(gp, "set ticslevel 0\n");

        // 軸の数値位置
        fprintf(gp, "set xtics 20 offset 0.0, 0.0\n");
        fprintf(gp, "set ytics 20 offset 0.0, 0.0\n");

        // グラフの出力
        fprintf(gp, "plot '%s' using 1:2 with points lc 'grey50' ps 0.3 pt 7 notitle, ", file_path_c);
        fprintf(gp, "'%s' using 1:2 with lines lc 'red' lw 3 notitle\n", grid_path_c); // LLS(1) の描画

        fflush(gp); // Clean up Data

        fprintf(gp, "exit\n"); // Quit gnuplot

        pclose(gp);
    }
};
