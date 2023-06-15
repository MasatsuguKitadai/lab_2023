#!/bin/bash

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

# #############################################################

# ./out/numerical_simulation_300.out
# pid1=${!}
# sleep 1
# wait ${pid1}

# # 処理時間の計算
# TIME_B=`date +%s`

# PT=`expr ${TIME_B} - ${TIME_A}`
# H=`expr ${PT} / 3600`
# PT=`expr ${PT} % 3600`
# M=`expr ${PT} / 60`
# S=`expr ${PT} % 60`

# # Finish
# echo "Finish\t:" `date '+%y/%m/%d %H:%M:%S'`
# echo "Time\t: ${H}:${M}:${S}"

# #############################################################

# ./out/numerical_simulation_350.out
# pid1=${!}
# sleep 1
# wait ${pid1}

# # 処理時間の計算
# TIME_B=`date +%s`

# PT=`expr ${TIME_B} - ${TIME_A}`
# H=`expr ${PT} / 3600`
# PT=`expr ${PT} % 3600`
# M=`expr ${PT} / 60`
# S=`expr ${PT} % 60`

# # Finish
# echo "Finish\t:" `date '+%y/%m/%d %H:%M:%S'`
# echo "Time\t: ${H}:${M}:${S}"

#############################################################

./out/10-400.out
pid1=${!}
sleep 1
wait ${pid1}

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

#############################################################

./out/10-450.out
pid1=${!}
sleep 1
wait ${pid1}

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

#############################################################