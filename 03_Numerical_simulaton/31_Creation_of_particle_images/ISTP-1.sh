echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

## シミュレーション(1) ###
for name in 6 7 8 9 11 12 13 14 15
do
    expect -c " 
    set timeout -1

    spawn out/$name-450.out
    expect \"Case Name:\"
    send \"$name-450\n\"
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

# 空フォルダの削除
find /mnt/e/workspace_SSD/03_numerical_simulation/ -type d -empty | xargs rm -r20