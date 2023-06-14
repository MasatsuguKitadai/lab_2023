#!/bin/bash
g++ cpp/velocity.cpp -o "out/velocity.out"

# 名前の読み取り
echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A="date +%s"

# 名前の読み取り
echo -n DATA_NAME:
read data 

echo -n DATA_SET:
read set 

for delta in 8 9 10 11 12
do
    expect -c " 
    set timeout -1
    spawn ./out/velocity.out
    expect \"Data Name:\"
    send \"$data\n\"
    expect \"Data Set:\"
    send \"${set}_n=$delta\n\"
    expect \"$\n\"
    "
done

# Finish
echo "Finish\t:" `date '+%y/%m/%d %H:%M:%S'`
echo "Time\t: ${H}:${M}:${S}"