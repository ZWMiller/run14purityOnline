#!/bin/bash

#: << 'MYCOMMENT'
#####################
#####################


n=0

    for f in `cat filelist_0508.lis`
    do

      if [ -e pwg/$f ]; then 
#        echo PicoProd/$f
        rm -f PicoProd/$f
      fi       
      
      let "n+=1"
      
      m=`expr $n % 1000`
      
      if [ $m -eq 0 ]; then
        echo "processing " $n "-th file"
      fi          
    done

