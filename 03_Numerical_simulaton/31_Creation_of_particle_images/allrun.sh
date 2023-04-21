#!/bin/bash

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

# シミュレーションプログラムの開始
g++ cpp/numerical_simulation.cpp -o "out/numerical_simulation.out"

# for name in Data_set_11 Data_set_12 Data_set_13 Data_set_14 Data_set_15 Data_set_16 Data_set_17 Data_set_18 Data_set_19 Data_set_20
for name in Data_set_11 Data_set_12 Data_set_13 Data_set_14 Data_set_15
do
    expect -c " 
    set timeout -1

    spawn ./out/numerical_simulation.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    exit 0
    "
done

# 空フォルダの削除
find /mnt/d/workspace_SSD/01_numerical_simulation/ -type d -empty | xargs rm -r

# 処理時間の計算
TIME_B=`date +%s`

PT=`expr ${TIME_B} - ${TIME_A}`
H=`expr ${PT} / 3600`
PT=`expr ${PT} % 3600`
M=`expr ${PT} / 60`
S=`expr ${PT} % 60`

# Finish
echo "Finish\t:" `date '+%y/%m/%d %H:%M:%S'`
echo "Time\t: ${H}:${M}:${S}"
