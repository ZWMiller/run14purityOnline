#!/bin/bash

if [ ! -d List ]; then
   mkdir List
fi
if [ ! -d PicoList ]; then
   mkdir PicoList
fi
if [ ! -d CheckLog ]; then
   mkdir CheckLog
fi
if [ ! -d Log ]; then
   mkdir Log
fi
if [ ! -d PicoProd ]; then
   mkdir PicoProd
fi
         
if [ ! -d CheckLog ]; then
   mkdir CheckLog
fi

if [ ! -d csh ]; then
   mkdir csh
fi

if [ ! -d err ]; then
   mkdir err 
fi

if [ ! -d report ]; then
   mkdir report 
fi

for DDD in `cat List/AuAu200_day.lis`
do
   echo $DDD

   if [ ! -d CheckLog/$DDD ]; then
     mkdir CheckLog/$DDD
   fi
   if [ ! -d Log/$DDD ]; then
     mkdir Log/$DDD
   fi
   if [ ! -d PicoProd/$DDD ]; then
     mkdir PicoProd/$DDD
   fi
   if [ ! -d PicoList/$DDD ]; then
     mkdir PicoList/$DDD
   fi      

   for RUN in `cat List/day${DDD}.lis`
   do
     echo $RUN
     
     if [ ! -d CheckLog/$DDD/$RUN ]; then
       mkdir CheckLog/$DDD/$RUN
     fi
     if [ ! -d Log/$DDD/$RUN ]; then
       mkdir Log/$DDD/$RUN 
     fi
     if [ ! -d PicoProd/$DDD/$RUN ]; then
       mkdir PicoProd/$DDD/$RUN
     fi
   done
done

if [ -d PicoList ]; then
   chmod -R ug+rw PicoList
fi
if [ -d CheckLog ]; then
   chmod -R ug+rw CheckLog
fi
if [ -d Log ]; then
   chmod -R ug+rw Log
fi
if [ -d PicoProd ]; then
   chmod -R ug+rw PicoProd
fi
