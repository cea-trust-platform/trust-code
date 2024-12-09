for rep in `find */ -name check_solver.sh`
do
	rep=`dirname $rep`
	cd $rep
	echo "======================"
	echo $rep
	jdd=$rep"_BENCH".data
	#jdd=$rep.data
	make_PAR.data $jdd 2
	./check_solver.sh $jdd
	./check_solver.sh PAR_$jdd
	cd - 1>/dev/null 2>&1
done

