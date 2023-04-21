# シミュレーションプログラムの開始
g++ cpp/evaluation.cpp -o "out/evaluation.out"
./out/evaluation.out

# 空フォルダの削除
find ../../simulation/ -type d -empty | xargs rm -r

