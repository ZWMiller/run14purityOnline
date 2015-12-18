#!/bin/bash

#START=$(date +%s)


#sleep 10s

#END=$(date +%s)
START=0
END=36000
echo $START
echo $END
DIFF=$((20 - (END - START ) / 3600))

if [ $DIFF -lt 0 ]; then
  DIFF=0
fi

#DIFF=`expr $DIFF \* 3600`
echo $DIFF

sleep ${DIFF}s
