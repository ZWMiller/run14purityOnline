#!/bin/bash

#: << 'MYCOMMENT'
#####################
#####################

pwgdir=/gpfs01/star/pwg_tasks/hf02/picodsts/Run14/AuAu/200GeV/physics2/P15ic



#for day in `cat List/AuAu200_day.lis`
#do
day=$1

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

#done

