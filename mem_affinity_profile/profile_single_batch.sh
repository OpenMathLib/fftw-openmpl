#!/bin/bash

cd .. 

make --quiet 2>&1 > /dev/null 

cd ./mem_affinity_profile

./compiler_norm.sh

cd ../threads

sed -i '1i#define MEM_AFFINITY' dft-vrank-geq1.c

cd ..

make --quiet > /dev/null 2>&1

cd ./mem_affinity_profile

./compiler_mem_affinity.sh

sed -i '1d' ../threads/dft-vrank-geq1.c

arg1_list=()
arg2_list=()

for (( j=100; j<1000; j+=100 )); do
    arg2_list+=($j)
done
for (( j=1000; j<10000; j+=1000 )); do
    arg2_list+=($j)
done
for (( j=10000; j<100000; j+=10000 )); do
    arg2_list+=($j)
done
for (( j=100000; j<=1000000; j+=50000 )); do
    arg2_list+=($j)
done

for arg2 in "${arg2_list[@]}"; do
    ./norm  "$arg2" "1"
    ./mem_affinity "$arg2" "1"
done

rm ./tmp.txt


