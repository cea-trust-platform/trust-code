np="" && [ "`ls *Zones`" != "" ] && np=`ls *.Zones | wc -l 2>/dev/null`
for p in 0.00 0.05 0.10 0.15 0.20 0.25 0.30 0.35 0.40 0.45 0.50 0.55 0.6 0.65 0.70 0.75 0.80 0.85 0.90 0.95 1.00
do
   jdd=amgx_$p
   cp amgx.data $jdd.data
   sed -i "1,$ s?XXX?$p?g" $jdd.data
   [ ! -f $jdd.out_err ] && trust $jdd $np 1>$jdd.out_err 2>&1
   oc=`grep "Operator " $jdd.out_err`
   l=`awk '/Number of Levels:/ {print $0}' $jdd.out_err | tail -1`
   ram=`awk '/Maximum / {print "RAM: "$(NF-1)}' $jdd.out_err | tail -1`
   its=`awk '/Total Iterations:/ {print $0}' $jdd.out_err | tail -1`
   msit=`awk '/solve\(per iteration):/ {print 1000*$3}' $jdd.out_err | tail -1`
   t=`awk '/Time to solve system on GPU/ {print "Time: "1000*$NF}' $jdd.out_err | head -1`
   echo -e "$p\t$oc\t$l\t$ram\t$its\t$t\t$msit"
done
