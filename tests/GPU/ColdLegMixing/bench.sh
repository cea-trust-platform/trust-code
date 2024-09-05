jdd=ColdLegMixing
cp $jdd.data $jdd"_BENCH".data
jdd=$jdd"_BENCH"
sed -i "1,$ s?3 2 2?61 31 31?" $jdd.data
trust $jdd.data 2>&1 | tee $jdd.out_err
