#!/bin/bash

#: << 'MYCOMMENT'
rm -f CrashedMuDst.lis
cp blank CrashedMuDst.lis
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


    for f in `cat PicoList/${day}/${run}_MuDst.lis.sum`
    do
      file=$(basename $f)
      des=${file/MuDst/picoDst}
      if [ ! -e PicoProd/${day}/${run}/$des ]; then
        good=`grep -l $file MuDstToBeSubmitted_?.lis`
        if [ ${#good} -lt 1 ]; then
          line=$(get_file_list.pl -keys path,filename -cond filename=$file,production=P15ic,storage=NFS -delim / -distinct)
     
          echo $line >> CrashedMuDst.lis
        fi
      
      fi
      
    done
    
  done

done

#MYCOMMENT

