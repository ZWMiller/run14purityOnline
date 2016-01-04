#!/bin/bash

for RUN in `cat List/testList4.list`
do
     echo $RUN
     #cd Submit/$day/$RUN
     #ln -s ../../../AuAu200.xml
     star-submit-template -template bingchuSubmit.xml -entities runnumber=$RUN
done
