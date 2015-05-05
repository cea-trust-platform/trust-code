from xsalome_pirate import XSalomeSession
salome = XSalomeSession(modules=["SUPERV,SHELLCOMPO,DISTRIBUTOR"], logger=0)
from GenGraph import DefaNewDataFlow,listOfUseCase, MachineToLaunch, ContainerOfDistributor
TEST = DefaNewDataFlow(listOfUseCase, MachineToLaunch, ContainerOfDistributor)
print "Generation du fichier de supervision"
TEST.Export("TEST.xml")
#print "Taper TEST.Run() pour lancer en batch"
print TEST.Run()
print "coucou",TEST.IsRunning()
while (TEST.IsRunning()):
    from time import sleep
    print " en attente"
    import sys
    sys.stdout.flush()
    sleep(10)
    pass
from os import system
system('killall -q -9 SALOME_Container')
