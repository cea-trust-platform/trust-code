# Lance la configuration et la compilation de VisIt et d'eventuels plugins
# Pour downloader/construire/installer une nouvelle version de VisIt, il faut:
# a) Changer le numero de version ci dessous
# b) Taper make
# c) Puis mettre a jour les paquets telecharges en gestion de conf sous $TRUST_ROOT/Outils/VisIt/src
# d) Essayer d'expurger (ex doc/demos/examples dans QT) pour des questions de place

name=visit
install=$(TRUST_ROOT)/exec/VisIt
cible=$(install)/bin/$(name)
#cible=bin/$(name)
check=$(TRUST_ROOT)/build/Outils/VisIt/test/ok
plugins=$(TRUST_ROOT)/exec/VisIt/current/linux-x86_64/plugins/databases/libEsondeDatabase_ser.so

all: $(cible) $(plugins)

$(plugins): ./plugins/Sondes/build_plugin ./plugins/lata/build_plugin plugins/triofield/build_plugin $(TRUST_ROOT)/exec/lata_analyzer
	./install plugins
	make $(check)

$(cible): ./makefile ./install ./src/build.sh ./src/build_new.sh $(MPI_INCLUDE)/mpi.h
	make clean
	./install visit

$(check): *.xml install customlauncher $(TRUST_ROOT)/RELEASE_NOTES
	# On ajoute la mise a jour des host profiles et on test
	./install check

clean:
	rm -r -f $(install) $(TRUST_ROOT)/build/Outils/VisIt src/*.help 
	(cd src; ./build.sh clean)
