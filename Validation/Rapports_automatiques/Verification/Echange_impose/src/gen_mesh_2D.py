import medcoupling as mc

def doIt(suff, y_coords,quad=False):
    # genere un maillage de N seg alignes sur une ligne entre -1 et 1. Les N/2 premiers elem sont dans le groupe 'left_elem'
    # les autres dans le groupe 'right_elem'
    N_ELEM = 10  # doit etre pair
    if quad:
        F_NAME = "domain_%s_quad.med" % suff
    else:
        F_NAME = "domain_%s.med" % suff

    m = mc.MEDCouplingCMesh("med_domain")
    da = mc.DataArrayDouble(N_ELEM+1)
    da.iota()
    da /= 0.5*da[N_ELEM,0]

    if quad:
        da *= 0.5
        da *= da
        da *= 2.0

    da -= 1.0

    m.setCoordsAt(0, da)
    m.setCoordsAt(1, mc.DataArrayDouble(y_coords))
    m = m.buildUnstructured()

    m.setName("med_domain_%s" % suff)

    m_desc = m.buildDescendingConnectivity()[0]
    #m_desc.writeVTK("/tmp/merged_1D.vtu")
    coo = m.getCoords()

    # Creation des groupes d'elements et de faces
    bary = m.computeCellCenterOfMass()
    ids_left_elem = bary[:,0].findIdsInRange(-1.001, 0.0)
    ids_right_elem = bary[:,0].findIdsInRange(0.0, 1.001)
    ids_left_elem.setName("left_elem")
    ids_right_elem.setName("right_elem")

    if suff == "1D":
        tab = coo[:, 0]
    elif suff == "2D":
        baryD = m_desc.computeCellCenterOfMass()
        tab = baryD[:,0]

    ids_left = tab.findIdsInRange(-1.001,-0.999);    ids_left.setName("Gauche")
    ids_right = tab.findIdsInRange(0.9999,1.0001);  ids_right.setName("Droit")

    if suff=="1D":
        full_grps = [ids_left, ids_right]
    elif suff == "2D":
        ids_up = baryD[:, 1].findIdsInRange(y_coords[1]-0.001,y_coords[1]+0.001);        ids_up.setName("Haut")
        ids_down = baryD[:, 1].findIdsInRange(-0.01,0.01);     ids_down.setName("Bas")
        full_grps = [ids_left, ids_right, ids_up, ids_down]


    # Enregistrement :
    mfu = mc.MEDFileUMesh()
    mfu.setMeshAtLevel(0, m)
    mfu.setGroupsAtLevel(0, [ids_left_elem, ids_right_elem])
    mfu.setMeshAtLevel(-1, m_desc)
    mfu.setGroupsAtLevel(-1, full_grps)
    mfu.write(F_NAME, 2)

if __name__ == "__main__":
    doIt("2D", [0.0,0.2])
