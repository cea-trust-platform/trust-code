"""
Author : Elie Saikali
Script to read the med Mesh file and generate a nonuniform isotropic conductivity Field.
"""
import medcoupling as mc


# Name of mesh med file
fName = "Mesh_1.med"
mesh_f = mc.MEDFileUMesh(fName)
mesh2d = mesh_f.getMeshAtLevel(0)

# We create the conductivity field
numb=mesh2d.getNumberOfCells()
conduc = mc.DataArrayDouble(numb,1)
# Get cell center coord along both x and y-directions
coo2Dx = mesh2d.computeCellCenterOfMass()[:, 0]
coo2Dy = mesh2d.computeCellCenterOfMass()[:, 1]

eta=1.0

for i in range(numb):
	conduc[i,0]=coo2Dy[i]**2+eta*coo2Dx[i]**2

fconduc = mc.MEDCouplingFieldDouble(mc.ON_CELLS, mc.ONE_TIME)
fconduc.setArray(conduc)
fconduc.setMesh(mesh2d)
fconduc.setName("CONDUCTIVITY_ELEM_dom")
mc.WriteField("conduc2d_iso.med",fconduc, True)
