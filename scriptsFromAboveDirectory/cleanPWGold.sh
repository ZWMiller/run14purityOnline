#!/bin/bash

#: << 'MYCOMMENT'
#####################
#####################

pwg_new=/gpfs01/star/pwg_tasks/hf02/picodsts/Run14/AuAu/200GeV/physics2/P15ic
pwg_old=/gpfs01/star/pwg_tasks/hf02/picodsts/Run14/AuAu/200GeV/physics/P15ic

while true; do

echo "+++++ start a new round of picoDst clean in PWG area"

for day in `cat List/AuAu200_day.lis`
do
  echo ${day}
  
  for run in `cat List/day${day}.lis`
  do
    echo ${run}
          
    for f in `ls ${pwg_old}/${day}/${run}/*.picoDst.root --color=none`
    do
      file=$(basename $f)
      if [ -e ${pwg_new}/${day}/${run}/${file} ]; then
#         echo $f "new file is created, delete the old one"
        rm -f ${pwg_old}/${day}/${run}/${file}
      fi

    done
  done
done

date
echo "... waiting for another 6 hours before trying to clean again"
sleep 6h

done

