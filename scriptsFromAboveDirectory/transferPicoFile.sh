#!/bin/bash

trg="preview2"
prod="P15ic"

while true; do

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
    if [ ! -e PicoList/${day}/${run}_picoDst.lis.sum ]; then
      cp blank PicoList/${day}/${run}_picoDst.lis.sum
    fi
    rm -f ${day}/${run}_picoDstToBeTransferred.lis
    cp blank PicoList/${day}/${run}_picoDstToBeTransferred.lis

    nsum0=$(cat PicoList/${day}/${run}_picoDst.lis.sum | wc -l)
    for file in `ls PicoProd/${day}/${run}/*.picoDst.root`
    do
      f=$(basename $file)
      good=`grep -l ${f} PicoList/${day}/${run}_picoDst.lis.sum`
      if [ ${#good} -lt 1 ]; then
        echo ${f}>>PicoList/${day}/${run}_picoDstToBeTransferred.lis
      fi
    
    done
    
    ntran=$(cat PicoList/${day}/${run}_picoDstToBeTransferred.lis | wc -l)
    echo "Transferring $ntran picoDst files from this run ..."
    for f in `cat PicoList/${day}/${run}_picoDstToBeTransferred.lis`
    do
      echo $f
      globus-url-copy -vb -cd -p 10 file:///star/institutions/lbl_prod/Run14_AuAu200GeV/${trg}/PicoProd/${day}/${run}/$f gsiftp://dtn01.nersc.gov/project/projectdirs/starprod/picodsts/Run14/AuAu/200GeV/${trg}/${prod}/$day/$run/
      
    done

    cat PicoList/${day}/${run}_picoDstToBeTransferred.lis >> PicoList/${day}/${run}_picoDst.lis.sum    
    nsum1=$(cat PicoList/${day}/${run}_picoDst.lis.sum | wc -l)
    echo "picoDst list - number of files increasing from $nsum0 to $nsum1"
    
    rm -f PicoList/${day}/${run}_picoDstToBeTransferred.lis
    
    
  done

done

#MYCOMMENT

date
echo "... waiting for another 1 hour before trying to query again"
sleep 1h

done
