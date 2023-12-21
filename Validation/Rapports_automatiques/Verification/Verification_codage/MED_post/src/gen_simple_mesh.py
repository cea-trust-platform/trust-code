""" Generate simple triangular or quadrangular mesh 
with dummy field on it.
"""

import medcoupling as mc

def generate(tri_or_quad, dirOut="."):
    N = 3
    TRI_OR_QUAD = tri_or_quad # True: triangles, False: quad

    # Mesh
    tmp = mc.MEDCouplingCMesh("mesh")
    dax = mc.DataArrayDouble(N+1); dax.iota(); 
    day = mc.DataArrayDouble(N+1); day.iota(); 

    tmp.setCoords(dax, day)
    mesh = tmp.buildUnstructured()
    if TRI_OR_QUAD:
        mesh.simplexize(0)

    mesh_fac, _, _, r, rI = mesh.buildDescendingConnectivity()
    dsi = rI.deltaShiftIndex()
    skin_i = dsi.findIdsEqual(1)
    skin_i.setName("skin")

    # Recording
    mfu = mc.MEDFileUMesh()
    mfu.setMeshAtLevel(0, mesh)
    mfu.setMeshAtLevel(-1, mesh_fac)
    mfu.setGroupsAtLevel(-1, [skin_i])
    sfx = {True: "tri", False: "quad"}[TRI_OR_QUAD]
    fName = f"{dirOut}/mesh_simple_{sfx}.med"
    mfu.write(fName, 2)

    # Field part
    f = mc.MEDCouplingFieldDouble(mc.ON_CELLS, mc.ONE_TIME)
    f.setMesh(mesh)
    da = mc.DataArrayDouble(mesh.getNumberOfCells())
    da[:] = 123.
    f.setName("ze_field")
    f.setArray(da)
    mc.WriteFieldUsingAlreadyWrittenMesh(fName, f)

