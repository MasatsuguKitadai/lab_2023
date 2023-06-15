/******************************************************************************
PROGRAM : simulation
AUTHER  : Masatsugu Kitadai
DATE    : 2023/1/31
*******************************************************************************
【概要】”3次元空間における粒子像の作成プログラム”

パラメータ
・レーザーシート厚みの変更
・レーザーシート位置の変更
・粒子数密度の変更
・作成する画像サイズの変更
・発生させる流れ場の変更
・撮影速度の変更

動作
(1) 粒子像の位置情報の生成 （1[s]間）
・最初の粒子位置

(2) レーザーシート位置に対応した粒子画像の生成

x方向 正：主流方向
y方向 正：奥手方向
z方向 正：鉛直下方向

*******************************************************************************/

// 設定ファイルの読み込み
#include "../hpp/class.hpp"
#include "../hpp/functions.hpp"
string dir_path;

// プロトタイプ宣言
void Initialization();
void Make_Directory(string name);
void File_Name_Creator(Parameters &parameter, int i);
void Simulate_Rotation_near_the_ground(float seconds);
void Write_dat_Tank(string file_path, vector<float> x, vector<float> y, vector<float> z, float max, float min);
void Write_dat_grid(Parameters parameter);
void Write_dat_Screen(string file_path_read, string file_path_write);
void Cal_Intensity(vector<vector<float>> &intensity, float position, float min, float max, float mu, float sigma);
void Cal_Intensity_Calibration(vector<vector<float>> &intensity);
void Create_Img_8bit(string file_path, vector<vector<float>> &intensity);
void Create_Img_24bit(string file_path, vector<vector<float>> &intensity_blue, vector<vector<float>> &intensity_green);

/* 進捗表示 */
const char program_name[] = "NUMERICAL SIMULATION";
int progress_counter = 0; // 進捗表記用

/******************************************************************************
FUNCTION : Settings
概要：構造体の各要素に任意の値を与える
 IN ：void
OUT ：void
******************************************************************************/

/* 構造体の定義 */

/* 各方向の粒子成分 */
Axis x; // x方向のパラメータ
Axis y; // y方向のパラメータ
Axis z; // z方向のパラメータ

/* LLSの設定 */
Parameters lls_1;       // 前方のLLSの構造体定義
Parameters lls_2;       // 後方のLLSの構造体定義
Parameters calibration; // 後方のLLSの構造体定義

Gnuplot gnuplot; // gnuplotの設定

/******************************************************************************
FUNCTION : main
概要：メインの処理
******************************************************************************/

int main()
{
    /* 保存ディレクトリの設定 */
    char name_tmp[100];
    sprintf(name_tmp, "%.f-%d", deg, num_per_image);
    string name = string(name_tmp);

    /* 保存ディレクトリの設定 */
    // string name;
    // cout << "Case Name:";
    // cin >> name;

    /* ディレクトリパスの設定 */
    dir_path = main_path + name + "/";
    cout << dir_path << endl;

    /* 設定の書き出し */
    cout << endl;
    cout << "////////////////////////////////////////////" << endl;
    cout << "Density (1) [個/mm3] : " << density_particle << endl;
    cout << "Density (2) [個/枚]  : " << num_per_image << endl;
    cout << "Particle    [-]      : " << num_particle << endl;
    cout << "Omega       [rad/s]  : " << deg << " × 180/pi" << endl;
    cout << "////////////////////////////////////////////" << endl;
    cout << endl;

    /* 初期設定 */
    Initialization();

    /* ディレクトリの作成 */
    Make_Directory(lls_1.name);
    Make_Directory(lls_2.name);
    Make_Directory("Full");

    /* グリッドファイルの作成 */
    Write_dat_grid(lls_1); // LLS(1)
    Write_dat_grid(lls_2); // LLS(2)

    Write_dat_Screen(lls_1.dat_file_grid_tank, lls_1.dat_file_grid_screen); //  カメラ座標系からスクリーン座標系への変換
    Write_dat_Screen(lls_2.dat_file_grid_tank, lls_2.dat_file_grid_screen); //  カメラ座標系からスクリーン座標系への変換

    /* 乱数の生成 */
    srand((unsigned int)time(NULL));

    /* 配列の準備 */
    x.position_tank.clear();
    y.position_tank.clear();
    z.position_tank.clear();

    /* 配列のリサイズ */
    x.Resize_Vector(num_particle);
    y.Resize_Vector(num_particle);
    z.Resize_Vector(num_particle);

    /* 粒子に3次元の初期位置を与える */
    x.Initial_Position(num_particle);
    y.Initial_Position(num_particle);
    z.Initial_Position(num_particle);

    for (int i = 1; i <= shutter_speed * time_max; i++)
    {
        /* 進捗表示 */
        progress_counter = Progress_meter(program_name, i - 1, shutter_speed * time_max, progress_counter);

        /* ベクターの初期化 */
        lls_1.intensity.clear();                                    // LLS(1)
        lls_1.intensity.resize(height_px, vector<float>(width_px)); //

        lls_2.intensity.clear();                                    // LLS(2)
        lls_2.intensity.resize(height_px, vector<float>(width_px)); //

        float seconds = 1.0 / shutter_speed * (i - 1.0);

        /* 回転板に引きずられる流れのシミュレーション */
        Simulate_Rotation_near_the_ground(seconds);

        /* 位置情報の書き出しとグラフの作成 */
        gnuplot.seconds = seconds; // 描画する時刻 [s]

        // すべての粒子位置の書き出しと3dグラフ作成
        // 水槽座標系について
        string dat_file_3d_tank = dir_path + "Full/particle_position_tank/" + to_string(i) + ".dat"; // datファイルのパス
        string svg_file_3d_tank = dir_path + "Full/3d_tank_png/" + to_string(i) + ".png";            // datファイルのパス
        Write_dat_Tank(dat_file_3d_tank, x.position_tank, y.position_tank, z.position_tank, 250, -250);
        gnuplot.Plot_3d_Tank(dat_file_3d_tank, svg_file_3d_tank, lls_1.dat_file_grid_tank, lls_2.dat_file_grid_tank);

        /* ファイル名の作成 */
        File_Name_Creator(lls_1, i);
        File_Name_Creator(lls_2, i);

        // LLS(1) の粒子位置の書き出しとyzグラフ作成
        Write_dat_Tank(lls_1.dat_file_tank, x.position_tank, y.position_tank, z.position_tank, lls_1.max, lls_1.min); // datファイルの書き出し

        // LLS(1) の水槽座標，yzグラフ作成
        gnuplot.Plot_yz_Tank(lls_1.dat_file_tank, lls_1.graph_file_tank, lls_1.dat_file_grid_tank); // yz面グラフの書き出し(前方)

        // LLS(1) のカメラ座標からスクリーン座標へ変換
        Write_dat_Screen(dat_file_3d_tank, lls_1.dat_file_screen);

        /* 前方のLLSで撮影される粒子像の書き出し */
        Cal_Intensity(lls_1.intensity, lls_1.position, lls_1.min, lls_1.max, lls_1.position, lls_1.sigma);
        Create_Img_8bit(lls_1.bmp_file, lls_1.intensity);

        // LLS(2) の粒子位置の書き出し
        Write_dat_Tank(lls_2.dat_file_tank, x.position_tank, y.position_tank, z.position_tank, lls_2.max, lls_2.min); // datファイルの書き出し

        // LLS(2) の水槽座標，yzグラフ作成
        gnuplot.Plot_yz_Tank(lls_2.dat_file_tank, lls_2.graph_file_tank, lls_2.dat_file_grid_tank); // yz面グラフの書き出し(後方)

        /* 後方のLLSで撮影される粒子像 */
        Cal_Intensity(lls_2.intensity, lls_2.position, lls_2.min, lls_2.max, lls_2.position, lls_2.sigma);
        Create_Img_8bit(lls_2.bmp_file, lls_2.intensity);

        string full_bmp_file = dir_path + "Full/particle_image_bmp/" + to_string(i) + ".bmp"; // datファイルのパス
        Create_Img_24bit(full_bmp_file, lls_1.intensity, lls_2.intensity);
    }

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
    lls_1.position = lls_1_position;   // 前方のllsの位置 [mm]
    lls_1.thickness = lls_1_thickness; // 前方のllsの厚み [mm]
    lls_2.thickness = lls_2_thickness; // 後方のllsの厚み [mm]

    /* llsの設定 */
    lls_1.name = "LLS_1";
    lls_1.dat_file_grid_tank = dir_path + "/" + lls_1.name + "/LLS_zone_tank/zone.dat";     //
    lls_1.dat_file_grid_camera = dir_path + "/" + lls_1.name + "/LLS_zone_camera/zone.dat"; //
    lls_1.dat_file_grid_screen = dir_path + "/" + lls_1.name + "/LLS_zone_screen/zone.dat"; //
    lls_1.Cal();                                                                            // 初期設定

    lls_2.name = "LLS_2";
    lls_2.position = lls_1.position + lls_distance;                                         // 後方のllsの位置 [mm]
    lls_2.dat_file_grid_tank = dir_path + "/" + lls_2.name + "/LLS_zone_tank/zone.dat ";    //
    lls_2.dat_file_grid_camera = dir_path + "/" + lls_2.name + "/LLS_zone_camera/zone.dat"; //
    lls_2.dat_file_grid_screen = dir_path + "/" + lls_2.name + "/LLS_zone_screen/zone.dat"; //
    lls_2.Cal();                                                                            // 初期設定

    /* 粒子の生成範囲 */
    x.range_max = range_x_max; // x方向の粒子生成の下限範囲 [mm]
    x.range_min = range_x_min; // x方向の粒子生成の上限範囲 [mm]
    x.Cal_Range();

    y.range_max = width_shot_center + range_y; // y方向の粒子生成の下限範囲 [mm]
    y.range_min = width_shot_center - range_y; // y方向の粒子生成の上限範囲 [mm]
    y.Cal_Range();

    z.range_max = height_shot_center + range_z; // z方向の粒子生成の下限範囲 [mm]
    z.range_min = height_shot_center - range_z; // z方向の粒子生成の上限範囲 [mm]
    z.Cal_Range();

    /* Gnuplotの設定 */
    gnuplot.dir_path = dir_path; // ディレクトリのパス

    gnuplot.x_max = lls_1.position + 0.1; // x方向軸の上限範囲 [mm]
    gnuplot.x_min = lls_1.position - 0.1; // x方向軸の下限範囲 [mm]

    gnuplot.y_max = y.range_max; // y方向軸の上限範囲 [mm]
    gnuplot.y_min = y.range_min; // y方向軸の下限範囲 [mm]

    gnuplot.z_max = z.range_max; // z方向軸の上限範囲 [mm]
    gnuplot.z_min = z.range_min; // z方向軸の下限範囲 [mm]
}

/******************************************************************************
FUNCTION : Make_Directory
概要：ディレクトリを作成する
 IN ：void
OUT ：void
******************************************************************************/
void Make_Directory(string name)
{
    string dir_path_str;

    dir_path_str = dir_path;
    const char *dir_path_main = dir_path_str.c_str();
    mkdir(dir_path_main, dir_mode);

    // メインディレクトリの作成
    dir_path_str = dir_path + "/" + name;
    const char *dir_path_name = dir_path_str.c_str();
    mkdir(dir_path_name, dir_mode);

    // bmp ディレクトリ
    dir_path_str = dir_path + "/" + name + "/particle_image_bmp";
    const char *dir_path_bmp = dir_path_str.c_str();
    mkdir(dir_path_bmp, dir_mode);

    // dat ディレクトリ
    dir_path_str = dir_path + "/" + name + "/particle_position_tank";
    const char *dir_path_dat_tank = dir_path_str.c_str();
    mkdir(dir_path_dat_tank, dir_mode);

    dir_path_str = dir_path + "/" + name + "/particle_position_camera";
    const char *dir_path_dat_camera = dir_path_str.c_str();
    mkdir(dir_path_dat_camera, dir_mode);

    dir_path_str = dir_path + "/" + name + "/particle_position_screen";
    const char *dir_path_dat_screen = dir_path_str.c_str();
    mkdir(dir_path_dat_screen, dir_mode);

    // yz_tank ディレクトリ
    dir_path_str = dir_path + "/" + name + "/yz_tank_svg";
    const char *dir_path_tank_yz = dir_path_str.c_str();
    mkdir(dir_path_tank_yz, dir_mode);

    // xy_camera ディレクトリ
    dir_path_str = dir_path + "/" + name + "/xy_camera_svg";
    const char *dir_path_camera_xy = dir_path_str.c_str();
    mkdir(dir_path_camera_xy, dir_mode);

    // LLSゾーン ディレクトリ
    dir_path_str = dir_path + "/" + name + "/LLS_zone_tank";
    const char *dir_path_grid_tank = dir_path_str.c_str();
    mkdir(dir_path_grid_tank, dir_mode);

    dir_path_str = dir_path + "/" + name + "/LLS_zone_camera";
    const char *dir_path_grid_camera = dir_path_str.c_str();
    mkdir(dir_path_grid_camera, dir_mode);

    dir_path_str = dir_path + "/" + name + "/LLS_zone_screen";
    const char *dir_path_grid_screen = dir_path_str.c_str();
    mkdir(dir_path_grid_screen, dir_mode);

    // dir_path_str = dir_path + "/" + name + "/3d_tank_svg";
    dir_path_str = dir_path + "/" + name + "/3d_tank_png";
    const char *dir_path_svg_tank = dir_path_str.c_str();
    mkdir(dir_path_svg_tank, dir_mode);

    // tank ディレクトリ
    dir_path_str = dir_path + "/" + name + "/3d_camera_svg";
    const char *dir_path_svg_camera = dir_path_str.c_str();
    mkdir(dir_path_svg_camera, dir_mode);
}

/******************************************************************************
FUNCTION : File_Name_Creator
概要：ファイル名を作成する
 IN ：
OUT ：
@param void
******************************************************************************/
void File_Name_Creator(Parameters &parameter, int i)
{
    parameter.dat_file_tank = dir_path + parameter.name + "/particle_position_tank/" + to_string(i) + ".dat";     // datファイルのパス
    parameter.graph_file_tank = dir_path + parameter.name + "/yz_tank_svg/" + to_string(i) + ".svg";              // svgファイルのパス
    parameter.dat_file_camera = dir_path + parameter.name + "/particle_position_camera/" + to_string(i) + ".dat"; // datファイルのパス
    parameter.graph_file_camera = dir_path + parameter.name + "/xy_camera_svg/" + to_string(i) + ".svg";          // svgファイルのパス
    parameter.dat_file_screen = dir_path + parameter.name + "/particle_position_screen/" + to_string(i) + ".dat"; // datファイルのパス
    parameter.bmp_file = dir_path + parameter.name + "/particle_image_bmp/" + to_string(i) + ".bmp";
}

/******************************************************************************
FUNCTION : Simulate_Rotation_near_the_ground
概要：参考文献による流れの再現
 IN ：seconds：計算する時刻 [s]
OUT ：void /  x[]，y[]，z[] 配列に値を格納する
******************************************************************************/
void Simulate_Rotation_near_the_ground(float seconds)
{
    /* 回転中心の設定 */
    const float y_center = 50; // y方向の渦中心 [mm]
    const float z_center = 50; // y方向の渦中心 [mm]

    vector<float> r(x.position_tank.size());   // 回転半径 [mm]
    vector<float> phi(x.position_tank.size()); // 回転角 [-]

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

    /* 回転半径の計算 */
    for (int i = 0; i < x.position_tank.size(); i++)
    {
        float y_tmp = y.position_tank[i] - y_center; // y方向の回転中心からの距離 [mm]
        float z_tmp = z.position_tank[i] - z_center; // z方向の回転中心からの距離 [mm]
        r[i] = sqrt(y_tmp * y_tmp + z_tmp * z_tmp);  // 回転半径の初期値計算
        phi[i] = atan2(z_tmp, y_tmp);                // 回転角の初期値計算
    }

    float vr_ave = 0;
    float vtheta_ave = 0;
    float u_ave = 0;
    float r_ave = 0;

    /* 粒子位置の計算 */
    for (int i = 0; i < x.position_tank.size(); i++)
    {
        // zeta の計算
        float zeta_tmp = x.position_tank[i] * sqrt(omega / nu);

        // 関数表の採用
        int index = Find_Closest_Value(zeta_tmp, Zeta);

        // 補完用データの作成
        vector<float> Zeta_lagrange;
        vector<float> F_lagrange;
        vector<float> G_lagrange;
        vector<float> H_lagrange;

        for (int j = -1; j <= 1; j++)
        {
            Zeta_lagrange.push_back(Zeta[index + j]);
            F_lagrange.push_back(F[index + j]);
            G_lagrange.push_back(G[index + j]);
            H_lagrange.push_back(H[index + j]);
        }

        // 関数値の格納
        float F_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, F_lagrange);
        float G_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, G_lagrange);
        float H_tmp = Lagrange_Interpolation(zeta_tmp, Zeta_lagrange, H_lagrange);

        // 粒子位置の積分
        r[i] = r[i] + r[i] * omega * F_tmp * 1 / shutter_speed;     // 半径方向の位置
        phi[i] = phi[i] + r[i] * omega * G_tmp * 1 / shutter_speed; // 周方向の位置（角度）

        // 速度の計算
        float vy = r[i] * omega * F_tmp * cos(phi[i]) - r[i] * omega * G_tmp * sin(phi[i]); // y方向速度の計算 [mm]
        float vz = r[i] * omega * F_tmp * sin(phi[i]) + r[i] * omega * G_tmp * cos(phi[i]); // z方向速度の計算 [mm]

        // x.position_tank[i] = x.position_tank[i] + sqrt(r[i] * omega) * H_tmp * 1 / shutter_speed; // x座標の計算 [mm]
        x.position_tank[i] = x.position_tank[i] + sqrt(nu * omega) * H_tmp * 1 / shutter_speed; // x座標の計算 [mm]
        y.position_tank[i] = y.position_tank[i] + vy * 1 / shutter_speed;                       // y座標の計算 [mm]
        z.position_tank[i] = z.position_tank[i] + vz * 1 / shutter_speed;                       // z座標の計算 [mm]

        // 合計値の計算
        vr_ave += r[i] * omega * F_tmp;
        vtheta_ave += r[i] * omega * G_tmp;
        u_ave += sqrt(nu * omega) * H_tmp;
        r_ave += r[i];
    }

    // 平均値の計算
    vr_ave = vr_ave / x.position_tank.size();
    vtheta_ave = vtheta_ave / x.position_tank.size();
    u_ave = u_ave / x.position_tank.size();
    r_ave = r_ave / x.position_tank.size();
    // printf("r = %.3f\tVr = %.3f\tVΘ = %.3f\tu = %.3f\t", r_ave, vr_ave, vtheta_ave, u_ave);
}

/******************************************************************************
FUNCTION : Write_dat_Tank
概要：水槽座標系におけるLLSに対応した粒子位置をdatファイルに書き出す
 IN ：@param Write_dat_Full_Tank の引数, max：LLSの最大位置，min：LLSの最小位置
OUT ：void / 指定のファイル名に書き出し
******************************************************************************/
void Write_dat_Tank(string file_path, vector<float> x, vector<float> y, vector<float> z, float max, float min)
{
    const char *file_path_c = file_path.c_str(); // str型からchar型への変換

    fp = fopen(file_path_c, "w");

    for (int i = 0; i < x.size(); i++)
    {
        if (min < x[i] && x[i] < max)
        {
            // 文字列の結合
            string str;                                                              // 座標の統合した文字列
            str = to_string(x[i]) + "\t" + to_string(y[i]) + "\t" + to_string(z[i]); // 結合

            // 書き出し
            fprintf(fp, "%s\n", str.c_str());
        }
    }

    fclose(fp);
}

/******************************************************************************
FUNCTION : Write_dat_grid
概要：LLSの範囲を書き出す
 IN ：
OUT ：
******************************************************************************/
void Write_dat_grid(Parameters parameter)
{
    const char *file_path_c = parameter.dat_file_grid_tank.c_str(); // str型からchar型への変換

    fp = fopen(file_path_c, "w");

    // レーザーシートの設定
    float y_max = width_shot_center + width_mm / 2;
    float y_min = width_shot_center - width_mm / 2;
    float z_max = height_shot_center + height_mm / 2;
    float z_min = height_shot_center - height_mm / 2;

    // 文字列の結合
    string grid_1, grid_2, grid_3, grid_4;                                                      // 座標の統合した文字列
    grid_1 = to_string(parameter.position) + "\t" + to_string(y_min) + "\t" + to_string(z_min); // 結合
    grid_2 = to_string(parameter.position) + "\t" + to_string(y_max) + "\t" + to_string(z_min); // 結合
    grid_3 = to_string(parameter.position) + "\t" + to_string(y_max) + "\t" + to_string(z_max); // 結合
    grid_4 = to_string(parameter.position) + "\t" + to_string(y_min) + "\t" + to_string(z_max); // 結合

    // 書き出し
    fprintf(fp, "%s\n", grid_1.c_str());
    fprintf(fp, "%s\n", grid_2.c_str());
    fprintf(fp, "%s\n", grid_3.c_str());
    fprintf(fp, "%s\n", grid_4.c_str());
    fprintf(fp, "%s\n", grid_1.c_str());

    fclose(fp);
}

/******************************************************************************
FUNCTION : Write_dat_Screen
概要：シミュレーションで撮影される画像の範囲を描画
 IN ：file_path_read：カメラ座標系の粒子位置，file_path_read：スクリーン座標系の粒子位置
OUT ：
******************************************************************************/
void Write_dat_Screen(string file_path_read, string file_path_write)
{
    /* 視野角の計算 */
    const char *file_path_write_c = file_path_write.c_str(); // str型からchar型への変換
    const char *file_path_read_c = file_path_read.c_str();   // str型からchar型への変換

    vector<float> x_tank{};
    vector<float> y_tank{};
    vector<float> z_tank{};

    x_tank.resize(x.position_tank.size());
    y_tank.resize(x.position_tank.size());
    z_tank.resize(x.position_tank.size());

    fp_w = fopen(file_path_write_c, "w");

    for (int i = 0; i < x.position_tank.size(); i++)
    {
        x.position_screen[i] = (y.position_tank[i] - width_shot_center) * width_px / width_mm + width_px / 2;     // 画像上の位置の左右方向の位置 [px];
        y.position_screen[i] = (z.position_tank[i] - height_shot_center) * height_px / height_mm + height_px / 2; // 画像上の位置の上下方向の位置 [px];
        fprintf(fp_w, "%f\t%f\t%f\n", x.position_tank[i], x.position_screen[i], y.position_screen[i]);
        // printf("%f\t%f\t%f\n", x_tank[i], x.position_screen[i], y.position_screen[i]);
    }

    fclose(fp_w);
}

/******************************************************************************
FUNCTION : Cal_Intensity
概要：粒子の位置に対応した輝度値を与える
 IN ：*data：使用するオブジェクト名
OUT ：
******************************************************************************/
void Cal_Intensity(vector<vector<float>> &intensity, float position, float min, float max, float mu, float sigma)
{
    /* yzの位置による輝度値の計算 */
    float sigma_particle = r_particle / 3 * width_px / width_mm;
    int n = 0;

    for (int i = 0; i < x.position_tank.size(); i++)
    {
        /* レーザーシート内の粒子のみ */
        if (min < x.position_tank[i] && x.position_tank[i] < max)
        {
            float intensity_x = intensity_max * Gaussian(x.position_tank[i], mu, sigma); // x方向輝度値計算 [-]

            for (int j = 0; j < height_px; j++)
                for (int k = 0; k < width_px; k++)
                {
                    //  輝度値の計算
                    float intensity_tmp = intensity_x * Gaussian_2D(k, j, x.position_screen[i], y.position_screen[i], sigma_particle);

                    // 配列への格納
                    if (intensity_tmp > intensity[j][k])
                    {
                        intensity[j][k] = intensity_tmp;
                        // printf("%f\t%f\t%lf\n", y_tmp, z_tmp, intensity[j][k]);
                    }
                }

            n += 1;
        }
    }

    // printf("n = %d\t", n);
}

/******************************************************************************
FUNCTION : Cal_Intensity_Calibration
概要：粒子の位置に対応した輝度値を与える
 IN ：*data：使用するオブジェクト名
OUT ：
******************************************************************************/
void Cal_Intensity_Calibration(vector<vector<float>> &intensity)
{
    /* yzの位置による輝度値の計算 */
    float sigma_particle = 0.5 * magnification / 3.0 * width_px / width_mm;

    for (int i = 0; i < x.position_tank.size(); i++)
    {
        for (int j = 0; j < height_px; j++)
            for (int k = 0; k < width_px; k++)
            {
                //  輝度値の計算
                float r_tmp = sqrt((k - x.position_screen[i]) * (k - x.position_screen[i]) + (j - y.position_screen[i]) * (j - y.position_screen[i]));

                // 配列への格納
                if (sigma_particle > r_tmp)
                {
                    intensity[j][k] = 200;
                    // printf("%ld\t%ld\t%lf\n", j, k, intensity[j][k]);
                }
            }
    }
}

/******************************************************************************
FUNCTION : Create_Img_8bit
概要：受け取った2次元配列から8bit bmp画像で生成
 IN ：file_path：生成画像の名前，intensity：画像の輝度値
OUT ：void / bmp 画像の生成
******************************************************************************/
void Create_Img_8bit(string file_path, vector<vector<float>> &intensity)
{
    const int binary_size = width_px * height_px; // 画像のデータサイズ
    vector<unsigned char> binary(binary_size);    // 1次元配列の定義

    /* 1次元配列への格納 */
    int counter = 0;
    for (int i = 0; i < height_px; i++)
        for (int j = 0; j < width_px; j++)
        {
            binary[counter] = (unsigned char)intensity[i][j];
            counter += 1;
        }

    /* 画像の書き出し */
    const int header_size = 1078;
    unsigned char header_data[header_size];
    unsigned char binary_buf[binary_size]; // 1次元配列の定義（使用しない）

    // str型から const char型へ変換
    const char *header_path_c = header_path.c_str();
    const char *file_path_c = file_path.c_str();

    // vector型から 配列型へ変換
    unsigned char binary_c[binary.size()];
    copy(binary.begin(), binary.end(), binary_c);

    // ヘッダーの読み込み
    Load_Bmp_8bit(header_path_c, header_data, binary_buf);

    fp = fopen(file_path_c, "wb");

    // ヘッダー情報の書き込み
    fwrite(header_data, sizeof(unsigned char), header_size, fp);

    // 画像情報の書き込み
    fwrite(binary_c, sizeof(unsigned char), binary_size, fp);

    fclose(fp);
}

/******************************************************************************
FUNCTION : Create_Img_24bit
概要：受け取った2次元配列から8bit bmp画像で生成
 IN ：file_path：生成画像の名前，intensity：画像の輝度値
OUT ：void / bmp 画像の生成
******************************************************************************/
void Create_Img_24bit(string file_path, vector<vector<float>> &intensity_blue, vector<vector<float>> &intensity_green)
{
    const int binary_size = width_px * height_px * 3; // 画像のデータサイズ
    vector<unsigned char> binary(binary_size * 3);    // 1次元配列の定義

    /* 1次元配列への格納 */
    int counter = 0;
    for (int i = 0; i < height_px; i++)
        for (int j = 0; j < width_px; j++)
        {
            binary[counter] = (unsigned char)intensity_blue[i][j];
            counter += 1;
            binary[counter] = (unsigned char)intensity_green[i][j];
            counter += 1;
            binary[counter] = 0;
            counter += 1;
        }

    /* 画像の書き出し */
    const int header_size = 54;
    unsigned char header_data[header_size];
    unsigned char binary_buf[binary_size]; // 1次元配列の定義（使用しない）

    // str型から const char型へ変換
    const char *header_path_c = header_path_color.c_str();
    const char *file_path_c = file_path.c_str();

    // vector型から 配列型へ変換
    unsigned char binary_c[binary.size()];
    copy(binary.begin(), binary.end(), binary_c);

    // ヘッダーの読み込み
    Load_Bmp_24bit(header_path_c, header_data, binary_buf);

    fp = fopen(file_path_c, "wb");

    // ヘッダー情報の書き込み
    fwrite(header_data, sizeof(unsigned char), header_size, fp);

    // 画像情報の書き込み
    fwrite(binary_c, sizeof(unsigned char), binary_size, fp);

    fclose(fp);
}
