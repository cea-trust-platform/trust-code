#!/bin/sh

script=$1
data_in=$2
data_out=$3
data_ref=$4

# python conver_mil.py toto.data toto_out.data && diff toto_out.data ref/toto_ref.data
python $script $data_in $data_out 
diff $data_out $data_ref 
exit $?

