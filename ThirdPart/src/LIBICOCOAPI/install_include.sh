mkdir -p $TRUST_ICOCOAPI/include

cd include
for f in `ls *.h *.hxx`
do

[ "`diff $f $TRUST_ICOCOAPI/include/$f 2>&1`" != "" ] && cp $f $TRUST_ICOCOAPI/include && echo $f updated
done
exit 0
