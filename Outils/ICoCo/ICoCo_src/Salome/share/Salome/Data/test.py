#import Problem
import salome
salome.salome_init()
#raw_input("hhhh")
#comp = salome.lcc.FindOrLoadComponent( "FactoryServer","YACS" )
factory="FactoryServer2"
comp = salome.lcc.FindOrLoadComponent( factory,"MED" )
comp = salome.lcc.FindOrLoadComponent( factory,"YACS" )
comp = salome.lcc.FindOrLoadComponent( factory,"Problem" )
print(comp)
import Problem_ORB
comp = salome.lcc.FindOrLoadComponent( factory,"Problem" )
print(comp)
print(dir(comp))
comp.setDataFile("toto")
comp.initialize()
comp.terminate()
if (1):
    comp2 = salome.lcc.FindOrLoadComponent( factory,"Problem" )
    comp2.setDataFile("toto")
    comp2.initialize()
    comp2.terminate()
    print("fin run 2")
