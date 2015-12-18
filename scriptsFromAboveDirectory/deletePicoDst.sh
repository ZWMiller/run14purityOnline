#!/bin/bash

owner=$(whoami)
echo $owner

while true; do

ndel=0
if [ -e PicoListToRemove_${owner}.lis ]; then
  ndel=$(cat PicoListToRemove_${owner}.lis | wc -l)
fi

for file in `cat PicoListToRemove_${owner}.lis`
do
  rm -f $file
  
  if [ ! -e PicoListRemoved_${owner}.lis ]; then
    cp blank PicoListRemoved_${owner}.lis
  fi
  
  echo $file>>PicoListRemoved_${owner}.lis  

done

rm -f PicoListToRemove_${owner}.lis
echo " $ndel picoDst files removed in this round "

date
echo "... waiting for another 1 hour before trying to continue again"
sleep 1h

done
