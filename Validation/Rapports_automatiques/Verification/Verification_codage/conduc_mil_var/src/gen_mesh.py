import MEDLoader as mc

# genere un maillage de N cubes alignes sur une ligne entre -1 et 1. Les N/2 premiers elem sont dans le groupe 'left_elem'
# les autres dans le groupe 'right_elem'

N_ELEM = 30  # doit etre pair
F_NAME = "domain.med"

m = mc.MEDCouplingCMesh("med_domain")
da = mc.DataArrayDouble(N_ELEM+1)
da.iota()
da /= 0.5*da[N_ELEM,0]
da -= 1.0
m.setCoordsAt(0, da)
m.setCoordsAt(1, mc.DataArrayDouble([0.0,1.0]))

m = m.buildUnstructured()
m_desc = m.buildDescendingConnectivity()[0]

# Creation des groupes d'elements et de faces
bary = m.computeCellCenterOfMass()
ids_left_elem = bary[:,0].findIdsInRange(-1.001, 0.0)
ids_right_elem = bary[:,0].findIdsInRange(0.0, 1.001)
ids_left_elem.setName("left_elem")
ids_right_elem.setName("right_elem")
bary = m_desc.computeCellCenterOfMass()
ids_up = bary[:, 1].findIdsInRange(0.9,1.1);        ids_up.setName("Haut")
ids_down = bary[:, 1].findIdsInRange(-0.1,0.1);     ids_down.setName("Bas")
ids_left = bary[:, 0].findIdsInRange(-1.001,-0.999);    ids_left.setName("Gauche")
ids_right = bary[:, 0].findIdsInRange(0.9999,1.0001);  ids_right.setName("Droit")
assert(ids_left.getNumberOfTuples() == 1)
assert(ids_right.getNumberOfTuples() == 1)

mfu = mc.MEDFileUMesh()
mfu.setMeshAtLevel(0, m)
mfu.setGroupsAtLevel(0, [ids_left_elem, ids_right_elem])
mfu.setMeshAtLevel(-1, m_desc)
mfu.setGroupsAtLevel(-1, [ids_left, ids_right, ids_up, ids_down])
mfu.write(F_NAME, 2)

# Creation d'un champ T_init au som pour la discret EF
f = mc.MEDCouplingFieldDouble(mc.ON_NODES, mc.ONE_TIME)
n_nodes = N_ELEM+1
da = mc.DataArrayDouble(n_nodes*2)
da[:n_nodes/2] = 20.0
da[n_nodes/2:n_nodes] = 10.0
da[n_nodes:3*n_nodes/2] = 20.0
da[3*n_nodes/2:] = 10.0
da[n_nodes/2] = 15.0
da[3*n_nodes/2] = 15.0
f.setArray(da)
f.setMesh(m); f.setName("T_init"); f.setTime(0.0, 0, 0)
mc.WriteFieldUsingAlreadyWrittenMesh(F_NAME, f)
