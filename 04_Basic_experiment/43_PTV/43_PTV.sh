#!/bin/bash
g++ cpp/PTV_n.cpp -o "out/PTV_n.out"
# g++ cpp/velocity.cpp -o "out/velocity.out"

# 名前の読み取り
echo -n DATA_NAME:
read name 

echo -n DATA_SET:
read set 

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

for delta in 8 9 10 11 12
do
    echo "${set}_n=${delta}"
    
    expect -c " 
    set timeout -1
    spawn ./out/PTV_n.out
    expect \"Data Set:\"
    send \"$set\n\"
    expect \"Delta:\"
    send \"$delta\n\"
    expect \"$\n\"

    spawn ./out/velocity.out
    expect \"Data Name:\"
    send \"$name\n\"
    expect \"Data Set:\"
    send \"${set}_n=${delta}\n\"
    expect \"$\n\"

    exit 0
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