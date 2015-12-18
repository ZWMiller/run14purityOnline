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

#    if [ ! -e PicoList/${day}/${run}_MuDst.lis.sum ]; then
#      cp blank PicoList/${day}/${run}_MuDst.lis.sum
#    fi
    rm -f PicoList/${day}/${run}_MuDst.lis.sum
    cat PicoList/${day}/${run}_picoDst.clean.sum | sed 's/picoDst/MuDst/' > PicoList/${day}/${run}_MuDst.lis.sum

    for f in `ls PicoProd/${day}/${run}/*.picoDst.root --color=none`
    do
      if [ -s $f ]; then
        file=$(basename $f)
        des=${file/picoDst/MuDst}
        good=`grep -l $des PicoList/${day}/${run}_MuDst.lis.sum`
        if [ ${#good} -lt 1 ]; then
         echo $des >> PicoList/${day}/${run}_MuDst.lis.sum
        fi
      fi      
    done
    
  done

done

#MYCOMMENT

