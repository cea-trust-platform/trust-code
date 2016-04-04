mkdir -p include  build
mkdir -p $TRUST_LATAFILTER/lib
cd build
ln -sf ../commun_triou/* . 1>/dev/null 2>&1
if [ ! -f makefile ] || [ ../makefile.sa -nt makefile ]
then
   
   cp ../makefile.sa makefile
   make clean
fi
make depend
$TRUST_MAKE || exit -1

# Construction du repertoire include
cd ../include
ln -sf ../commun_triou/*.h .
cd ..
mkdir -p $TRUST_LATAFILTER/include
for f in `ls include/*.h`
do
[ "`diff $f $TRUST_LATAFILTER/include 2>&1`" != "" ] && cp  $f $TRUST_LATAFILTER/include && echo $f updated
done
exit 0
