#!/bin/bash
g++ cpp/velocity.cpp -o "out/velocity.out"

# 名前の読み取り
echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A="date +%s"

data="20230517"   

for delta in 8 9 10 11 12 13 14 15 16 17
do
    expect -c " 
    set timeout -1
    spawn ./out/velocity.out
    expect \"Data Name:\"
    send \"$data\n\"
    expect \"Data Set:\"
    send \"delta_001_n=$delta\n\"
    expect \"$\n\"
    "
done


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