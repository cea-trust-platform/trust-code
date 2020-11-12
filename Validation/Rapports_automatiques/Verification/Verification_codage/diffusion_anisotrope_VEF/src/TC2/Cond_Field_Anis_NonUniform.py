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
# Get cell center coord along both x and y-directions
coo2Dx = mesh2d.computeCellCenterOfMass()[:, 0]
coo2Dy = mesh2d.computeCellCenterOfMass()[:, 1]

# Values based on test case of https://hal.archives-ouvertes.fr/hal-00605548/document
# Sec 7.4
eta=1.0

for i in range(numb):
	conduc[i,0]=coo2Dy[i]**2+eta*coo2Dx[i]**2
	conduc[i,1]=coo2Dx[i]*coo2Dy[i]*(eta-1.0)
	conduc[i,2]=conduc[i,1]
	conduc[i,3]=coo2Dx[i]**2+eta*coo2Dy[i]**2

fconduc = mc.MEDCouplingFieldDouble(mc.ON_CELLS, mc.ONE_TIME)
fconduc.setArray(conduc)
fconduc.setMesh(mesh2d)
fconduc.setName("CONDUCTIVITY_ELEM_dom")
mc.WriteField("conduc2d_aniso.med",fconduc, True)
