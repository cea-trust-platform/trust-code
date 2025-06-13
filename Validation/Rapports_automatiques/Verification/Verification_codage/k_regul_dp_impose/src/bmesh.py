import medcoupling as mc

def build_mesh(tets, original_med, target_med):
    mfum = mc.MEDFileUMesh(original_med)
    mesh = mfum.getMeshAtLevel(0)
    mesh.mergeNodes(1e-6)
    mesh.zipCoords()

    if tets:
        mesh, _, _ = mesh.tetrahedrize(mc.GENERAL_24)
        mesh = mesh.buildUnstructured()
        mesh.mergeNodes(1e-6)
        mesh.zipCoords()

    mf, _, _, F_E, F_Ei = mesh.buildDescendingConnectivity()
    bf = mf.computeIsoBarycenterOfNodesPerCell()

    surf1, surf2, wall, iwall = [], [], [], []
    for i, b in enumerate(bf):
        if F_Ei[i + 1] == F_Ei[i] + 1:
            wall.append(i)
        elif abs(b[2] - 0.5) < 1e-6:
            (surf1 if b[0] < 0.2 else surf2).append(i)
        elif abs(b[0] - 0.2) < 1e-6 and 0.25 < b[2] < 0.75:
            iwall.append(i)

    bc = mesh.computeIsoBarycenterOfNodesPerCell()
    vol_pump = []
    for i, b in enumerate(bc):
        if (b[0] < 0.2) and 0.25 < b[2] < 0.75:
            vol_pump.append(i)
    vol_pump = mc.DataArrayInt.New(vol_pump)
    vol_pump.setName("vol_pump")


    mm = mc.MEDFileUMesh.New()
    mm.setMeshAtLevel(0, mesh)
    mm.addGroup(0, vol_pump)
    mm.setMeshAtLevel(-1, mf)
    for name, g in zip(["surf1", "surf2", "wall", "iwall"], [surf1, surf2, wall, iwall]):
        grp = mc.DataArrayInt.New(g)
        grp.setName(name)
        mm.addGroup(-1, grp)

    mm.buildInnerBoundaryAlongM1Group("iwall")

    mm.write(target_med, 2)
