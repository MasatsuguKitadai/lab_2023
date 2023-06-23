#!/bin/bash
g++ cpp/evaluation.cpp -o "out/evaluation.out"

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

## シミュレーション(1) ###
for num in 6 7 8 9 10 11 12 13 14 
do
    expect -c " 
    set timeout -1

    spawn out/evaluation.out
    expect \"Case Name:\"
    send \"$num-50\n\"
    expect \"Speed of rotation:\"
    send \"$num\n\"
    expect \"$\n\"

    exit 0
    "
done

## シミュレーション(1) ###
for num in 6 7 8 9 10 11 12 13 14 
do
    expect -c " 
    set timeout -1

    spawn out/evaluation.out
    expect \"Case Name:\"
    send \"$num-300\n\"
    expect \"Speed of rotation:\"
    send \"$num\n\"
    expect \"$\n\"
    
    exit 0
    "
done

## シミュレーション(1) ###
for num in 400 450 500 
do
    expect -c " 
    set timeout -1

    spawn out/evaluation.out
    expect \"Case Name:\"
    send \"10-$num\n\"
    expect \"Speed of rotation:\"
    send \"10\n\"
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