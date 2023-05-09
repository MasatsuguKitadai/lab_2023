#!/bin/bash

g++ cpp/labeling_for_blue.cpp -o "out/labeling_for_blue.out"
g++ cpp/PTV.cpp -o "out/PTV_case2.out"

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

## シミュレーション(1) ###
for name in Data_set_16 Data_set_17 Data_set_18 Data_set_19 Data_set_20
do
    expect -c " 
    set timeout -1

    spawn python3 py/bmp_to_png.py
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    spawn out/labeling_for_blue.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    spawn out/PTV_case2.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    spawn out/velocity.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    exit 0
    "
done

## シミュレーション(2) ###
for name in  Data_set_21 Data_set_22 Data_set_23 Data_set_24 Data_set_25
do
    expect -c " 
    set timeout -1

    # spawn python3 py/bmp_to_png.py
    # expect \"Case Name:\"
    # send \"$name\n\"
    # expect \"$\n\"

    spawn out/labeling_for_blue.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    spawn out/PTV.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    spawn out/velocity.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    exit 0
    "
done

## シミュレーション(3) ###
for name in Data_set_11 Data_set_12 Data_set_13 Data_set_14 Data_set_15
do
    expect -c " 
    set timeout -1

    # spawn python3 py/bmp_to_png.py
    # expect \"Case Name:\"
    # send \"$name\n\"
    # expect \"$\n\"

    spawn out/labeling_for_blue.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    spawn out/PTV.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"

    spawn out/velocity.out
    expect \"Case Name:\"
    send \"$name\n\"
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