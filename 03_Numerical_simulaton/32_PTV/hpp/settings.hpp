/******************************************************************************
PROGRAM : settings.h
DATE    : 2022/11/11
******************************************************************************/
/** ディレクトリ **/
// const char dir_path[] = "/mnt/d/workspace_HDD/03_numerical_simulation/";
const char dir_path[] = "/mnt/e/workspace_SSD/03_numerical_simulation/";
// const char dir_path[] = "/mnt/g/workspace_SSD/03_numerical_simulation/"; // SSD

/** 各種パラメータ **/
const int data_num = 800;                 // 画像の枚数 [-]
const int width_px = 800;                 // 画像の横幅 [px]
const int height_px = 400;                // 画像の縦幅 [px]
const int size_px = width_px * height_px; // 画像の画素数 [px]
const int delta_n = 8;                    // 対応させる時刻差 (枚)
const int w1 = 30;                        // 探査窓 [px] x [px]
const int w2 = 60;                        // 検査窓 [px] x [px]
const int grid_size = 8;                  // グリッドサイズ [px]
const float shutter_speed = 800;          // 撮影速度 [1/s]

/* 仮想上の撮影画像の設定 */
const float width_mm = 40;           // 実際の撮影範囲の横幅 [mm]
const float height_mm = 20;          // 実際の撮影範囲の縦幅 [mm]
const float width_shot_center = 50;  // y方向の撮影中心 [mm]
const float height_shot_center = 50; // z方向の撮影中心 [mm]
const float border_min = 0.90;       // PTVの相関係数のしきい値 [-]
const float border_max = 1.00;       // PTVの相関係数のしきい値 [-]

/** 固定パラメータ **/
unsigned char header_8bit[1078]; // 8bit header
unsigned char header_24bit[54];  // 24bit header
