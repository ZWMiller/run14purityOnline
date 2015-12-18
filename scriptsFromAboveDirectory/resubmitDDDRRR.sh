#!/bin/bash

day=$1
run=$2
SubDir=Submit/$day/$run
PicoDir=PicoProd/$day/$run
LogDir=Log/$day/$run
ListDir=PicoList/$day

aaa=`ls $SubDir/*.session.xml -lrt`
echo $aaa
echo "------------"
ccc=${aaa#-*Submit/}
echo $ccc
echo "------------"
bbb=${ccc:13:32}
echo ${bbb}

#./checkoutDDDRRR.sh $day $run

for badFile in `cat $ListDir/${day}_${run}_bad.lis`
do
  tmp=`echo ${badFile:22}`
  fileid=${tmp/picoDst.root/}
  rm -f $PicoDir/$tmp
  rm -f $LogDir/$fileid*
#  echo $fileid
  tmp=${tmp/picoDst/MuDst} 
  echo $tmp
  bad=`grep -l $tmp $SubDir/sched${bbb}_*.list`
  echo "bad1 $bad"
  bad=${bad#S*_}
  echo "bad2 $bad"
  bad=${bad/.list/}
  echo "bad3 $bad"
#  echo $bad
  echo "bbb $bbb"

  cd $SubDir
  echo "re-submit crashed job for $PicoDir/${tmp/MuDst/picoDst}"
#  star-submit-1.10.3 -r $bad $bbb.session.xml
  star-submit -r $bad $bbb.session.xml
  
  echo -n
  cd ../../../
  
done
