/******************************************************************************
PROGRAM : settings.h
DATE    : 2023/5/12
******************************************************************************/
/** ディレクトリ **/
// const char dir_path[] = "/mnt/d/workspace_HDD/03_numerical_simulation/";
const char dir_path[] = "/mnt/e/workspace_SSD/04_basic_experiment";
// const char dir_path[] = "/mnt/g/workspace_SSD/03_numerical_simulation/"; // SSD

/** 各種パラメータ **/
// const int data_num = 4000;                           // 画像の枚数 [-]
const int number = 4000;                             // 撮影枚数 [-]
const int width_origin = 800;                        // 基画像の横幅 [pixel]
const int height_origin = 600;                       // 基画像の縦幅 [pixel]
const int width = 800;                               // 基画像の横幅 [pixel]
const int height = 320;                              // 基画像の縦幅 [pixel]
const int size_pixel = width_origin * height_origin; // 画像の画素数 [pixel]
const int delta_n = 8;                               // 対応させる時刻差 (枚)
const int w1 = 30;                                   // 探査窓 [pixel] x [pixel]
const int w2 = 60;                                   // 検査窓 [pixel] x [pixel]
const int grid_size = 10;                            // グリッドサイズ [pixel]
const float shutter_speed = 800;                     // 撮影速度 [1/s]
const int border = 10;                               // 校正画像のしきい値 [-]

const int px_8_origin = width_origin * height_origin;        // 8bit画像の画素数 [pixel]
const int px_24_origin = width_origin * height_origin * 3.0; // 24bit画像の画素数 [pixel]
const int px_8 = width * height;                             // 8bit画像の画素数 [pixel]

/* 撮影画像の設定 */
const float width_mm = 40;           // 実際の撮影範囲の横幅 [mm]
const float height_mm = 40;          // 実際の撮影範囲の縦幅 [mm]
const float width_shot_center = 50;  // y方向の撮影中心 [mm]
const float height_shot_center = 50; // z方向の撮影中心 [mm]

/* キャリブレーション */
const int calibration_size = 40; // dot画像の大きさ [pixel]
const int points = 5;            // 縦列の数 [-]
const int column = 5;            // 縦列の数 [-]

/** 固定パラメータ **/
unsigned char header_8bit[1078]; // 8bit header
unsigned char header_24bit[54];  // 24bit header

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
