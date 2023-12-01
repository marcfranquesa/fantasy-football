#!/bin/bash

benchs="new_benchs"
output="public_benchs_results"

rm $output/*

for file in $(ls $benchs); do
    if [[ $file == "easy"* || $file == "med"* ]]; then
        ./$1.out data_base.txt public_benchs/$file $output/$file
    fi
done