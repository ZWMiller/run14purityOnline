#!/bin/bash

#: << 'MYCOMMENT'
#####################
# obtain the difference between the current MuDst List and produced Pico List
#####################
rm -f diffMuDstPicoDst.lis
cp blank diffMuDstPicoDst.lis

for day in `cat List/AuAu200_day.lis`
do
  echo ${day}

  for run in `cat List/day${day}.lis`
  do
    echo ${run}
    
#day=$1
#run=$2

    for mudst in `cat PicoList/${day}/${run}_MuDst.lis.sum`
    do
      pico=${mudst/MuDst/picoDst}
      del=`grep -l $pico PicoList/${day}/${run}_picoDst.clean.sum`
      if [ ${#del} -lt 1 ]; then
        if [ ! -e PicoProd/${day}/${run}/${pico} ]; then
          echo $mudst >> diffMuDstPicoDst.lis
        fi
      fi
      
    done
    
  done

done



#MYCOMMENT

rm -f diffMuDstPicoDst.lis.new
cp blank diffMuDstPicoDst.lis.new

   for file1 in `cat diffMuDstPicoDst.lis`
   do
     storage=$(get_file_list.pl -keys storage -cond "filename=$file1,production=P15ic,storage=local" -distinct)
     events=$(get_file_list.pl -keys 'sum(events)' -cond "filename=$file1,production=P15ic,storage=local" -distinct)
     line=$(get_file_list.pl -keys path,filename -cond "filename=$file1,production=P15ic,storage=local" -delim / -distinct)
                     
     if [ ${#line} -gt 0 ]; then
        if [ "$storage" = "local" ]; then
          echo -n "root://xrdstar.rcf.bnl.gov:1095/" >> diffMuDstPicoDst.lis.new
        fi
        echo $line >> diffMuDstPicoDst.lis.new
     fi
                             
   done
   
   mv diffMuDstPicoDst.lis.new diffMuDst_ToBeSubmitted.lis
                       