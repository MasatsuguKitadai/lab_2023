/******************************************************************************
PROGRAM : settings.h
DATE    : 2022/11/11
******************************************************************************/
/** ディレクトリ **/
const char dir_path[] = "/mnt/e/workspace_SSD/04_basic_experiment";

/** 各種パラメータ **/
const int number = 4000;                  // 画像の枚数 [-]
const int width_px = 800;                 // 画像の横幅 [px]
const int height_px = 320;                // 画像の縦幅 [px]
const int size_px = width_px * height_px; // 画像の画素数 [px]
const int delta_n = 10;                   // 対応させる時刻差 (枚)
const int w1 = 30;                        // 探査窓 [px] x [px]
const int w2 = 60;                        // 検査窓 [px] x [px]
const int grid_size = 16;                 // グリッドサイズ [px] (8 pixcel = 1 mm)
const float shutter_speed = 800;          // 撮影速度 [1/s]
const float frame_rate = 800;             // 撮影速度 [1/s]

/* 仮想上の撮影画像の設定 */
const float width_mm = 100.0;          // 実際の撮影範囲の横幅 [mm]
const float height_mm = 40.0;          // 実際の撮影範囲の縦幅 [mm]
const float width_shot_center = 50.0;  // y方向の撮影中心 [mm]
const float height_shot_center = 20.0; // z方向の撮影中心 [mm]
const float border_min = 0.0;          // PTVの相関係数のしきい値 [-]
const float border_max = 1.00;         // PTVの相関係数のしきい値 [-]

/** 固定パラメータ **/
unsigned char header_8bit[1078]; // 8bit header
unsigned char header_24bit[54];  // 24bit header

/******************************************************************************
FUNCTION : Progress_meter
概要     ：進捗割合を表示する
******************************************************************************/
int Progress_meter(const char program_name[], int i, int max, int progress_count)
{
    /* メーターの表示 */
    if (i == 0)
    {
        printf("\n【%s】\n", program_name);
        printf("0    10   20   30   40   50   60   70   80   90  100 [%%]\n"
               "|----|----|----|----|----|----|----|----|----|----|\n"
               "*");
        fflush(stdout);
        progress_count += 1;
    }

    /* 進捗率の計算 */
    const float progress_ratio = (float)i / (float)max * 50.0; // [%]
    if (progress_ratio > progress_count)
    {
        printf("*");
        fflush(stdout);
        progress_count += 1;
    }

    /* 完了 */
    if (i == max - 1)
    {
        printf("*\n\n");
    }

    return progress_count;
}
