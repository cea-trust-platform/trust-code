import medcoupling as mc

# genere un maillage de N seg alignes sur une ligne entre -1 et 1. Les N/2 premiers elem sont dans le groupe 'left_elem'
# les autres dans le groupe 'right_elem'

N_ELEM = 30  # doit etre pair
F_NAME = "domain_1D.med"

m = mc.MEDCouplingCMesh("med_domain")
da = mc.DataArrayDouble(N_ELEM+1)
da.iota()
da /= 0.5*da[N_ELEM,0]
da -= 1.0
m.setCoordsAt(0, da)
m.setCoordsAt(1, mc.DataArrayDouble([0.0]))


m = m.buildUnstructured()

# Split into two sub-parts
m1 = m[0:N_ELEM//2].deepCopy()
m1.zipCoords()
m2 = m[N_ELEM//2:].deepCopy()
m2.zipCoords()
m = mc.MEDCouplingUMesh.MergeUMeshes([m1, m2])
m.setName("med_domain_1D")

m_desc = m.buildDescendingConnectivity()[0]
m_desc.writeVTK("/tmp/merged_1D.vtu")
coo = m.getCoords()

# Creation des groupes d'elements et de faces
bary = m.computeCellCenterOfMass()
ids_left_elem = bary[:,0].findIdsInRange(-1.001, 0.0)
ids_right_elem = bary[:,0].findIdsInRange(0.0, 1.001)
ids_left_elem.setName("left_elem")
ids_right_elem.setName("right_elem")

ids_left = coo[:, 0].findIdsInRange(-1.001,-0.999);    ids_left.setName("Gauche")
ids_right = coo[:, 0].findIdsInRange(0.9999,1.0001);  ids_right.setName("Droit")
assert(ids_left.getNumberOfTuples() == 1)
assert(ids_right.getNumberOfTuples() == 1)

# Creation des groupes de "paroi interne" = noeud duplique
## ATTENTION, code sensible a la numerotation
ids_int_gche = mc.DataArrayInt([15]);  ids_int_gche.setName("interne_gauche")
ids_int_dte = mc.DataArrayInt([16]);  ids_int_dte.setName("interne_droite")
ids_int_all = mc.DataArrayInt([15,16]);  ids_int_all.setName("interne_tout")

# Enregistrement :
mfu = mc.MEDFileUMesh()
mfu.setMeshAtLevel(0, m)
mfu.setGroupsAtLevel(0, [ids_left_elem, ids_right_elem])
mfu.setMeshAtLevel(-1, m_desc)
#mfu.setGroupsAtLevel(-1, [ids_left, ids_right, ids_up, ids_down, ids_int_gche, ids_int_dte])
mfu.setGroupsAtLevel(-1, [ids_left, ids_right, ids_int_all])
mfu.write(F_NAME, 2)
