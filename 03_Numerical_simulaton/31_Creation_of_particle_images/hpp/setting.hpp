/******************************************************************************
PROGRAM : setting.h
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

const float pi = 4 * atan(1.0); // 円周率

/* 保存ディレクトリの設定 */
// const std::string dir_path_sub = "/mnt/e/workspace_SSD/03_numerical_simulation/"; // SSD
const std::string dir_path_sub = "/mnt/d/workspace_HDD/03_numerical_simulation/"; // HDD 家のPC

/* 流れの条件 */
const float flow_speed = 8.5; // 流速 [mm/s]
float omega = pi / 180 * 10;  // 角速度
float nu = 1.004;             // 動粘性係数

// 各種パラメータ
const float time_max = 1.0;        // シミュレーションする時間 [s]
const int shutter_speed = 800;     // 撮影速度 [1/s]
const float intensity_max = 250.0; // 粒子の最大輝度値 [-]
const float r_particle = 0.2;      // 粒子半径 [mm]
const float camera_angle = 55;     // カメラの設置角度 [°]
const float angle_of_view = 30;    // カメラ(レンズ)の視野角 [°]
const float camera_position = 500; // カメラの設置位置 [mm] (水槽の中心からどの程度カメラが離れているか)
const float focal_length = 25;     // レンズの焦点距離 [mm]
const float magnification = 3;     // 拡大率 [-]

/* 生成する画像の設定 */
const int width_px = 800;                     // 画像の横幅 [px]
const int height_px = 800;                    // 画像の縦幅 [px]
const int binary_size = width_px * height_px; // バイナリデータの大きさ [-]
const int header_size = 1078;                 // 8bit bmp のヘッダーファイル

/* 仮想上の撮影画像の設定 */
const float width_mm = 40;           // 実際の撮影範囲の横幅 [mm]
const float height_mm = 40;          // 実際の撮影範囲の縦幅 [mm]
const float width_shot_center = 50;  // y方向の撮影中心 [mm]
const float height_shot_center = 50; // z方向の撮影中心 [mm]

/* LLSの位置設定 */
const float lls_distance = 0.010;                           // LLS間距離 [mm]
const float lls_1_position = 7.0;                           // 前方のllsの位置 [mm]
const float lls_1_thickness = 0.003;                        // 前方のllsの厚み [mm]
const float lls_2_position = lls_1_position + lls_distance; // 後方のllsの位置 [mm]
const float lls_2_thickness = lls_1_thickness * 3;          // 後方のllsの厚み [mm]

/* 粒子の生成範囲の設定 */
const float range_x_max = lls_2_position;     // x方向の粒子生成範囲　[mm]
const float range_x_min = lls_2_position - 3; // x方向の粒子生成範囲　[mm]

const float range_x = range_x_max - range_x_min; // y方向の粒子生成範囲　[mm]
const float range_y = 20.0 * sqrt(2);            // y方向の粒子生成範囲　[mm]
const float range_z = range_y;                   // z方向の粒子生成範囲　[mm]

/* 粒子の生成量の設定 */
const int num_per_image = 120; // 1枚あたりに映り込む粒子数 [個]
// const int num_per_image = 240;                                                           // 1枚あたりに映り込む粒子数 [個]
const int times = 1.0;                                                                   // 粒子数の倍率 [-]
const float density_particle = num_per_image / (width_mm * height_mm * lls_1_thickness); // 粒子密度 [個/mm^2]
const int num_particle = density_particle * range_x * range_y * range_y * pi * times;    // 生成する粒子数 [-]

/* 校正板 */
const int point_x = 3;
const int point_y = 5;
const int point_z = 10;
const int num_calibration = point_x * point_y * point_z;

/******************************************************************************/

FILE *fp, *fp_r, *fp_w, *gp;
mode_t dir_mode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;
const std::string header_path = "header/800x800_8bit.bmp";        // 使用するヘッダーファイル
const std::string header_path_color = "header/800x800_24bit.bmp"; // 使用するヘッダーファイル
