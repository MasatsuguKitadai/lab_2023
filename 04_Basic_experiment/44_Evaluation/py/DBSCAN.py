from sklearn.cluster import DBSCAN
import numpy as np

date = input("Data Name:")
name = input("Data Set:")

dir_path = "/mnt/e/workspace_SSD/04_basic_experiment/"  # 研究室SSD
model = DBSCAN(eps=10)  # クラスタリングのモデル指定


# データの読み込み
read_file = dir_path + date + "/44_Evaluation/" + name + "/timeline/timeline.dat"
t = np.loadtxt(read_file, comments="#", usecols=0)
x = np.loadtxt(read_file, comments="#", usecols=1)
y = np.loadtxt(read_file, comments="#", usecols=2)
X = np.array([t, x, y])
XT = X.T

# 要素数の取得
row, col = XT.shape
print("Row =" + str(row))
print("Col =" + str(col))

# クラスタリングの実行
model.fit(XT)
label = model.fit_predict(XT)  # クラスタリングの実行

Y = np.array([t, x, y, label])
YT = Y.T
print("Max Label =" + str(max(label)))

# データの書き出し
write_file = dir_path + date + "/44_Evaluation/" + name + "/timeline/dbscan.dat"
np.savetxt(
    write_file,
    YT,
    fmt="%2.6f",
)
