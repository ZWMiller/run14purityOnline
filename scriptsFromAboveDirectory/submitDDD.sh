#!/bin/bash

day=$1
for RUN in `cat List/day${1}.lis`
do
     echo $RUN
     cd Submit/$day/$RUN
     ln -s ../../../AuAu200.xml
     star-submit-template -template AuAu200.xml -entities day=$day,runnumber=$RUN
     cd ../../..
done
