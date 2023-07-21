#!/bin/bash
g++ cpp/background.cpp -o "out/background.out"
g++ cpp/background_sub.cpp -o "out/background_sub.out"
g++ cpp/filter.cpp -o "out/filter.out"
g++ cpp/separation.cpp -o "out/separation.out"
g++ cpp/stretch_images.cpp -o "out/stretch_images.out"

# 名前の読み取り
echo -n DATA_NAME:
read name 

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

expect -c " 
set timeout -1

spawn ./out/background_sub.out
expect \"Data Name:\"
send \"$name\n\"
expect \"$\n\"

spawn ./out/filter.out
expect \"Data Name:\"
send \"$name\n\"
expect \"$\n\"
exit 0
"

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