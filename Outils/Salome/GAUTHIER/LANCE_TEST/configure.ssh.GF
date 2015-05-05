#!/bin/ksh
[ ${#TRUST_TMP} = 0 ] && TRUST_TMP=$TMPDIR
adr=$1
# Ajout option -csh pour configurer si le shell par defaut est csh sur la machine
[ ${#1} = 0 ] && echo "Usage: $0 machine [-csh]" && exit
echo "Configuration par ssh de $adr"
cd $HOME
# Attention, il faut (Merci S. Hitzel) que le repertoire $HOME soit en 755
# C'est pour cela que la connection vers jungfrau et dibona ne marchait pas  
[ ! -d .ssh ] && mkdir .ssh && chmod 700 .ssh
type=rsa
if [ ! -f .ssh/authorized_keys2 ]
then
   ssh-keygen -t dsa -f .ssh/id_$type -N ""
   # Il semble que sur vielles versions ssh l'option -t n'existe
   # pas et en plus ca marche par exemple avec login mathieu (Mke 7.2)
   #ssh-keygen -d -f .ssh/id_$type -N ""
   cp -f .ssh/id_$type.pub .ssh/authorized_keys2
fi
eval `ssh-agent` 1>/dev/null 2>&1 
ssh-add .ssh/id_$type 1>/dev/null 2>&1
# Le fichier necessaire sur le remote .ssh est
# authorized_keys pour toutes les machines
# authorized_keys2 pour dom1.alpes.cea.fr 
# Attention le - devant les commandes ne marchent pas forcement
# Donc on fait en 2 fois:
# On copie les fichiers par scp
# On cree .ssh et les tests avec ssh
echo "Copie de fichiers sur $adr:"
if [ "$2" = -csh ]
then
   echo $ECHO_OPTS "#!/bin/bash\nmkdir .ssh 2>/dev/null;chmod 700 .ssh;cd .ssh;" > .run
   echo $ECHO_OPTS "for file in authorized_keys authorized_keys2; do " >> .run
   echo $ECHO_OPTS "[ \"\`grep '$HOST' \$file 2>/dev/null\`\" = \"\" ] && cat ../authorized_keys2 >> \$file && chmod 600 \$file" >> .run
   echo $ECHO_OPTS "done;rm -f ../authorized_keys2" >> .run
fi
scp .ssh/authorized_keys2 $adr":"
echo "Configuration de \$HOME/.ssh sur $adr:"
HOST=`hostname`
if [ "$2" = -csh ]
then
   ssh $adr -n 'chmod +x .run;./.run' 
else
	ssh $adr -n 'mkdir .ssh 2>/dev/null;chmod 700 .ssh;cd .ssh;for file in authorized_keys authorized_keys2; do [ "`grep '$HOST' $file 2>/dev/null`" = "" ] && cat ../authorized_keys2 $file | sort -u >prov_auto&& mv prov_auto $file && chmod 600 $file;done;rm -f ../authorized_keys2'
fi
echo $ECHO_OPTS "Test de ssh sur $adr :\c"
ssh $adr -n ls > $TRUST_TMP/ssh_com
if [ $? = 0 ]
then
   echo "OK!"
   exit 0
else
   echo "Error."
   exit 1
fi
eval `ssh-agent -k` 1>/dev/null 2>&1 
