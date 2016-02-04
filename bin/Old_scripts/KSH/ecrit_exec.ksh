#!/bin/bash
# Ecriture du fichier d'ordre venant d'un script cg
ordre=$TRUST_TMP/exec && echo $ECHO_OPTS $*"\nrm -f $ordre" > $ordre && chmod 755 $ordre
# Important META pour ne pas creer un fichier dans le cache !
echo "<HTML><HEAD><META HTTP-EQUIV="pragma" CONTENT="no-cache"></HEAD>`cat $ordre`</HTML>"
#kill -$SIG_EXEC `cat $HOME/.exec`
