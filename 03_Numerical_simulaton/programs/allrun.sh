# シミュレーションプログラムの開始
g++ cpp/numerical_simulation.cpp -o "out/numerical_simulation.out"
./out/numerical_simulation.out

# 空フォルダの削除
find ../../simulation/ -type d -empty | xargs rm -r

