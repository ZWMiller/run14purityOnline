#!/bin/bash

#check for argument 1
if [ -z "$1" ]
then
  echo "No argument supplied. Arg 1 = folder name"
  exit 1
fi

i=1
files=`find . -maxdepth 1 -name "*.root" -type f -mmin +60 -size +10k | wc -l`
echo "Num. Files: $files"
while [ $files -gt 0 ]
do
  mkdir $1_set$i
  find . -maxdepth 1 -name "*.root" -type f -mmin +60 -size +10k | head -1000 | xargs mv -t $1_set$i
  cd $1_set$i
  hadd puritySample_$1_$i.root *.root
  cd ..
  echo "/gpfs/mnt/gpfs01/star/pwg/zamiller/PurityProd/$1_set$i/puritySample_$1_$i.root" >> haddFiles_$1.list
  files=`find . -maxdepth 1 -name "*.root" -type f -mmin +60 -size +10k | wc -l`
  echo "$files Remain"
  i=$[i+1]
done
