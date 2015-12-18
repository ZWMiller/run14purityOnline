#!/bin/bash


day=$1
for run in `cat List/day$day.lis`
do
  ./resubmitDDDRRR.sh $day $run
done

