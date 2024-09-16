for rep in `find */ -name check_perf.sh`
do
	rep=`dirname $rep`
	cd $rep
	echo $rep
	./check_perf.sh
	grep GPU: $rep"_BENCH".TU
	cd - 1>/dev/null 2>&1
done

