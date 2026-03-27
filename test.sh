#!/bin/bash

# 写表头（可选但强烈推荐）
echo "round of level0,round of level1,round of level2,round of level3,butterknife cycles,eden cycles" > result.csv

for a in {3..7}
do
  for b in {1..4}
  do
    for c in {1..4}
    do
      for d in {5..8}
      do
        # 运行程序并获取输出（两个浮点数）
        out=$(taskset -c 3 ./a.out $a $b $c $d)

        # 输出为 CSV 行
        echo "$a,$b,$c,$d,$out" >> result.csv
      done
    done
  done
done