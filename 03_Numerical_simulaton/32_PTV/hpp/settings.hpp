/******************************************************************************
PROGRAM : settings.h
DATE    : 2022/11/11
******************************************************************************/
/** ディレクトリ **/
// const char dir_path[] = "/mnt/d/workspace_HDD/03_numerical_simulation/";
const char dir_path[] = "/mnt/e/workspace_SSD/03_numerical_simulation/";

/** 各種パラメータ **/
const int data_num = 800;                 // 画像の枚数 [-]
const int width_px = 800;                 // 画像の横幅 [px]
const int height_px = 800;                // 画像の縦幅 [px]
const int width_mm = 40;                  // 画像の横幅 [mm]
const int height_mm = 40;                 // 画像の縦幅 [mm]
const int size_px = width_px * height_px; // 画像の画素数 [px]
const int delta_n = 10;                   // 対応させる時刻差 (枚)
const int w1 = 30;                        // 探査窓 [px] x [px]
const int w2 = 60;                        // 検査窓 [px] x [px]
const int grid_size = 10;                 // グリッドサイズ [px]
const float shutter_speed = 800;          // 撮影速度 [1/s]

/** 固定パラメータ **/
unsigned char header_8bit[1078]; // 8bit header
unsigned char header_24bit[54];  // 24bit header
