#!/bin/bash

# 動作テスト

command1() {
  sleep 1
  echo '動作テスト (1)'
  date
} 
command2() {
  sleep 2
  echo '動作テスト (2)'
  date
} 
command3() {
  sleep 3
  echo '動作テスト (3)'
  date
} 

command1 &
pid1=${!}
command2 &
pid2=${!}
command3 &
pid3=${!}

wait ${pid1} ${pid2} ${pid3}

echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

#############################################################

# ## プロセス(1)
# ./out/10-50.out &
# pid1=${!}
# sleep 1
# ./out/10-100.out &
# pid2=${!}
# sleep 1
# ./out/10-150.out &
# pid3=${!}
# sleep 1
# wait ${pid1} ${pid2} ${pid3}

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

## プロセス(2)
./out/10-200.out &
pid1=${!}
sleep 1
./out/10-250.out &
pid2=${!}
sleep 1
./out/10-300.out &
pid3=${!}
sleep 1
wait ${pid1} ${pid2} ${pid3}

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

## プロセス(3)
./out/10-400.out &
pid1=${!}
sleep 1
./out/10-450.out &
pid2=${!}
sleep 1
wait ${pid1} ${pid2}

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

## プロセス(4)
./out/10-350.out &
pid1=${!}
sleep 1
./out/10-500.out &
pid2=${!}
sleep 1
wait ${pid1} ${pid2} 

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

## プロセス(5)
./out/6-100.out &
pid1=${!}
sleep 1
./out/8-100.out &
pid2=${!}
sleep 1
wait ${pid1} ${pid2} 

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

## プロセス(6)
./out/12-100.out &
pid1=${!}
sleep 1
./out/14-100.out &
pid2=${!}
sleep 1
wait ${pid1} ${pid2} 

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