import MEDLoader as ml
import numpy as np

### Generate MED mesh #####

xarr = ml.DataArrayDouble.New(11,1)
xarr.iota(0.)
cmesh = ml.MEDCouplingCMesh.New()
cmesh.setCoords(xarr,xarr,xarr)
mesh = cmesh.buildUnstructured()
mesh.setName("mesh")

# define faces
mesh_fac, _, _, r, rI = mesh.buildDescendingConnectivity()
dsi = rI.deltaShiftIndex()
skin_i = dsi.findIdsEqual(1)
skin_i.setName("frontieres")
bary_fac = mesh_fac.computeCellCenterOfMass()

eps = 1.0e-4

bary_fac_z = bary_fac[:,2]
bc1 = bary_fac_z.findIdsInRange(-eps, +eps)
bc1.setName("down")
bc2 = bary_fac_z.findIdsInRange(10.-eps, 10.+eps)
bc2.setName("up")

bary_fac_x = bary_fac[:,0]
bc3 = bary_fac_x.findIdsInRange(-eps, +eps)
bc3.setName("left")
bc4 = bary_fac_x.findIdsInRange(10.-eps, 10.+eps)
bc4.setName("right")

bary_fac_y = bary_fac[:,1]
bc5 = bary_fac_y.findIdsInRange(-eps, +eps)
bc5.setName("front")
bc6 = bary_fac_y.findIdsInRange(10.-eps, 10.+eps)
bc6.setName("back")

#Recording
mfu = ml.MEDFileUMesh()
mfu.setMeshAtLevel(0, mesh)
mfu.setMeshAtLevel(-1, mesh_fac)
mfu.setGroupsAtLevel(-1, [skin_i, bc1, bc2,bc3,bc4,bc5,bc6])
mfu.write("mesh.med", 2) # 2 stands for write from scratch

# Add Partitionnement
meshRead = ml.ReadUMeshFromFile("mesh.med","mesh",0)

arr2 = ml.DataArrayInt(meshRead.getNumberOfCells())

arr2[0:250] = 0
arr2[250:500] = 1
arr2[500:750] = 2
arr2[750:1000] = 3

f = ml.MEDCouplingFieldInt.New(ml.ON_CELLS, ml.ONE_TIME)
f.setArray(arr2)
f.setMesh(meshRead)
f.setName("Thread")

ml.WriteField("Partition.med",f,True)
