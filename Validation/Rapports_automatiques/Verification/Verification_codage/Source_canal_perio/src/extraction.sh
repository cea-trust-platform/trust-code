# This script is called from validation form exercise in the user tutorial
nb1=` grep "Total number of elements" std.err | awk '{print $NF}'`
nb2=` grep "Total number of elements" debit/debit.err | awk '{print $NF}'`
nb3=` grep "Total number of elements" debit2/debit2.err | awk '{print $NF}'`
echo $nb1 $nb2 $nb3 > nbcells.dat
tp1=` grep "Backup of the field" std.err | awk '{print $NF}' | head -n 1`
tp2=` grep "Backup of the field" debit/debit.err | awk '{print $NF}' | head -n 1`
tp3=` grep "Backup of the field" debit2/debit2.err | awk '{print $NF}' | head -n 1`
echo $tp1 $tp2 $tp3 > lasttime.dat
