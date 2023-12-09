#!/bin/bash

benchs="new_benchs"
output=$benchs"_results"

rm $output/* || mkdir $output

for file in $(ls $benchs); do
    #if [[ $file == "easy"* || $file == "med"* ]]; then
        ./$1.out data_base.txt $benchs/$file $output/$file
    #fi
done