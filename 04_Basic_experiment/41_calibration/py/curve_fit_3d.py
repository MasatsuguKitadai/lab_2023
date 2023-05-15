from math import sqrt
from scipy.optimize import curve_fit
import matplotlib.pyplot as plt
from mpl_toolkits.mplot3d import Axes3D
import numpy as np
import os

## 設定ファイルのインポート ##
name = "230512_delta"
dirpath = "/mnt/e/workspace_SSD/04_basic_experiment/"
file_path = str(dirpath) + str(name)

## フォルダパス設定 ##
file_path = str(dirpath) + str(name)
dir = file_path + "/41_calibration"

if not os.path.exists(dir + '/curve_fit'):
    os.makedirs(dir + '/curve_fit')

## 2次元3次曲面 ##


def func(X, a, b, c, d, e, f, g, h, i, j):
    x, y, = X
    z = a * x*x*x + b * y*y*y + c*x*x*y + d*y*y * \
        x + e*x*x + f*y*y + g*x*y + h*x + i*y + j
    return z.ravel()


for num in range(3):

    distance = 2.5 * num

    ## データの読み込み ##
    data_file_1 = str(dir) + '/get_peaks/peak_positions_' + \
        str('{:1}'.format(distance)) + 'mm.dat'
    print(str(data_file_1))

    x_data = np.loadtxt(data_file_1, comments='#', usecols=0)
    y_data = np.loadtxt(data_file_1, comments='#', usecols=1)
    x_px = np.loadtxt(data_file_1, comments='#', usecols=2)
    y_px = np.loadtxt(data_file_1, comments='#', usecols=3)

    delta_x = x_data - x_px  # 変換後の平面(x座標)と変換前の平面(x'座標)の差 → 最終的には差を0にしなければならない
    delta_y = y_data - y_px

    X = np.array([x_px, y_px])
    popt_1, pcov_1 = curve_fit(func, X, x_data)
    popt_2, pcov_2 = curve_fit(func, X, y_data)

    np_write = np.hstack((popt_1.reshape(10, 1), popt_2.reshape(10, 1)))
    # np_write = np.hstack((popt_1.reshape(1, 10), popt_2.reshape(1, 10)))

    print(np_write)

    np.savetxt(dir + '/curve_fit/curve_fit_' + str('{:1}'.format(distance)) + 'mm.dat',
               np_write.reshape(10, 2), fmt='%3.12f')
    #    np_write.reshape(2, 20), fmt='%3.12f')
