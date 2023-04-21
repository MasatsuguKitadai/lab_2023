#!/bin/bash

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

# make_dir
g++ 03_PTV/cpp/make_dir.cpp -o "03_PTV/out/make_dir.out"
./03_PTV/out/make_dir.out

python3 03_PTV/py/bmp_to_png.py

g++ 03_PTV/cpp/labeling_for_ptv_blue.cpp -o "03_PTV/out/labeling_for_ptv_blue.out"
./03_PTV/out/labeling_for_ptv_blue.out

# ptv
g++ 03_PTV/cpp/PTV.cpp -o "03_PTV/out/PTV.out"
./03_PTV/out/PTV.out

g++ 03_PTV/cpp/velocity.cpp -o "03_PTV/out/velocity.out" -lm
./03_PTV/out/velocity.out

g++ 03_PTV/cpp/vorticity.cpp -o "03_PTV/out/vorticity.out" -lm
./03_PTV/out/vorticity.out

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