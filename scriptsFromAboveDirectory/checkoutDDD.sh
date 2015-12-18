#!/bin/bash


day=$1
for run in `cat List/day$day.lis`
do
  ./checkoutDDDRRR.sh $day $run
done

