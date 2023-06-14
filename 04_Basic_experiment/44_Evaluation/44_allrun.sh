#!/bin/bash
g++ cpp/transition.cpp -o "out/transition.out"
g++ cpp/velocity_max.cpp -o "out/velocity_max.out"

# 名前の読み取り
echo -n DATA_NAME:
read name 

echo -n DATA_SET:
read set 

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

expect -c " 
set timeout -1

spawn ./out/transition.out
expect \"Data Name:\"
send \"$name\n\"
expect \"Data Set:\"
send \"$set\n\"
expect \"$\n\"

spawn ./out/velocity_max.out
expect \"Data Name:\"
send \"$name\n\"
expect \"Data Set:\"
send \"$set\n\"
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