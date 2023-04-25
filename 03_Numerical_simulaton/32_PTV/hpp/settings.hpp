/******************************************************************************
PROGRAM : settings.h
DATE    : 2022/11/11
******************************************************************************/

// 変更しない!! //

// 一般変数
int num[10];
int counter;

// ファイルポインタ
FILE *fp, *fp_2, *fp_3, *gp;

// 円周率 pi
double pi = 4 * atan(1.0);

// ディレクトリ配列
char dirname[10][200];
const char dir_path[] = "/mnt/e/workspace_SSD/03_numerical_simulation";

// ヘッダー配列
unsigned char header_8bit[1078]; // 8bit header
unsigned char header_24bit[54];  // 24bit header

unsigned char header_buf[1878]; // 8bit header

// ファイル配列
char imagename[10][200];

// 計算用バッファー
float cal[10];
float buf[100];

// フォルダ作成用
mode_t dirmode = S_IRUSR | S_IWUSR | S_IXUSR | S_IRGRP | S_IWGRP | S_IXGRP | S_IROTH | S_IXOTH | S_IXOTH;
