#!/bin/bash

#: << 'MYCOMMENT'
#####################
#####################

pwgdir=/gpfs01/star/pwg_tasks/hf02/picodsts/Run14/AuAu/200GeV/physics2/P15ic

while true; do

echo "+++++ start a new round of picoDst copy to PWG area"

START=$(date +%s)

for day in `cat List/AuAu200_day.lis`
do
  echo ${day}

  for run in `cat List/day${day}.lis`
  do
    echo ${run}
    
#day=$1
#run=$2

    for f in `ls PicoProd/${day}/${run}/*.picoDst.root --color=none`
    do
      file=$(basename $f)
      
      if [ ! -e ${pwgdir}/${day} ]; then
        mkdir ${pwgdir}/${day}
      fi
      if [ ! -e ${pwgdir}/${day}/${run} ]; then
        mkdir ${pwgdir}/${day}/${run}
      fi
      
      if [ ! -e ${pwgdir}/${day}/${run}/${file} ]; then
        echo "copying file " $file " ..."
        cp PicoProd/${day}/${run}/${file} ${pwgdir}/${day}/${run}
      fi
      
    done

#MYCOMMENT
    
  done

done

END=$(date +%s)
DIFF=$((6 - (END - START ) / 3600))

if [ $DIFF -lt 0 ]; then
  DIFF=0
fi
  
date
echo "... waiting for another " $DIFF " hours before trying to copy again"
sleep ${DIFF}h

done

