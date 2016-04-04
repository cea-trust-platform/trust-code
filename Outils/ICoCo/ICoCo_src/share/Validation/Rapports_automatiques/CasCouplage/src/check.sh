ln -sf ref_lml ref.lml
compare_lata Post1.lml ref.lml  > diff_resu.out
status=$?
cat diff_resu.out
exit $status

