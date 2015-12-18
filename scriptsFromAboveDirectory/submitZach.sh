#!/bin/bash

for RUN in `cat List/sampleDec16.list`
do
     echo $RUN
     #cd Submit/$day/$RUN
     #ln -s ../../../AuAu200.xml
     star-submit-template -template bingchuSubmit.xml -entities runnumber=$RUN
done
