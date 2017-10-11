#!/bin/bash
#######################################################################
#     Bienvenues au CINES sur la machine de visualisation CRISTAL     #
#                                                                     #
# -1 noeuds R428 : 32 coeurs Intel Sandy Bridge @ 2.20GHz,   1To RAM  #
#                  4 GPU Quadro 7000                                  #
# -8 noeuds R425 : 16 coeurs Intel Sandy Bridge @ 2.20GHz, 128Go RAM  #
#                  2 GPU Quadro 6000                                  #
# -Espaces communs a Occigen et Cristal :                             #
#   temporaire /scratch (Lustre); sauvegarde© /home (Panasas)         #
#                                                                     #
#                           Red Hat 6.7                               #
#                                                                     #
#  - Pour acceder a vos espaces de travail, utilisez les variables    #
#    $HOMEDIR $SHAREDHOMEDIR $SCRATCHDIR $SHAREDSCRATCHDIR $STOREDIR  #
#    Exemple d'utilisation : cd $SCRATCHDIR                           #
#                                                                     #
#######################################################################


##################################
# Variables for configure script #
##################################
define_modules_config()
{
   env=$TRUST_ROOT/env/machine.env
   #
   # Create ROMIO_HINTS file for MPI IO optimizations on Lustre file system
   echo "# ROMIO HINTS
# Select the max OST available:
striping_factor -1
# Collective comm between nodes before write:
romio_cb_write enable
# Collective comm between nodes before read:
romio_cb_read  enable
# One process on each node do the coll comm task:
cb_config_list *:1" > ROMIO_HINTS.env
   echo "export ROMIO_HINTS=\$TRUST_ROOT/env/ROMIO_HINTS.env # ROMIO HINTS" >> $env
   #
   # Load modules
   # bullxmpi 1.2.8.4 1.2.9.1(default)
   module="bullxmpi/1.2.9.1"
   #
   echo "# Module $module detected and loaded on $HOST."
   echo "module purge 1>/dev/null" >> $env
   echo "module load $module 1>/dev/null" >> $env     
   . $env
   # Creation wrapper qstat -> squeue
   echo "#!/bin/bash
squeue" > $TRUST_ROOT/bin/qstat
   chmod +x $TRUST_ROOT/bin/qstat
}

##############################
# Variables for trust script #
##############################
define_soumission_batch()
{
   soumission=999
   sub=""
}
