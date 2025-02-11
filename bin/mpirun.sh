#!/bin/bash
# Surcharge du script mpirun
usage()
{
echo "Usage: mpirun.sh [-gdb | -valgrind [-gdb]| -valgrind_strict [-gdb]] -np nb_procs binary_path test_case nb_procs" && exit
}
[ $# = 0 ] && usage

# Astuce pour specifier des noeuds
machinefile="" && [ -f machinefile ] && machinefile="-machinefile machinefile"
[ ${#hostfile} != 0 ] && machinefile="-hostfile $hostfile"
# Verifie que l'environnement est bien initialise
if [ "$Mpirun" = "" ]
then
   depart=`pwd`
   cd ${0%/bin/mpirun.sh}
   source env_TRUST.sh
   cd $depart
fi
# Passer progressivement tous les cas dans le script trust...
case $Mpirun in
        /usr/bin/srun) $Mpirun -n $2 $3 $4 $5;;
	/usr/bin/prun) 	$Mpirun -vts -n $2 $3 $4 $5;;
	/opt/hpmpi/bin/mpijob) $Mpirun $3 $4 $5;;
	/opt/hpmpi/bin/mpirun) $Mpirun -srun -n $2 $3 $4 $5;;
	/opt/mpi/mpibull*/bin/mpirun) srun -n $2 $3 $4 $5;;
	*/poe) export MP_NODES=1;export MP_PROCS=$2;$Mpirun $3 $4 $5;;
	*)   	
	        ############################
		# Case provided MPI is used:
		############################ 
	        MPI=OPENMPI
		[ "`$Mpirun --help 2>&1 | grep -i mpich`" != "" ] && MPI=MPICH 
		#[ ${MPI_ROOT%mpich}  != $MPI_ROOT ] && MPI=MPICH
		#[ ${MPI_ROOT%mpich2} != $MPI_ROOT ] && MPI=MPICH2
		
                #################
                # Cas de MPICH2 #
                #################
		if [ $MPI = "MPICH2" ] 
		then
		   # Attention, toujours d'actualite
		   mpd=`ls $MPI_ROOT/bin/mpd /usr/bin/mpd 2>/dev/null | head -1`
                   if [ "$mpd" != "" ]
                   then
		      # Configure
                      if [ ! -f $HOME/.mpd.conf ]
                      then
                	 echo "MPD_SECRETWORD=mr45-j9z" > $HOME/.mpd.conf
                	 chmod 600 $HOME/.mpd.conf
                      fi
		      # Delete this file created by a previous launch of mpd:
		      # Changing id/gid on Saclay PCs had created problem to run mpich2 and this file
		      # should have been deleted to fix it
		      # rm -f /tmp/mpd2.console_`whoami`
		      # Yes but removing this file each time runs several instance of mpd! So it is better to change the location of the file (now $TRUST_TMP):
		      # No tmpdir do not work.
                      $mpd 1>/dev/null & # Run the server
                      # Attendre avant le lancement
                      sleep 2
                      # Ajout d'un timeout de 10s pour MPICH2 livre (vu sur fsf)
                      [ ${Mpirun%lib/src/LIBMPI/mpich/bin/mpirun} != $Mpirun ] && TRUST_MPIRUN_OPTIONS=$TRUST_MPIRUN_OPTIONS" -recvtimeout 10"
                   fi	
		elif [ $MPI = "MPICH" ]
		then
		   ##################
		   # Cas de MPICH 3.x
		   ##################
		   TRUST_MPIRUN_OPTIONS=$TRUST_MPIRUN_OPTIONS""
		elif [ $MPI = "OPENMPI" ] 
		then
		   ##################
		   # Cas de OPENMPI #
		   ##################
		   # Message erreur sur OpenMPI 3/4/5.x sur PCs avec pas assez de coeurs: There are not enough slots available in the system to satisfy the ...
		   Mpirun="$Mpirun -oversubscribe"
		fi	
			
		#######################
		# Lancement avec -gdb #
		#######################
		if [ "$1" = "-gdb" ]
		then
		   unset TRUST_GDB 
		   if [ $MPI = "MPICH2" ]
		   then	
		      # MPICH2 != MPICH (MPICH=MPICH3)
		      # Nouveau avec MPICH2 livre, la variable d'environnement TRUST_GDB est mise a 1 pour forcer le scatter a ouvrir une fenetre
		      # pour tous les processeurs, cela evite de mettre debug dans le code lorsque c'est compile en shmem       
		      export TRUST_GDB=1
		      $Mpirun $machinefile $*
		   elif [ $MPI = "OPENMPI" ] || [ $MPI = "MPICH" ]
		   then
		      # OpenMPI && MPICH 3:
		      gdb_options=`mktemp_`
		      gdb=`mktemp_`
		      shift;shift
		      np=$1;shift
		      exec=$1;shift
		      option="" && [ $MPI = "OPENMPI" ] && option=-on_error_abort
		      echo $ECHO_OPTS "break exit\nbreak main\nrun $* $option\nbreak Process::exit(int)\necho -> Enter cont to continue: \n" > $gdb_options
                      echo "you can add breakpoints or cont in break_points file"
		      if [ -f break_points ]
		      then
			  echo "On rajoute les breakpoints inclus dans le fichier break_points"
			  cat break_points >> $gdb_options
		      fi		
                      echo gdb.sh -q $exec -x $gdb_options > $gdb
                      chmod +x $gdb
                      [ "x$Xterm" = "x" ] && echo "Warning: gdb will not work in parallel unless you install xterm and export Xterm=xterm"
		      # Attention xterm -e sur Mandriva 2008 n'accepte plus qu'une seule commande derriere -e
		      if [ $MPI = "OPENMPI" ]
		      then
		         #$Mpirun -np $np -d $Xterm -e $gdb
                 $Mpirun -np $np `echo $Xterm | awk '{print $1}'` -geometry 150x30 -hold -e $gdb -ex run --args
		      elif [ $MPI = "MPICH" ]
		      then
		         #$Mpirun -np $np `echo $Xterm | awk '{print $1}'` -e $gdb
                 $Mpirun -np $np `echo $Xterm | awk '{print $1}'` -geometry 150x30 -hold -e $gdb -ex run --args
		      fi
		      rm -f $gdb_options $gdb
		   else
		      echo "$1 not supported yet."
		      exit
		   fi
		#######################
		# Support de valgrind #
		#######################
		elif [ "`echo $* | grep bin/valgrind`" != "" ]
		then
		   # See http://valgrind.org/docs/manual/mc-manual.html#mc-manual.mpiwrap
                   MPIWRAP_DEBUG="verbose"
		   val=`which valgrind`
		   VALGRIND_ROOT=`dirname $val`/..
                   LD_PRELOAD=`\ls $VALGRIND_ROOT/lib/valgrind/libmpiwrap*.so 2>/dev/null`	  
		   if [ "$LD_PRELOAD" = "" ]
		   then
		      echo "The debug debug wrapper library not found $TRUST_ROOT/exec/valgrind/lib/valgrind/libmpiwrap*.so ?"
		      echo "You can't use valgrind in parallel."
		      exit -1
		   fi 
		   command="DDMPIWRAP_DEBUG=$MPIWRAP_DEBUG LD_PRELOAD=$LD_PRELOAD $Mpirun $machinefile $*"
		   echo $command
		   eval $command	
		else
		   #######################
		   # Lancement classique #
		   #######################
		   
	           # Binding in order to reduce memory access in parallel (For 2 CPUs asked, 1 binded to one socket (processor), 2 to the other socket)
		   # See: http://www.mcs.anl.gov/petsc/documentation/faq.html#computers
		   # Wait and see, highly version OpenMPI & MPICH dependant !!!
                   #[ $MPI = "MPICH" ]   && TRUST_MPIRUN_OPTIONS=$TRUST_MPIRUN_OPTIONS"--binding cpu:sockets"
		   #[ $MPI = "OPENMPI" ] && TRUST_MPIRUN_OPTIONS=$TRUST_MPIRUN_OPTIONS"--bysocket --bind-to-socket --report-bindings"
		   command="$Mpirun $TRUST_MPIRUN_OPTIONS $machinefile $*"		   
		   #echo $command
		   eval $command
   		   err=$?
   		   # Essaie de relancer le calcul si plantage MPI aleatoire (vieilles versions d'OpenMPI)
   		   if [ -f $4.err ]
   		   then
		      # exited on signal 15 se produit sur une erreur TRUST
		      if [ "`grep -e 'writev failed' -e 'Address not mapped' $4.err`" != "" ]
		      then
		         # On previent le support
      		         $Mpirun $TRUST_MPIRUN_OPTIONS $machinefile $*
      		         err=$?
		      fi
   		   fi
   		   exit $err
		fi
		;;
esac
