#!/bin/csh

rm log_test
root4star -b -l << EOF >& log_test
.O2
.x makePicoDst.C
.q
EOF
