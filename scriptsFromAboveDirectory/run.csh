#!/bin/csh

root4star -b -l << EOF 
.O2
.x runAnaTree.C($1, "$2", "$3")
.q
EOF
