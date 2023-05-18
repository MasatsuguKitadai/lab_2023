#!/bin/bash
g++ cpp/binarization.cpp -o "out/binarization.out"
g++ cpp/correlation.cpp -o "out/correlation.out"
g++ cpp/labeling.cpp -o "out/labeling.out"
g++ cpp/get_peaks.cpp -o "out/get_peaks.out"
g++ cpp/stretch_image.cpp -o "out/stretch_image.out"

# 名前の読み取り
echo -n DATA_NAME:
read name 

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

expect -c " 
set timeout -1

spawn out/binarization.out
expect \"Data Name:\"
send \"$name\n\"
expect \"$\n\"

spawn python3 py/bmp_to_png.py
expect \"Data Name:\"
send \"$name\n\"
expect \"$\n\"

spawn out/correlation.out
expect \"Data Name:\"
send \"$name\n\"
expect \"$\n\"

spawn out/labeling.out
expect \"Data Name:\"
send \"$name\n\"
expect \"$\n\"

spawn out/get_peaks.out
expect \"Data Name:\"
send \"$name\n\"
expect \"$\n\"

spawn python3 py/curve_fit_3d.py
expect \"Data Name:\"
send \"$name\n\"
expect \"$\n\"

spawn out/stretch_image.out
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