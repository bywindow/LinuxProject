#!/bin/bash

for (( n=1; n<=$1; n++))
do
  for ((m=1; m<=$2; m++))
  do
    let num=$n*$m
    echo "$n*$m=$num "
  done
  echo " "
done


