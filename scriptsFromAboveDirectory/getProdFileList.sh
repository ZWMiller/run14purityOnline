#!/bin/bash

while true; do

echo "++++++ Start a new round of MuDst querying ++++++"
#: << 'MYCOMMENT'
#####################
# obtain the difference between the current MuDst List and produced Pico List
#####################

nsum=0
for day in `cat List/AuAu200_day.lis`
do
#  echo ${day}

  for run in `cat List/day${day}.lis`
  do
#    echo ${run}
#day=$1
#run=$2
    
    rm -f PicoList/${day}/${run}_MuDst.lis
        
#    rm -f PicoList/${day}/${run}_MuDst.lis
#    mv PicoList/${day}/${run}_picoDst.lis PicoList/${day}/${run}_picoDstOld.lis
    
    get_file_list.pl -keys filename -cond "runnumber=${run},filename~st_physics,production=P15ic,filetype=daq_reco_MuDst,storage=local" -limit 0 -distinct | sort > PicoList/${day}/${run}_MuDst.lis

########
# need to be adjusted
########
#    ls PicoProd/${day}/${run}/*.picoDst.root --color=none | sort > PicoList/${day}/${run}_picoDstNew.lis
#    cat PicoList/${day}/${run}_picoDstNew.lis PicoList/${day}/${run}_picoDstOld.lis | sort > PicoList/${day}/${run}_picoDst.lis
#    rm -f PicoList/${day}/${run}_picoDstNew.lis
#    rm -f PicoList/${day}/${run}_picoDstOld.lis


    if [ -e PicoList/${day}/${run}_MuDst.lis.sum ]; then

      cp blank PicoList/${day}/${run}_MuDstToBeSubmitted.lis
      for file in `cat PicoList/${day}/${run}_MuDst.lis`
      do
#        filebase=${file%.MuDst.root}
#        echo ${filebase}
        good=`grep -l ${file} PicoList/${day}/${run}_MuDst.lis.sum`
        if [ ${#good} -lt 1 ]; then
          echo ${file}>>PicoList/${day}/${run}_MuDstToBeSubmitted.lis
        fi
      
      done
    else
      cp PicoList/${day}/${run}_MuDst.lis PicoList/${day}/${run}_MuDstToBeSubmitted.lis
    fi
    
    mv -f PicoList/${day}/${run}_MuDst.lis PicoList/${day}/${run}_MuDst.lis.sum
    
    n1=$(cat PicoList/${day}/${run}_MuDstToBeSubmitted.lis | wc -l)
    
    nsum=`expr $nsum + $n1`
    
	if [ "$nsum" -ge 6000 ]; then
	  break 2
	fi

  done
  
done

#MYCOMMENT

cat PicoList/*/*_MuDstToBeSubmitted.lis > MuDstToBeSubmitted.lis
rm -f PicoList/*/*_MuDstToBeSubmitted.lis

########################
# Split into 5 files
########################
nl=`cat MuDstToBeSubmitted.lis | wc -l`
nls=`expr $nl / 2 + 1`
echo $nls
split -l $nls -d --suffix-length=1 MuDstToBeSubmitted.lis MuDstToBeSubmitted_
rm -f MuDstToBeSubmitted.lis

#######################
# Merge with the existing files which has not been submitted
#######################
for i in `seq 0 1`;
do
   echo "Producing submission file list $i ..."
   
   cp blank MuDstToBeSubmitted_$i.lis.new
   
   for file1 in `cat MuDstToBeSubmitted_$i`
   do
     storage=$(get_file_list.pl -keys storage -cond "filename=$file1,production=P15ic,storage=local" -distinct)
     events=$(get_file_list.pl -keys 'sum(events)' -cond "filename=$file1,production=P15ic,storage=local" -distinct)
     line=$(get_file_list.pl -keys path,filename -cond "filename=$file1,production=P15ic,storage=local" -delim / -distinct)

     if [ ${#line} -gt 0 ]; then     
       if [ "$storage" = "local" ]; then
         echo -n "root://xrdstar.rcf.bnl.gov:1095/" >> MuDstToBeSubmitted_$i.lis.new
       fi
       echo $line >> MuDstToBeSubmitted_$i.lis.new
     fi
     
   done

   nadd=$(cat MuDstToBeSubmitted_$i.lis.new | wc -l)
   echo "appending " $nadd " files to file #" $i
   cat MuDstToBeSubmitted_$i.lis.new >> MuDstToBeSubmitted_$i.lis
   nnew=$(cat MuDstToBeSubmitted_$i.lis | wc -l)      
   echo "    ... now file #" $i " has number of files = " $nnew
   
   chmod g+rw MuDstToBeSubmitted_$i.lis
   
   rm -f MuDstToBeSubmitted_$i.lis.new
   rm -f MuDstToBeSubmitted_$i
done

date
echo "... waiting for another 9 hour before trying to query again"
sleep 9h

done
