#!/bin/bash
# Ecriture du fichier d'ordre venant d'un script cgi ou autre
# execute.ksh sans argument : execute le dernier ordre ecrit
# execute.ksh avec argument : ecrit le fichier d'ordre et
# l'execute si no_exec est vide...
ordre=$TRUST_TMP/.file
#Erreur.ihm $no_exec
if [ ${#*} = 0 ]
then
   $ordre
else
   echo $ECHO_OPTS "#!/bin/bash\n"$*"\nsleep 1" > $ordre
   chmod 755 $ordre
   [ ${#no_exec} = 0 ] && $ordre
   [ "$UTILISE_APPACHE" = "1" ] && echo $ECHO_OPTS "Content-type: application/x-ksh\n"
   echo $ECHO_OPTS "`cat $ordre`"
fi
