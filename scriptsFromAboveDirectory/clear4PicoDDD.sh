#!/bin/bash

day=$1

if [ -d Submit/$day ]; then
  rm -rf Submit/$day/*/*
fi
if [ -d Log/$day ]; then
  rm -rf Log/$day/*/*
fi
if [ -d PicoProd/$day ]; then
  rm -rf PicoProd/$day/*/*
fi
