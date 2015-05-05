#!/bin/bash
echo $$ > $TRUST_TMP/.exec
ok=0
# Efface car sinon probleme possible si $TRUST_TMP/exec existe deja !
file=$TRUST_TMP/exec
rm -f $file
while [ 1 ]
do
   [ -f $file ] && (nohup $file 1>/dev/null 2/dev/null &)
   sleep 1
done
