#!/bin/bash

#: << 'MYCOMMENT'
#####################
# obtain the difference between the current MuDst List and produced Pico List
#####################
for day in `cat List/AuAu200_day.lis`
do
  echo ${day}

  for run in `cat List/day${day}.lis`
  do
    echo ${run}
    
#day=$1
#run=$2
    rm -f PicoList/${day}/${run}_picoDst.lis.sum
    cp PicoList/${day}/${run}_picoDst.lis.pdsf PicoList/${day}/${run}_picoDst.lis.sum
    
  done

done

#MYCOMMENT

