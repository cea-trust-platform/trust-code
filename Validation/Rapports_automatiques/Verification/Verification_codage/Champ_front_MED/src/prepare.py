import MEDLoader as ml
import medcoupling as mc
import math

filename = "fields.med"
# Lecture du maillage
meshMEDFileRead = ml.MEDFileMesh.New("mesh.med")
mesh = meshMEDFileRead.getMeshAtLevel(0)

ON_CELLS = True
if ON_CELLS:
    f = ml.MEDCouplingFieldDouble.New(mc.ON_CELLS, mc.ONE_TIME)
    f.setName("VITESSE_ELEM_dom")
    coords = mesh.computeCellCenterOfMass()
    print("Creating a velocity field on cells...")
else:
    f = ml.MEDCouplingFieldDouble.New(mc.ON_NODES, mc.ONE_TIME)
    f.setName("VITESSE_SOM_dom")
    coords = mesh.getCoords()
    print("Creating a velocity field on nodes...")

xc, yc, zc = coords[:,0], coords[:,1], coords[:,2]
f.setArray(coords)
f.setMesh(mesh)

print("Writing mesh into file...")
ml.WriteUMesh(filename,f.getMesh(),True)

# Boucle sur plusieurs pas de temps dt=0.1
dt=0.1
for i in range(0, 30):
    t = i * dt
    print("Writing field at time ",t," into file...")
    # Calcul de f(x,y,z,t)
    for node in range(0, yc.getNumberOfTuples()):
        y = yc[node]
        f.getArray()[node, 0] = 4*y*(1-y)+0.5*math.cos(5*t)
        f.getArray()[node, 1] = 0.0
        f.getArray()[node, 2] = 0.0
    f.setTime(t, i, i)
    ml.WriteFieldUsingAlreadyWrittenMesh(filename, f)

# Ecriture d'une autre vitesse
f.setName("PRESSION_ELEM_dom")
f.setTime(0, 0, 0)
f.fillFromAnalytic(1, "x*0.0+y*(1-y)+z*0.0") # Bug MEDCoupling, il faut specifier x,y,z !
ml.WriteFieldUsingAlreadyWrittenMesh(filename, f)