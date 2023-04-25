/******************************************************************************
PROGRAM : parameters.h
DATE    : 2022/11/25
******************************************************************************/

/** 画像の設定 **/
// 変換画像のサイズ
const int width = 800;  // [px] : 横幅
const int height = 800; // [px] : 縦幅

// 二値化の閾値
const int border = 50;

/** 相関係数の算出 **/

// 窓の大きさ
const int search_size = 40; // 解析窓のサイズ
const int interr_size = 40; // 固定・参照窓のサイズ

// 相関係数の参照ドットの大きさ
const int size_dot = 40;

// 列の数
int column = 5;

// 縦列の点の数
int points = 5;

// 撮影画像の枚数
int number = 800;

// 撮影回数
int times = 1;

// レーザーシートの位置
float pos_blue = 0;    // [mm]
float pos_green = 2.5; // [mm]

// 画像の対応間隔
int delta = 10;

// PTV のパラメータ
const int cal_size = 40;
const int win_size = 20;

int border_piv = 0;

//  8bit Greyscale データ配列
unsigned char image_0[width][height];
unsigned char image_1[width][height];

// PIV 解析窓配列
unsigned char search[cal_size][cal_size];

// 相関係数の算出用
unsigned char win_area[win_size][win_size];

// 相関係数の保存用配列
double cp_ptv[5][win_size][win_size];
double cp_ptv_sum[win_size][win_size];

/** ラベリング **/

// ラベリング用配列
int label_0[width][height];
int label_1[width][height];

// これより下は変更しない!! /////////////////////////////////////////////////////////

// バイナリ
const int px_8_stretch = width * height;
const int px_24_stretch = width * height * 3;