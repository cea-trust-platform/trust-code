import medcoupling as mc

# meshes from https://github.com/ndjinga/FVCA_Meshes
for m in ["Kershaw2D"]:
    for i in range(1, 7):
        mesh_name = f"{m}/jdd_{i}/mesh.med"
        mfum = mc.MEDFileUMesh(mesh_name)
        mfum.setName("mesh")
        mfum.getMeshAtLevel(0).setName("mesh")
        mfum.getMeshAtLevel(0).convertAllToPoly()
        # on regroupe tous les groupes en un seul
        arrays = []
        for gn in mfum.getGroupsNames():
            arrays.append(mfum.getGroupArr(-1, gn))
            mfum.removeGroup(gn)
        a = mc.DataArrayInt.Aggregate(arrays)
        a.sort()
        a.setName("boundary")
        mfum.addGroup(-1, a)
        mfum.write(mesh_name, 2)
        # mfum.write(f"/tmp/{m}_jdd_{i}_mesh.med", 2)