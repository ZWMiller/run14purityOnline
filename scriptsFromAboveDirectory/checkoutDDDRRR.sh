#!/bin/bash

day=$1
run=$2

dtstamp=`date +"%H-%M-%d-%m-%Y"`
echo $dtstamp

ListDir=PicoList/$day

rm -f $ListDir/${day}_$run.lis

if [ -e "$ListDir/${day}_$run.lis-origin" ]; then
   echo "$ListDir/${day}_$run.lis-origin already exists !"
else
   ./getpicolist.sh $day $run
fi

if [ -e "$ListDir/${day}_${run}_bad.lis" ]; then
   if [ -s "$ListDir/${day}_${run}_bad.lis" ]; then
      echo "Updating ${day}_${run}_bad.lis"        
      mv $ListDir/${day}_${run}_bad.lis $ListDir/${day}_${run}_bad.lis-$dtstamp
      mv $ListDir/${day}_${run}_good.lis $ListDir/${day}_${run}_good.lis-$dtstamp
      ln -s ${day}_${run}_bad.lis-$dtstamp $ListDir/${day}_${run}.lis
   else
      echo "${day}_${run}_bad.lis is Empty! No need to update!"
      exit
   fi
else 
   ln -s ${day}_$run.lis-origin $ListDir/${day}_$run.lis
fi

root -b -q -x 'checkfileDDDRRR.C("'$day'",'$run')'

