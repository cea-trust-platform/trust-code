"""
Author : Elie Saikali
Script to read the med Mesh file and generate an anisotropic conductivity Field.
We test a 2D problem, so the field contains 4 components

      /          \
     |  Kxx  Kxy  |
K  = |            |
     |  Kyx  Kyy  |
      \          /

"""
import medcoupling as mc

# Name of mesh med file
fName = "Mesh_1.med"
mesh_f = mc.MEDFileUMesh(fName)
mesh2d = mesh_f.getMeshAtLevel(0)

# We create the conductivity field
numb=mesh2d.getNumberOfCells()
conduc = mc.DataArrayDouble(numb,4)
# Get cell center coord along x-direction
coo2Dx = mesh2d.computeCellCenterOfMass()[:, 0]

# Values based on the test case of https://hal.archives-ouvertes.fr/hal-00605548/document
# Sec 7.2
kxxl=1.
kxyl=-1.
kyyl=4.

kxxr=10.
kxyr=-3.
kyyr=2.

for i in range(numb):
	if (coo2Dx [i] < 0.5 ):
		conduc[i,0]=kxxl
		conduc[i,1]=kxyl
		conduc[i,2]=conduc[i,1]
		conduc[i,3]=kyyl
	else:
		conduc[i,0]=kxxr
		conduc[i,1]=kxyr
		conduc[i,2]=conduc[i,1]
		conduc[i,3]=kyyr

fconduc = mc.MEDCouplingFieldDouble(mc.ON_CELLS, mc.ONE_TIME)
fconduc.setArray(conduc)
fconduc.setMesh(mesh2d)
fconduc.setName("CONDUCTIVITY_ELEM_dom")
mc.WriteField("conduc2d_aniso.med",fconduc, True)
