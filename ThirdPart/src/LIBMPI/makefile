# Lance la configuration et la compilation du MPI livre
# env MAKELEVEL= pour eviter une erreur d'installation sur Mandriva 2007
mpi=$(MPI_ROOT)/bin/mpirun
all:$(mpi)
$(mpi): Installer_mpi Installer_openmpi Installer_mpich makefile
	make clean
	#env MAKELEVEL= ./Installer_mpi
	./Installer_mpi

clean:
	rm -r -f hwloc openmpi openmpi-?.?.? mpich2 mpich $(MPI_ROOT)
	rm -f *.gz *.tar *.tgz
