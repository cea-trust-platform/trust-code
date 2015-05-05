#!/bin/bash
#

SW_NUMACTL=""
SW_NODE=""
SW_CPUSET="1"
# GF ajout de etection de PSC_MPI_RANK car nouvelle version de mpirun
[ -n "${PSC_MPI_RANK}" ] && MPIRUN_RANK=${PSC_MPI_RANK} 
if [ -n "${GMPI_ID}" ] ; then
    # Myrinet: GMPI_ID goes from 0 to (n-1)
    SW_NODE=$(( ${GMPI_ID} % 2 ))
elif [ -n "${MPIRUN_RANK}" ] ; then
    # Infiniband Silverstorm: MPIRUN_RANK goes from 0 to (n-1)
    SW_NODE=$(( ${MPIRUN_RANK} % 2 ))
fi

[ -n "${SW_NODE}" -a "${SW_CPUSET}" = "1" ] && \
    SW_NUMACTL="/usr/bin/numactl --membind=${SW_NODE} --cpubind=${SW_NODE}"
    #SW_NUMACTL="/usr/bin/numactl   --preferred=${SW_NODE}"

[ -n "${SW_VERBOSE}" ] && echo "SW_NODE=$SW_NODE SW_CPUSET=$SW_CPUSET Running $@"
exec ${SW_NUMACTL} "$@"

