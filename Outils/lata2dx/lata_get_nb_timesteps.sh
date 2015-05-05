#!/bin/bash

echo "#" $1 
echo "object 1 class array"
echo "type int rank 1 shape 1 items 1"
echo "ascii data follows"
grep TEMPS $1 | wc -l 

