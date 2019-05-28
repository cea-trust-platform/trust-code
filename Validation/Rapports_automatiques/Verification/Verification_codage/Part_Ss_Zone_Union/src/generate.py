import MEDLoader as mc

if __name__ == "__main__":
  m = mc.MEDCouplingCMesh("domaine_glob")
  da = mc.DataArrayDouble(21)
  da.iota()
  m.setCoords(da, da)
  m = m.buildUnstructured()
  g1 = mc.DataArrayInt(m.getNumberOfCells() / 2)
  g1.setName("sous_zone_dom1")
  g1.iota()
  g2 = g1.buildComplement(m.getNumberOfCells())
  g2.setName("sous_zone_dom2")
  mfu = mc.MEDFileUMesh.New()
  mfu.setMeshAtLevel(0, m)
  mfu.setGroupsAtLevel(0, [g1, g2])
  
  # Needs some boundary or will crash:
  m1, _, _, rev, revI = m.buildDescendingConnectivity()
  dsi = revI.deltaShiftIndex()
  ids = dsi.findIdsEqual(1)
  mfu.setMeshAtLevel(-1, m1[ids])
  g = mc.DataArrayInt(ids.getNumberOfTuples())
  g.iota()
  g.setName("out")
  mfu.setGroupsAtLevel(-1, [g])

  mfu.write("domaine_glob.med", 2)
  
  
