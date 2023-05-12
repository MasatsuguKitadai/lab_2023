/******************************************************************************
PROGRAM : settings.h
DATE    : 2023/5/12
******************************************************************************/
/** ディレクトリ **/
// const char dir_path[] = "/mnt/d/workspace_HDD/03_numerical_simulation/";
const char dir_path[] = "/mnt/e/workspace_SSD/04_basic_experiment/";
// const char dir_path[] = "/mnt/g/workspace_SSD/03_numerical_simulation/"; // SSD

/** 各種パラメータ **/
const int data_num = 4000;                        // 画像の枚数 [-]
const int width_origin = 800;                     // 画像の横幅 [px]
const int height_origin = 600;                    // 画像の縦幅 [px]
const int size_px = width_origin * height_origin; // 画像の画素数 [px]
const int delta_n = 8;                            // 対応させる時刻差 (枚)
const int w1 = 30;                                // 探査窓 [px] x [px]
const int w2 = 60;                                // 検査窓 [px] x [px]
const int grid_size = 10;                         // グリッドサイズ [px]
const float shutter_speed = 800;                  // 撮影速度 [1/s]
const int border = 10;                            // 校正画像のしきい値 [-]

const int px_8_origin = width_origin * height_origin;        // 8bit画像の画素数 [px]
const int px_24_origin = width_origin * height_origin * 3.0; // 24bit画像の画素数 [px]

/* 撮影画像の設定 */
const float width_mm = 40;           // 実際の撮影範囲の横幅 [mm]
const float height_mm = 40;          // 実際の撮影範囲の縦幅 [mm]
const float width_shot_center = 50;  // y方向の撮影中心 [mm]
const float height_shot_center = 50; // z方向の撮影中心 [mm]

/** 固定パラメータ **/
unsigned char header_8bit[1078]; // 8bit header
unsigned char header_24bit[54];  // 24bit header

/*  */
