import medcoupling as mc
from math import radians, sin, cos, ceil
import numpy as np

def build_mesh(r, h, n, nz, alpha_d, folder, med_name):

    theta_mesh_size = 2.0 # en degres
    nt = ceil(alpha_d / theta_mesh_size) # equivalent axisymetrique (une maille 3D d'epaisseur)
    alpha = radians(alpha_d)

    # maillage de la section de 1 degre
    meshes = []

    x = [i / nt for i in range(nt + 1)]
    y = [i / n for i in range(n + 1)]
    arrx = mc.DataArrayDouble(x)
    arry = mc.DataArrayDouble(y)

    # maillage cartesien
    mccm = mc.MEDCouplingCMesh.New("section")
    mccm.setCoords(arrx, arry)
    # passage en maillage non structure
    mesh_e = mccm.buildUnstructured()
    # conversion des elements en polyhedres pour PolyMAC
    mesh_e.convertAllToPoly()
    # transformation des coordonnees des noeuds
    coo = mesh_e.getCoords()
    for i in range(coo.getNumberOfTuples()):
        r_ = int(coo[i, 1]+1.-1.e-6) * (1/40. + 39./40.*coo[i, 1]) * r
        x = r_ * cos(alpha * coo[i, 0])
        y = r_ * sin(alpha * coo[i, 0])
        coo[i, 0] = x
        coo[i, 1] = y
    meshes.append(mesh_e)

    mesh = mc.MEDCouplingUMesh.MergeMeshes(meshes)
    mesh.mergeNodes(1e-6)
    mesh.zipCoords()
    mesh.convertDegeneratedCells()

    # maillage d'extrusion
    z = [0]
    z.extend([i * h / nz for i in range(1, nz + 1)])

    mesh1d = mc.MEDCouplingUMesh("ex", 1)
    mesh1d.allocateCells(len(z) - 1)
    coo = [0., 0., z[0]]
    for i, z_ in enumerate(z[1:]):
        mesh1d.insertNextCell(mc.NORM_SEG2, 2, [i, i + 1])
        coo.extend([0., 0., z_])
    coo_arr = mc.DataArrayDouble(coo, len(z), 3)
    mesh1d.setCoords(coo_arr)

    # extrusion de mesh par mesh1d
    mesh.changeSpaceDimension(3, z[0])
    mesh = mesh.buildExtrudedMesh(mesh1d, 0)
    mesh.setName("mesh")
    mesh.convertAllToPoly()
    mesh.orientCorrectlyPolyhedrons()

    # groupes de cellules 3D
    bc = mesh.computeCellCenterOfMass()
    g = []
    z_bas_coeur, z_haut_coeur = 0.0, h
    for i, b in enumerate(bc):
        if b[2] < z_bas_coeur or b[2] > z_haut_coeur or (b[0] * b[0] + b[1] * b[1]) > r * r: g.append(i)
        else: g.append(i)

    # maillage des faces
    mf, desc, descIndx, F_E, F_Ei = mesh.buildDescendingConnectivity()
    mf.setName("mesh")
    # groupes de cellules 2D
    bf = mf.computeCellCenterOfMass()
    z_b, z_t = 0, h
    g_t, g_b, g_w, g_s = [], [], [], [] #top, bottom, wall, symmetry
    for i, b in enumerate(bf):
        if   abs(b[2] - z_t) < 1e-5: g_t.append(i)
        elif abs(b[2] - z_b) < 1e-5: g_b.append(i)
        elif ( b[1]<1.e-8 ) : g_s.append(i)
        elif ( b[0]>1.e-8 ) and (abs(alpha - np.arctan(b[1]/b[0]))) < 1e-5: g_s.append(i) # use of pi/2-angle because we know b[1]>0
        elif F_Ei[i + 1] == F_Ei[i] + 1 : g_w.append(i)

    # structure pour stocker les maillages avec leurs groupes
    mm = mc.MEDFileUMesh.New()
    mm.setMeshAtLevel(0, mesh)
    mm.setMeshAtLevel(-1, mf)
    # ajout des groupes
    for (n, g) in [("haut", g_t), ("bas", g_b), ("wall", g_w), ("sym", g_s)]:
        grp = mc.DataArrayInt.New(g)
        grp.setName(n)
        mm.addGroup(-1, grp)

    mm.write(f"{folder}/{med_name}", 2)

if __name__ == "__main__":
    # rayon maillage
    r = 0.02
    # hauteur maillage
    h = 3.08
    # quelque chose
    n = 40
    nz = 80
    build_mesh(r, h, n, nz, 2.0, ".", "canal.med")
