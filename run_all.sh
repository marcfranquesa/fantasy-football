#!/bin/bash

benchs="new_benchs"
output=$benchs"_results"_$1

rm $output/* || mkdir $output

make

for file in $(ls $benchs); do
    #if [[ $file == "easy"* || $file == "med"* ]]; then
        ./$1.out data_base.txt $benchs/$file $output/$file
        ./checker.out data_base.txt $benchs/$file $output/$file
    #fi
done