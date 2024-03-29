#!/bin/bash

g++ cpp/labeling_for_blue.cpp -o "out/labeling_for_blue.out"
g++ cpp/PTV.cpp -o "out/PTV.out"
# g++ cpp/PTV_new.cpp -o "out/PTV.out"
g++ cpp/velocity.cpp -o "out/velocity.out"
g++ cpp/vorticity.cpp -o "out/vorticity.out"

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

# 名前の読み取り
echo -n INPUT_DATA_NAME:
read name 

# DIR_1="/mnt/e/workspace_SSD/03_numerical_simulation/$name/LLS_1/particle_image_png" 
# if [ ! -d $DIR_1 ];then
    expect -c " 
    set timeout -1
    # spawn python3.8 py/bmp_to_png.py
    spawn python3 py/bmp_to_png.py
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"
    exit 0
    "
# fi

# DIR_2="/mnt/e/workspace_SSD/03_numerical_simulation/$name/LLS_1/labeling_position_dat" 
# if [ ! -d $DIR_2 ];then
    expect -c " 
    set timeout -1
    spawn ./out/labeling_for_blue.out
    expect \"Case Name:\"
    send \"$name\n\"
    expect \"$\n\"
    exit 0
    "
# fi

## シミュレーション ###
expect -c " 
set timeout -1

spawn ./out/PTV.out
expect \"Case Name:\"
send \"$name\n\"
expect \"$\n\"

spawn ./out/velocity.out
expect \"Case Name:\"
send \"$name\n\"
expect \"$\n\"

spawn ./out/vorticity.out
expect \"Case Name:\"
send \"$name\n\"
expect \"$\n\"

exit 0
"

# 空フォルダの削除
find /mnt/e/workspace_SSD/03_numerical_simulation/ -type d -empty | xargs rm -r

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