#!/bin/bash

#: << 'MYCOMMENT'
#####################
#####################

n=$1

while true; do

echo "+++++ start a new round of picoDst clean"

cat fileLists_done/picoList*.list > clean_done/pdsfList_$n.list
find pwg/ -maxdepth 3 -name "*.picoDst.root" > clean_done/pwgList_$n.list

for day in `cat List/AuAu200_day.lis`
do
  echo ${day}
  
  for run in `cat List/day${day}.lis`
  do
    echo ${run}
          
    for f in `ls PicoProd/${day}/${run}/*.picoDst.root --color=none`
    do
       file=$(basename $f)
       
       if [ ! -e PicoList/${day}/${run}_picoDst.clean.sum ]; then
         cp blank PicoList/${day}/${run}_picoDst.clean.sum
       fi
       
       pdsfdone=`grep -l ${file} clean_done/pdsfList_$n.list`
       pwgdone=`grep -l ${file} clean_done/pwgList_$n.list`
       
       if [ ${#pdsfdone} -gt 1 -a ${#pwgdone} -gt 1 ]; then
         echo ${file}>>PicoList/${day}/${run}_picoDst.clean.sum
         rm -f PicoProd/${day}/${run}/${file}
       fi
       
                
    done
  done
done

let "n+=1"

date
echo "... waiting for another 12 hours before trying to clean again"
sleep 12h

done

