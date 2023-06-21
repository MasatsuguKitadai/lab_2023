echo "Start\t:" `date '+%y/%m/%d %H:%M:%S'`
TIME_A=`date +%s`   

## シミュレーション(1) ###
for num in 50 150 250 350
do
    # 粒子数の生成
    expect -c " 
    set timeout -1
    spawn out/numerical_simulation.out
    expect \"Rotational speed :\"
    send \"10\n\"
    expect \"Particle number :\"
    send \"$num\n\"
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
done

# 空フォルダの削除
find /mnt/e/workspace_SSD/03_numerical_simulation/ -type d -empty | xargs rm -r20