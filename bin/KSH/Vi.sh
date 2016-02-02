#!/bin/bash
file=`basename $1`
file=`Find.sh $file`
vi $file
echo $file
