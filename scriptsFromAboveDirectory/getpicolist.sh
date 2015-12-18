#!/bin/bash

day=$1
run=$2
SubDir=Submit/$day/$run
PicoDir=PicoProd/$day/$run
LogDir=Log/$day/$run
ListDir=PicoList/$day

# Original
aaa=`ls $SubDir/*.session.xml -lrt`
echo $aaa
echo "------------"
ccc=${aaa#-*Submit/}
echo $ccc
echo "------------"
bbb=${ccc:13:32}
echo ${bbb}

#Original
#input=$SubDir/sched$bbb.list

input=List/PicoDst/Remain/mudst.run${run}.lis

index=0

if [ ! -e $input ] ; then
  input=$SubDir/sched$bbb.list
fi

for FILE in `cat $input`
do
#----------------------------------------------------------------------------------------------------
# Comment out this line for Run11 2nd production (Hiroshi)
#   if [ $[index%2] -eq 0 ]; then
#----------------------------------------------------------------------------------------------------

#     echo $FILE
     tmp=`echo ${FILE:(-18)}`
#     echo "FILE $FILE"
#     echo "tmp $tmp"
#     prename=`echo ${FILE:(-46)}`            
     prename=`basename $FILE`
#     echo "prename $prename"
     prename=`echo $prename | sed 's:MuDst:picoDst:g'`
#     prename=${prename:0:14}
#    echo $prename
#    if [ "$prename" == "st_physics_adc" ]; then
#       echo $PicoDir/st_physics_adc_${run}_raw_${tmp/MuDst/picoDst} >>$ListDir/${day}_$run.lis-origin
#    else
#       echo $PicoDir/st_physics_${run}_raw_${tmp/MuDst/picoDst} >>$ListDir/${day}_$run.lis-origin
#    fi
#     echo $PicoDir/$prename
     echo $PicoDir/$prename >>$ListDir/${day}_$run.lis-origin

#----------------------------------------------------------------------------------------------------
# Comment out this line for Run11 2nd production (Hiroshi)
#   fi
#----------------------------------------------------------------------------------------------------

   let "index+=1";
done

