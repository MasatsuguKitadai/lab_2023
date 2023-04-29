# シミュレーションプログラムの開始
g++ cpp/evaluation.cpp -o "out/evaluation.out"
./out/evaluation.out

# 空フォルダの削除
find /mnt/e/workspace_SSD/03_numerical_simulation/ -type d -empty | xargs rm -r