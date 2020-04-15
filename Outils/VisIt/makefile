# Lance la configuration et la compilation de VisIt et d'eventuels plugins
# Pour downloader/construire/installer une nouvelle version de VisIt, il faut:
# a) Changer le numero de version ci dessous
# b) Taper make
# c) Puis mettre a jour les paquets telecharges en gestion de conf sous $TRUST_ROOT/Outils/VisIt/src
# d) Essayer d'expurger (ex doc/demos/examples dans QT) pour des questions de place

name=visit
version=2_12_1
new_version=3_1_1
install=$(TRUST_ROOT)/exec/VisIt
cible=$(install)/bin/$(name)
#cible=bin/$(name)
check=$(TRUST_ROOT)/build/Outils/VisIt/test/ok   

all: $(cible)
	./install $(version) plugins $(new_version)
	make $(check)

$(cible): ./makefile ./install ./src/build.sh ./src/build_new.sh $(MPI_INCLUDE)/mpi.h
	make clean
	./install $(version) visit $(new_version)

$(check): *.xml install customlauncher $(TRUST_ROOT)/RELEASE_NOTES
	# On ajoute la mise a jour des host profiles et on test
	./install $(version) check $(new_version)

clean:
	rm -r -f $(install) $(TRUST_ROOT)/build/Outils/VisIt src/*.help 
	(cd src; ./build.sh clean)
