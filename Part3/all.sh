make

std_time=0 my_time=0 my_rate=0
# 进行100次测试
for ((i=1;i<=100;i++))
do
    echo "run No.$i"
    ./data > in.txt
    ./std_malloc_test < in.txt > std.txt 
    ./my_malloc_test < in.txt > my.txt   
    # 保存std_malloc的运行时间 
    while read -r num1 ; do
        std_time=$(echo "$std_time+$num1" | bc -l)
    done < std.txt
    # 保存my_malloc运行时间 
    while read -r num1 num2; do
        my_time=$(echo "$my_time+$num1" | bc -l)
        my_rate=$(echo "$my_rate+$num2" | bc -l)
    done < my.txt
done
echo "清理文件"
make clean
# 输出时间
echo "$my_time $std_time"
# 将stdtime作为基准
my_socre=$(echo "$std_time / $my_time * 100" | bc -l)
# 内存利用率 /100 次求均值 再*100作为分数 相当于不变
#my_rate=$(echo "$my_rate   " | bc -l)
echo "$my_socre $my_rate"

