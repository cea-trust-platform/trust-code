BEGIN { 
nfh=1
while ( getline < fich ) {fh[nfh]=$0;nfh++}
# listefh noms des fichiers .h
# fh tableau des fichiers .h
# rep repertoire $rep_dev/exec
}
# Cas ou il n y'a que 1 SEUL include par ligne (mkmf par HP)
/\.o:/ && ($NF=="\\") { cpp=substr($1,1,length($1)-2)"cpp"
   OK=0
   while (1) {
   # logiquement le nom de l'include est en
   #  avant derniere position sur la ligne :
      include=$(NF-1)
      for (j=1;j<nfh;j++) {
         ind=index(include,fh[j]);
         if (ind==1 || (ind>0 && substr(include,ind-1,1)=="/")) {
      	    # On trouve l'occurrence fh[j] donc on affiche le fichier cpp
	    if (OK==0) print cpp
	    OK=1
	    fic=rep"/."fh[j]".prov"
	    # Sauvegarde dans un fichier provisoire pour
	    # eviter les ctrl-c intempestifs :
	    print cpp >> fic
	 }
      }
      # On sort des que $NF != \ cad si il n'y a plus de \ en fin de ligne
      if ($NF != "\\") break
      else getline
   }
}
#  Cas ou il y'a plus d'un seul include par ligne (makedepend)
/\.o:/ && ($NF!="\\") { cpp=substr($1,1,length($1)-2)"cpp"
   OK=0
   for (i=2;i<NF+1;i++) {
      include=$i
      for (j=1;j<nfh;j++) {
         ind=index(include,fh[j]);
         if (ind==1 || (ind>0 && substr(include,ind-1,1)=="/")) {
      	    # On trouve l'occurrence fh[j] donc on affiche le fichier cpp
            if (OK==0) print cpp
            OK=1
            fic=rep"/."fh[j]".prov"
            # Sauvegarde dans un fichier provisoire pour
            # eviter les ctrl-c intempestifs :
            print cpp >> fic
         }
      }
   }
}

