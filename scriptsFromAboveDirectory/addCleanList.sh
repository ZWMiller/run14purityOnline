#!/bin/bash

input=$1

n=0

for f in `cat $input`
do
  day=${f:0:3}
#  echo $day
 
  run=${f:4:8}
#  echo $run
  
  filename=$(basename $f)
#  echo $filename

  if [ ! -e PicoList/${day}/${run}_picoDst.clean.sum ]; then
    cp blank PicoList/${day}/${run}_picoDst.clean.sum
  fi
  
  good=`grep -l $filename PicoList/${day}/${run}_picoDst.clean.sum`
  
  if [ ${#good} -lt 1 ]; then
    if [ -e pwg/${day}/${run}/${filename} ]; then
#      echo "This file has been deleted, but not recorded"
      echo ${filename}>>PicoList/${day}/${run}_picoDst.clean.sum
    fi
  fi

      let "n+=1"
      
      m=`expr $n % 1000`
            
      if [ $m -eq 0 ]; then
         echo "processing " $n "-th file"
      fi
                                
done
