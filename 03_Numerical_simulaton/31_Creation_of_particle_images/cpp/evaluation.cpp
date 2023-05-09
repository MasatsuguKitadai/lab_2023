/******************************************************************************
PROGRAM : correct_data
AUTHER  : Masatsugu Kitadai
DATE    : 2023/4/15
*******************************************************************************/

// 設定ファイルの読み込み
#include "../hpp/class.hpp"
#include "../hpp/functions.hpp"
using namespace std;

Parameters lls_1; // 前方のLLSの構造体定義
Parameters lls_2; // 後方のLLSの構造体定義
string name;      // データセットの名前

const float grid_size = 10; // 格子サイズの設定
const int number_y = (width_px / grid_size);
const int number_z = (height_px / grid_size);
const int number = number_y * number_z;

/** プロトタイプ宣言 **/
void Initialization();
float correct_data();
void evaluation(float v_max);
void plot_error();
void plot_velocity();
void plot_vorticity();
void plot_vorticity_profile();

/******************************************************************************
FUNCTION :
 IN ：
OUT ：
******************************************************************************/
int main()
{
    Initialization();
    float v = correct_data();
    printf("v_max = %f\n", v);
    evaluation(v);

    // float v_max = 2.5;
    // evaluation(v_max);

    plot_error();
    plot_velocity();
    plot_vorticity();
    plot_vorticity_profile();

    return 0;
}

/******************************************************************************
FUNCTION : Initialization
概要：各パラメータの設定を行う
 IN ：void
OUT ：void
******************************************************************************/
void Initialization()
{
    /* パラメータの設定 */
    const float dx = 0.010;               // LLS間距離 [mm]
    lls_1.position = 7.0;                 // 前方のllsの位置 [mm]
    lls_1.thickness = 0.003;              // 前方のllsの厚み [mm]
    lls_2.position = lls_1.position + dx; // 前方のllsの位置 [mm]
    lls_2.thickness = 0.009;              // 後方のllsの厚み [mm]
}

/******************************************************************************
FUNCTION : correct_data
概要：
 IN ：
OUT ：
******************************************************************************/
float correct_data()
{
    /* 保存ディレクトリの設定 */
    cout << "Case Name:";
    cin >> name;

    string dir_path_str;

    dir_path_str = main_path + name + "/Evaluation";
    const char *dir_path_main = dir_path_str.c_str();
    mkdir(dir_path_main, dir_mode);

    dir_path_str = main_path + name + "/Evaluation/error";
    const char *dir_path_error = dir_path_str.c_str();
    mkdir(dir_path_error, dir_mode);

    dir_path_str = main_path + name + "/Evaluation/data";
    const char *dir_path_data = dir_path_str.c_str();
    mkdir(dir_path_data, dir_mode);

    float delta_n = 10; // 対応枚数の差
    float v_max = 0;    // 最大周方向移動量

    /* 回転中心の設定 */
    const float y_center = 50; // y方向の渦中心 [mm]
    const float z_center = 50; // y方向の渦中心 [mm]

    float omega = pi / 180 * 10; // 角速度 (case-1, case-4, case-7)
    // float omega = pi / 180 * 5; // 角速度 (case-2, case-5, case-8)
    // float omega = pi / 180 * 15; // 角速度 (case-3, case-6, case-9)
    float nu = 1.004; // 動粘性係数

    /* 配列の作成 */
    vector<vector<float>> position_y_mm(number_y, vector<float>(number_z));
    vector<vector<float>> position_y_px(number_y, vector<float>(number_z));
    vector<vector<float>> v_y(number_y, vector<float>(number_z));

    vector<vector<float>> position_z_mm(number_y, vector<float>(number_z));
    vector<vector<float>> position_z_px(number_y, vector<float>(number_z));
    vector<vector<float>> v_z(number_y, vector<float>(number_z));

    vector<vector<float>> r(number_y, vector<float>(number_z));   // 回転半径 [mm]
    vector<vector<float>> phi(number_y, vector<float>(number_z)); // 回転角 [-]

    /* 算出位置の付与 */
    int count = 0;
    for (int i = 0; i < width_px / grid_size; i++)
        for (int j = 0; j < height_px / grid_size; j++)
        {
            position_y_px[i][j] = grid_size * i; //  [px]
            position_z_px[i][j] = grid_size * j; //  [px]

            position_y_mm[i][j] = position_y_px[i][j] * width_mm / width_px;   //  [mm]
            position_z_mm[i][j] = position_z_px[i][j] * height_mm / height_px; //  [mm]
        }

    /* 関数の読み込み */
    const char read_file[] = "csv/data.csv"; // ファイル名
    vector<float> Zeta;                      // 参考文献からの値 [-]
    vector<float> F;                         // 参考文献からの値 [-]
    vector<float> G;                         // 参考文献からの値 [-]
    vector<float> H;                         // 参考文献からの値 [-]
    double zeta, f, g, h;

    fp = fopen(read_file, "r");

    // 画像ファイルが見つからない場合のエラー処理
    if (fp == NULL)
    {
        printf("Not found : %s \n", read_file);
        exit(-1);
    }

    // ファイルの読み込み
    while (fscanf(fp, "%lf,%lf,%lf,%lf", &zeta, &f, &g, &h) != EOF)
    {
        Zeta.push_back(zeta);
        F.push_back(f);
        G.push_back(g);
        H.push_back(h);
    }

    fclose(fp);

    /* 関数のラグランジュ補間 */

    // zeta の計算
    float zeta_tmp = (lls_1.position + (lls_2.position - lls_1.position) / 2) * sqrt(omega / nu);
    // printf("position [mm] = %.3f\tzeta [-] = %.3f\n", lls_1.position + (lls_2.position - lls_1.position) / 2, zeta_tmp);

    // 関数表の採用
    int index = Find_Closest_Value(zeta_tmp, Zeta);

    // 補完用データの作成
    vector<float> Zeta_lagrange;
    vector<float> F_lagrange;
    vector<float> G_lagrange;
    vector<float> H_lagrange;

    for (int i = -1; i <= 1; i++)
    {
        Zeta_lagrange.push_back(Zeta[index + i]);
        F_lagrange.push_back(F[index + i]);
        G_lagrange.push_back(G[index + i]);
        H_lagrange.push_back(H[index + i]);
    }

    // 関数値の格納
    float F_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, F_lagrange);
    float G_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, G_lagrange);
    float H_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, H_lagrange);
    printf("F = %.3f\tG = %.3f\tH = %.3f\n", F_tmp, G_tmp, H_tmp);

    const float dx = 0.010;                    // LLS間距離 [mm]
    const float vx = sqrt(nu * omega) * H_tmp; //  主流方向の速度

    printf("     Vx = %.3f\n", vx);
    printf("delta n = %.1f\n", dx / vx * 800);

    /* 回転半径の計算 */
    for (int i = 0; i < number_y; i++)
        for (int j = 0; j < number_z; j++)
        {
            float y_tmp = position_y_mm[i][j] - width_mm / 2;  // y方向の回転中心からの距離 [mm]
            float z_tmp = position_z_mm[i][j] - height_mm / 2; // z方向の回転中心からの距離 [mm]
            r[i][j] = sqrt(y_tmp * y_tmp + z_tmp * z_tmp);     // 回転半径の初期値計算
            phi[i][j] = atan2(z_tmp, y_tmp);                   // 回転角の初期値計算
        }

    /* 速度場の計算 */
    for (int i = 0; i < number_y; i++)
        for (int j = 0; j < number_z; j++)
        {
            v_y[i][j] = r[i][j] * omega * F_tmp * cos(phi[i][j]) - r[i][j] * omega * G_tmp * sin(phi[i][j]); // y方向速度の計算 [mm/s]
            v_z[i][j] = r[i][j] * omega * F_tmp * sin(phi[i][j]) + r[i][j] * omega * G_tmp * cos(phi[i][j]); // z方向速度の計算 [mm/s]
        }

    /* データの書き出し(1) */
    string filename = main_path + name + "/Evaluation/data/velocity_correct.dat";
    const char *filename_str = filename.c_str();

    fp = fopen(filename_str, "w");

    for (int i = 0; i < number_y; i++)
        for (int j = 0; j < number_z; j++)
        {
            float v_value = sqrt(v_y[i][j] * v_y[i][j] + v_z[i][j] * v_z[i][j]);
            fprintf(fp, "%f\t%f\t%lf\t%lf\t%lf\n", position_y_mm[i][j], position_z_mm[i][j], v_y[i][j] * (delta_n / shutter_speed) * (width_px / width_mm), v_z[i][j] * (delta_n / shutter_speed) * (width_px / width_mm), v_value);
            if (width_mm / 2 - 8 <= position_y_mm[i][j] && position_y_mm[i][j] <= width_mm / 2 + 8)
                if (height_mm / 2 - 8 <= position_z_mm[i][j] && position_z_mm[i][j] <= height_mm / 2 + 8)
                    if (v_value >= v_max)
                    {
                        v_max = v_value;
                    }
        }

    fclose(fp);

    /* 渦渡場の計算 */
    vector<vector<float>> vorticity(number_y, vector<float>(number_z));
    for (int i = 1; i < number_y - 1; i++)
        for (int j = 1; j < number_z - 1; j++)
        {
            vorticity[i][j] = (v_z[i + 1][j] - v_z[i - 1][j]) / grid_size - (v_y[i][j + 1] - v_y[i][j - 1]) / grid_size;
        }

    /* データの書き出し(2) */
    string filename_2 = main_path + name + "/Evaluation/data/vorticity_correct.dat";
    const char *filename_2_str = filename_2.c_str();

    fp = fopen(filename_2_str, "w");
    for (int i = 0; i < number_y; i++)
    {
        for (int j = 0; j < number_z; j++)
        {
            fprintf(fp, "%f\t%f\t%lf\n", position_y_mm[i][j], position_z_mm[i][j], vorticity[i][j]);
        }
        fprintf(fp, "\n");
    }
    fclose(fp);

    /* 渦度プロファイルの書き込み */
    string filename_3 = main_path + name + "/Evaluation/data/vorticity_profile_correct.dat";
    const char *filename_3_str = filename_3.c_str();

    fp = fopen(filename_3_str, "w");
    for (int i = 0; i < width_px / grid_size; i++)
        for (int j = 0; j < height_px / grid_size; j++)
        {
            if (position_z_px[i][j] == height_px / 2)
            {
                fprintf(fp, "%f\t%f\t%lf\n", position_y_mm[i][j], position_z_mm[i][j], vorticity[i][j]);
            }
        }
    fclose(fp);

    return (v_max);
}

/******************************************************************************
FUNCTION : evaluation
概要：
 IN ：
OUT ：
******************************************************************************/
void evaluation(float v_max)
{
    /* ファイル名の定義 */
    string read_file_1 = main_path + name + "/PTV/PTV_velocity_dat/velocity_" + name + ".dat";
    string read_file_2 = main_path + name + "/Evaluation/data/velocity_correct.dat";
    string result_file = main_path + name + "/Evaluation/error/error.dat";

    /* 配列の作成 */
    vector<vector<float>> vector_y(2, vector<float>(number));
    vector<vector<float>> vector_z(2, vector<float>(number));
    vector<vector<float>> vector_value(2, vector<float>(number));
    vector<vector<float>> vorticity(2, vector<float>(number));
    vector<float> y(number);
    vector<float> z(number);

    /* シミュレーションデータの読み込み */
    const char *read_file_1_str = read_file_1.c_str();
    fp = fopen(read_file_1_str, "r");

    // 画像ファイルが見つからない場合のエラー処理
    if (fp == NULL)
    {
        printf("Not found : %s \n", read_file_1_str);
        exit(-1);
    }

    // ファイルの読み込み
    int count = 0;
    while (fscanf(fp, "%f\t%f\t%f\t%f\t%f", &y[count], &z[count], &vector_y[0][count], &vector_z[0][count], &vector_value[0][count]) != EOF)
    {
        count += 1;
    }
    fclose(fp);

    printf("Simulation data = %d\n", count);

    /* 正解データの読み込み */
    const char *read_file_2_str = read_file_2.c_str();
    fp = fopen(read_file_2_str, "r");

    // 画像ファイルが見つからない場合のエラー処理
    if (fp == NULL)
    {
        printf("Not found : %s \n", read_file_2_str);
        exit(-1);
    }

    // ファイルの読み込み
    count = 0;
    while (fscanf(fp, "%f\t%f\t%f\t%f\t%f", &y[count], &z[count], &vector_y[1][count], &vector_z[1][count], &vector_value[1][count]) != EOF)
    {
        count += 1;
    }
    fclose(fp);

    printf("Test data = %d\n", count);

    /* 誤差計算計算 */
    vector<float> error_y(number);
    vector<float> error_z(number);
    vector<float> error_value(number);

    for (int i = 0; i < number; i++)
    {
        error_y[i] = abs(vector_y[0][i] - vector_y[1][i]) / v_max * 100;
        error_z[i] = abs(vector_z[0][i] - vector_z[1][i]) / v_max * 100;
        error_value[i] = abs(vector_value[0][i] - vector_value[1][i]) / v_max * 100;
    }

    /* 誤差率の書き込み */
    const char *result_file_str = result_file.c_str();
    count = 0;

    fp = fopen(result_file_str, "w");
    for (int i = 0; i < width_px / grid_size; i++)
    {
        for (int j = 0; j < height_px / grid_size; j++)
        {
            fprintf(fp, "%f\t%f\t%f\t%f\t%f\n", y[count], z[count], error_y[count], error_z[count], error_value[count]);
            count += 1;
        }
        fprintf(fp, "\n");
    }

    fclose(fp);

    /* RMSE の算出 */
    float rmse_y = 0;
    float rmse_z = 0;
    float rmse_yz = 0;

    // 合計値の計算
    for (int i = 0; i < number; i++)
    {
        if (vector_y[0][i] != 0 && vector_z[0][i] != 0)
        {
            rmse_y += (vector_y[0][i] - vector_y[1][i]) * (vector_y[0][i] - vector_y[1][i]);
            rmse_z += (vector_z[0][i] - vector_z[1][i]) * (vector_z[0][i] - vector_z[1][i]);
            rmse_yz += (vector_y[0][i] - vector_y[1][i]) * (vector_y[0][i] - vector_y[1][i]) + (vector_z[0][i] - vector_z[1][i]) * (vector_z[0][i] - vector_z[1][i]);
        }
    }

    rmse_y = sqrt(rmse_y / number);
    rmse_z = sqrt(rmse_z / number);
    rmse_yz = sqrt(rmse_yz / number);

    /* RMSE 誤差率の算出 */
    float rmse_y_per = rmse_y / v_max * 100;
    float rmse_z_per = rmse_z / v_max * 100;
    float rmse_yz_per = rmse_yz / v_max * 100;

    printf("RMSE:  y = %.3f [mm/s]\t%.3f[%]\n", rmse_y, rmse_y_per);
    printf("RMSE:  z = %.3f [mm/s]\t%.3f[%]\n", rmse_z, rmse_z_per);
    printf("RMSE: yz = %.3f [mm/s]\t%.3f[%]\n", rmse_yz, rmse_yz_per);
}

/******************************************************************************
FUNCTION :
概要：
 IN ：
OUT ：
******************************************************************************/
void plot_error()
{
    /** Gnuplot **/
    string result_file = main_path + name + "/Evaluation/error/error.dat";
    const char *result_file_str = result_file.c_str();

    string graphname_y = main_path + name + "/Evaluation/error/error_y.png";
    string graphname_z = main_path + name + "/Evaluation/error/error_z.png";
    string graphname_value = main_path + name + "/Evaluation/error/error_value.png";

    char graphtitle[] = "Error Value [%]";
    const char *graphname_y_str = graphname_y.c_str();
    const char *graphname_z_str = graphname_z.c_str();
    const char *graphname_value_str = graphname_value.c_str();

    // 軸の設定

    // range x
    float x_min = 12.5;
    float x_max = width_mm - 12.5;

    // range y
    float y_min = 12.5;
    float y_max = height_mm - 12.5;

    // range color
    float cb_min = 0;
    float cb_max = 100;

    // label
    const char *xxlabel = "y [px]";
    const char *yylabel = "z [px] ";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    // fprintf(gp, "set terminal svg enhanced size 1000, 1000 font 'Times New Roman, 16'\n");
    fprintf(gp, "set terminal png enhanced size 1200, 600 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname_y_str);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title 'Error Value : E_y [%]'\n");
    fprintf(gp, "set view map\n");

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.0\n");
    fprintf(gp, "set ylabel offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.0\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "splot '%s' using 1:2:3 with pm3d notitle\n", result_file_str);

    // z成分
    fprintf(gp, "set title 'Error Value : E_z [%]'\n");
    fprintf(gp, "set output '%s'\n", graphname_z_str);
    fprintf(gp, "splot '%s' using 1:2:4 with pm3d notitle\n", result_file_str);

    // 総成分
    fprintf(gp, "set title 'Error Value : E [%]'\n");
    fprintf(gp, "set output '%s'\n", graphname_value_str);
    fprintf(gp, "splot '%s' using 1:2:5 with pm3d notitle\n", result_file_str);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}

/******************************************************************************
FUNCTION :
概要：
 IN ：
OUT ：
******************************************************************************/
void plot_velocity()
{
    /** Gnuplot **/
    string filename = main_path + name + "/Evaluation/data/velocity_correct.dat";
    string graphname = main_path + name + "/Evaluation/data/velocity.svg";

    const char *filename_str = filename.c_str();
    const char *graphname_str = graphname.c_str();
    char graphtitle[] = "Velocoty : Ture value [mm/s]";

    // 軸の設定

    // range x
    float x_min = 12.5;
    float x_max = width_mm - 12.5;

    // range y
    float y_min = 12.5;
    float y_max = height_mm - 12.5;

    // range color
    float cb_min = 0;
    float cb_max = 2.5;

    // label
    const char *xxlabel = "y [mm]";
    const char *yylabel = "z [mm] ";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 500, 500 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname_str);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s'\n", graphtitle);

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.0\n");
    fprintf(gp, "set ylabel offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.0\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:2:3:4:5 with vectors lc palette lw 1 notitle\n", filename_str);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);

    // return 0;
}

/******************************************************************************
FUNCTION :
概要：
 IN ：
OUT ：
******************************************************************************/
void plot_vorticity()
{
    /** Gnuplot **/
    string filename = main_path + name + "/Evaluation/data/vorticity_correct.dat";
    string graphname = main_path + name + "/Evaluation/data/vorticity.svg";

    const char *filename_str = filename.c_str();
    const char *graphname_str = graphname.c_str();
    char graphtitle[] = "Correct Data [Vorticity]";

    // 軸の設定

    // range x
    float x_min = 12.5;
    float x_max = width_mm - 12.5;

    // range y
    float y_min = 12.5;
    float y_max = height_mm - 12.5;

    // range color
    float cb_min = -0.5;
    float cb_max = 0.5;

    // label
    const char *xxlabel = "y [px]";
    const char *yylabel = "z [px] ";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 1000, 1000 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio -1\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname_str);

    // 非表示
    fprintf(gp, "unset key\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s'\n", graphtitle);
    fprintf(gp, "set view map\n");

    // ベクトルの色付け
    fprintf(gp, "set palette rgb 22,13,-31\n");
    fprintf(gp, "set cbrange['%.3f':'%.3f']\n", cb_min, cb_max);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.0\n");
    fprintf(gp, "set ylabel offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.0\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "splot '%s' using 1:2:3 with pm3d notitle\n", filename_str);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);

    // return 0;
}

/******************************************************************************
FUNCTION : plot_vorticity_profile
概要：真値の渦度プロファイルについて描画する
 IN ：
OUT ：
******************************************************************************/
void plot_vorticity_profile()
{
    /** Gnuplot **/
    string filename_1 = main_path + name + "/Evaluation/data/vorticity_profile_correct.dat";
    string filename_2 = main_path + name + "/PTV/PTV_vorticity_dat/vorticity_profile.dat";
    string graphname = main_path + name + "/Evaluation/data/vorticity_profile.svg";

    const char *filename_1_str = filename_1.c_str();
    const char *filename_2_str = filename_2.c_str();
    const char *graphname_str = graphname.c_str();
    char graphtitle[] = "Vorticity Profile (z = 200)";

    // 軸の設定

    // range x
    float x_min = 12.5;
    float x_max = width_mm - 12.5;

    // range y
    float y_min = -1.5;
    float y_max = 1.5;

    // label
    const char *xxlabel = "y [px]";
    const char *yylabel = "Vorticity [-] ";

    // Gnuplot 呼び出し
    if ((gp = popen("gnuplot", "w")) == NULL)
    {
        printf("gnuplot is not here!\n");
        exit(0); // gnuplotが無い場合、異常ある場合は終了
    }

    fprintf(gp, "set terminal svg enhanced size 1000, 1000 font 'Times New Roman, 16'\n");
    fprintf(gp, "set size ratio 0.5\n");

    // 出力ファイル
    fprintf(gp, "set output '%s'\n", graphname_str);

    // 非表示
    fprintf(gp, "set key left bottom font 'Times New Roman, 20'\n");

    // 軸の範囲
    fprintf(gp, "set xrange [%.3f:%.3f]\n", x_min, x_max);
    fprintf(gp, "set yrange [%.3f:%.3f]\n", y_min, y_max);

    // グラフタイトル
    fprintf(gp, "set title '%s'\n", graphtitle);

    // 軸ラベル
    fprintf(gp, "set xlabel '%s'\n", xxlabel);
    fprintf(gp, "set ylabel '%s'\n", yylabel);

    // 軸のラベル位置
    fprintf(gp, "set xlabel offset 0.0, 0.0\n");
    fprintf(gp, "set ylabel offset 0.0, 0.0\n");

    // 軸の数値位置
    fprintf(gp, "set xtics offset 0.0, 0.0\n");
    fprintf(gp, "set ytics offset 0.0, 0.0\n");

    // グラフの出力
    fprintf(gp, "plot '%s' using 1:3 with lines lc 'red' title 'Correct', '%s' using 1:3 with lines lc 'black' title 'Simulation'\n", filename_1_str, filename_2_str);

    fflush(gp); // Clean up Data

    fprintf(gp, "exit\n"); // Quit gnuplot

    pclose(gp);
}
