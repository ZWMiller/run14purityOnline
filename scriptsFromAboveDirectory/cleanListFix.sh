#!/bin/bash

#: << 'MYCOMMENT'
#####################
#####################

cat fileLists_done/picoList*.list > clean_done/pdsfList_test.list
find pwg/ -maxdepth 3 -name "*.picoDst.root" > clean_done/pwgList_test.list

for day in `cat List/AuAu200_day.lis`
do
  echo ${day}
  
  for run in `cat List/day${day}.lis`
  do
    echo ${run}


          
    for f in `ls PicoProd/${day}/${run}/*.picoDst.root --color=none`
    do
       file=$(basename $f)

       rm -f PicoList/${day}/${run}_picoDst.clean.sum       

       if [ ! -e PicoList/${day}/${run}_picoDst.clean.sum ]; then
         cp blank PicoList/${day}/${run}_picoDst.clean.sum
       fi
       
       pdsfdone=`grep -l ${file} clean_done/pdsfList_test.list`
       pwgdone=`grep -l ${file} clean_done/pwgList_test.list`
       
       if [ ${#pdsfdone} -gt 1 -a ${#pwgdone} -gt 1 ]; then
         echo ${file}>>PicoList/${day}/${run}_picoDst.clean.sum
         rm -f PicoProd/${day}/${run}/${file}
       fi
       
                
    done
  done
done
