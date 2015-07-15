# Lance la configuration et la compilation de VisIt et d'eventuels plugins
# Pour downloader/construire/installer une nouvelle version de VisIt, il faut:
# a) Changer le numero de version ci dessous
# b) Taper make
# c) Puis mettre a jour les paquets telecharges en gestion de conf sous $TRUST_ROOT/Outils/VisIt/src
# d) Essayer d'expurger (ex doc/demos/examples dans QT) pour des questions de place
version=2_9_1

visit=bin/visit
check=test/ok   

all: $(visit)
	./install $(version) plugins
	make $(check)

$(visit): ./makefile ./install ./src/build.sh $(MPI_INCLUDE)/mpi.h
	make clean
	# gf on essaye 2 fois a cause de mesa 
	./install $(version) visit || ./install $(version) visit

$(check): *.xml install customlauncher $(TRUST_ROOT)/RELEASE_NOTES
	# On ajoute la mise a jour des host profiles et on test
	./install $(version) check

clean:
	rm -r -f bin data current ?.* plugins/*/build $(check)
