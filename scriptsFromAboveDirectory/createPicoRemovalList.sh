#!/bin/bash

#scp dongx@pdsf.nersc.gov:/eliza17/rnc/dongx/pico/Run14/monitor/PicoListPDSF.tar.gz .


while true; do

#: << 'MYCOMMENT'
#####################
# obtain the difference between the current MuDst List and produced Pico List
#####################

echo "==== Start to update PicoListToRemove lists ===="
for owner in `ls PicoListToRemove_*.lis`
do
  num0=$(cat $owner | wc -l)
  echo " before update $owner has number of files $num0"
done

for day in `cat List/AuAu200_day.lis`
do
#  echo ${day}

  for run in `cat List/day${day}.lis`
  do
#    echo ${run}
    
#day=$1
#run=$2

    for f in `cat PicoList/${day}/${run}_picoDst.lis.pdsf`
    do
      owner=$(ls PicoProd/${day}/${run}/$f -l | awk '{print $3}')

      if [ ! -e PicoListToRemove_${owner}.lis ]; then
        cp blank PicoListToRemove_${owner}.lis
      fi

      removed=`` 
      if [ -e PicoListRemoved_${owner}.lis ]; then
        removed=`grep -l $f PicoListRemoved_${owner}.lis`
      fi

      if [ ${#removed} -lt 1 ]; then
        good=`grep -l $f PicoListToRemove_${owner}.lis`
        if [ ${#good} -lt 1 ]; then
          echo PicoProd/${day}/${run}/$f >> PicoListToRemove_${owner}.lis
        fi
      fi
    done

  done

done

#MYCOMMENT
for owner in `ls PicoListToRemove_*.lis`
do
  num1=$(cat $owner | wc -l)
  echo " after update $owner has number of files $num1"
done

echo "==== Finish updating PicoListToRemove lists ===="

date
echo "... waiting for another 1 hour before trying to continue again"
sleep 1h

done
