import MEDLoader as ml
import numpy as np
import scipy
from scipy import sparse
import sys

sign = lambda a : float((a>0)) - float((a<0))

def middle(a, b):
    m = a + b
    m.applyLin(0.5, 0.)
    return m

class Maillage():
    def __init__(self, mesh_s):
        self.scalaire = mesh_s
        mesh_f, desc, descI, rev, revI = mesh_s.buildDescendingConnectivity2()
        self.decale = mesh_f ; self.decale.setName("faces")
        mesh_a, desca, descaI, reva, revaI = mesh_f.buildDescendingConnectivity()
        self.arete = mesh_a ; self.arete.setName("aretes")
        self.c2f = [desc,  descI ]
        self.f2c = [rev,   revI  ]
        self.a2f = [reva,  revaI ]
        self.f2a = [desca, descaI]
        self.Nc = mesh_s.getNumberOfCells()
        self.Nf = mesh_f.getNumberOfCells()
        self.Na = mesh_a.getNumberOfCells()
        self.vol = mesh_s.getMeasureField(True)
        self.sec = mesh_f.getMeasureField(True)
        self.bary_s = mesh_s.computeCellCenterOfMass()
        self.bary_f = mesh_f.computeCellCenterOfMass()
        self.bary_a = mesh_a.computeIsoBarycenterOfNodesPerCell()
        self.normal = mesh_f.buildOrthogonalField() ; self.normal.setName("normales")
        # volumes de controle decales
        self.distD, self.dist1, self.dist2, self.volD = self.calc_volD()

    def aretesIds(self, face):
        return self.f2a[0].getValues()[self.f2a[1].getValues()[face]:self.f2a[1].getValues()[face+1]]

    def facesIds_a(self, a):
        return self.a2f[0].getValues()[self.a2f[1].getValues()[a]:self.a2f[1].getValues()[a+1]]

    def facesIds_as(self, a):
        sf = []
        for f in self.facesIds_a(a):
            ccw = self.bary_a[a] - self.bary_f[f]
            ccw = ml.DataArrayDouble([ccw.getValues()[1], -ccw.getValues()[0]], 1, 2)
            s = sign(ml.DataArrayDouble.Dot(self.normal.getArray()[f], ccw).getValues()[0])
            sf.append([s, f])
        return sf

    def facesIds(self, cell):
        faces = self.c2f[0].getValues()[self.c2f[1].getValues()[cell]:self.c2f[1].getValues()[cell+1]]
        return [[sign(face), abs(face) - 1] for face in faces]

    def cellsIds(self, face):
        return self.f2c[0].getValues()[self.f2c[1].getValues()[face]:self.f2c[1].getValues()[face+1]]

    def cellsIds_s(self, f):
        sc = []
        for c in self.cellsIds(f):
            s = 0.0
            for [ss, ff] in self.facesIds(c):
                if f == ff: s = -ss
            if s == 0.0: raise Exception("Erreur sur la face %f dans l'operateur de gradient"%(f))
            sc.append([s, c])
        return sc

    def laplacien_face(self, f):
        n = self.normal.getArray()[f].getValues()
        lap = np.zeros(self.Nf + len(self.vta))
        sm = [0., 0., 0., 0., 0., 0., n[0], 0.0, 0.0, n[0], n[1], 0.0, 0.0, n[1]]
        stf, sta, alf, ala = self.calc_coeffs_via_M0123(f, np.array(sm), False, True, True)
        #stf, sta, alf, ala = self.calc_coeffs_via_M0123(f, np.array(sm), f in self.faces_bord, True, True)
        for i, a in enumerate(stf): lap[a] += alf[i]
        for i, a in enumerate(sta): lap[self.Nf + a] += ala[i]
        return lap

    def calc_coeffs_via_M0123(self, f, sm0, v2, mm3, minim):

        stencil_f, sbdnvn, sbdnnvn, sbvt, sbdnvt = [], [], [], [], []
        # liste des faces du stencil : faces des elements voisins
        for c in self.cellsIds(f):
            for [s, i] in self.facesIds(c):
                if i not in stencil_f: stencil_f.append(i)
        # liste des faces du stencil : faces connectees aux aretes de la face f
        for a in self.aretesIds(f):
            for i in self.facesIds_a(a):
                if v2:
                    for c in self.cellsIds(i):
                        for [s, ff] in self.facesIds(c):
                            if ff not in stencil_f: stencil_f.append(ff)
                if i not in stencil_f:
                    stencil_f.append(i)
        for ff in stencil_f:
            for a in self.aretesIds(ff):
                if a in self.faces_vt:
                    if a not in sbvt: sbvt.append(a)
                if a in self.faces_dnvt:
                    if a not in sbdnvt: sbdnvt.append(a)

        if f in self.f_dnvn:
            if f not in sbdnvn: sbdnvn.append(f)
        if f in self.f_dnnvn:
            if f not in sbdnnvn: sbdnnvn.append(f)

        L = self.sec.getArray()[f]
        sm = np.copy(sm0)
        na = len(stencil_f + sbdnvt + sbvt + sbdnnvn + sbdnvn)
        # construction matrices M0, M1, M2, M3
        M0, M1, M2, M3 = np.zeros((2, na)), np.zeros((2**2, na)), np.zeros((2**3, na)),  np.zeros((2**4, na))
        listofstencils, shift = [stencil_f, sbvt, sbdnvt, sbdnvn, sbdnnvn], 0
        # stencil_f : faces internes, sbvt : faces de bord avec vt imposee, sbdnvt : faces de bord avec dnvt imposee, ...
        import copy
        for ftype, stencil in enumerate(listofstencils):
            for a, ff in enumerate(stencil):
                if ftype in [0, 3, 4]:
                    n = copy.deepcopy(self.normal.getArray()[ff].getValues())
                    x0 = self.bary_f[ff]
                elif ftype == 1:
                    n = self.ff_vt[ff]["n"]
                    x0 = self.ff_vt[ff]["x"]
                elif ftype == 2:
                    n = self.ff_dnvt[ff]["n"]
                    x0 = self.ff_dnvt[ff]["x"]
                else: raise Exception("type de face non reconnu face %d : %d"%(f, ftype))
                x = (x0 - self.bary_f[f]).getValues()
                t = [-n[1], n[0]]
                if ftype in [3, 4]: n, t = n, n
                # Matrice M0
                for i in range(2):
                    if ftype in [0, 1]: M0[i, shift + a] = n[i]
                # Matrice M1
                for ij, [i, j] in enumerate([i, j] for i in range(2) for j in range(2)):
                    if ftype in [0, 1]: M1[ij, shift + a] = n[i] * x[j]
                    if ftype in [2, 3]: M1[ij, shift + a] = n[i] * t[j]
                # Matrice M2
                for ijk, [i, j, k] in enumerate([i, j, k] for i in range(2) for j in range(2) for k in range(2)):
                    if ftype in [0, 1]: M2[ijk, shift + a] = n[i] * x[j] * x[k]
                    if ftype in [2, 3]: M2[ijk, shift + a] = n[i] * (t[j] * x[k] + t[k] * x[j])
                    if ftype in [4   ]: M2[ijk, shift + a] = n[i] * t[j] * t[k] * 2.
                # Matrice M3
                for ijkl, [i, j, k, l] in enumerate([i, j, k, l] for i in range(2) for j in range(2) for k in range(2) for l in range(2)):
                    if ftype in [0, 1]: M3[ijkl, shift + a] = n[i] * x[j] * x[k] * x[l]
                    if ftype in [2, 3]: M3[ijkl, shift + a] = n[i] * (t[j] * x[k] * x[l] + t[k] * x[j] * x[l] + t[l] * x[j] * x[k])
                    if ftype in [4   ]: M3[ijkl, shift + a] = n[i] * (t[j] * t[k] * x[l] + t[l] * t[j] * x[k] + t[l] * t[k] * x[j]) * 2.
            shift += len(stencil)

        # M012
        M = np.vstack([M0, M1, M2]) if mm3 else np.vstack([M0, M1])
        Mp = np.linalg.pinv(M)
        # M3P012
        P = np.eye(na) - Mp.dot(M)
        Ma = M3 if mm3 else M2
        MP = Ma.dot(P) ; MPp = np.linalg.pinv(MP, rcond = 1.0e-10)

        # calcul alphas
        if minim: alpha = (np.eye(na) - P.dot(MPp.dot(Ma))).dot(Mp).dot(sm)
        else: alpha = Mp.dot(sm)

        if np.linalg.norm(M.dot(alpha) - sm) > 1.0e-6:
            if v2: raise Exception("Stencil pas trouve face %d"%f)
            else: return self.calc_coeffs_via_M0123(f, sm0, True, mm3, minim)

        alf = alpha[:len(stencil_f)]
        ala, sta = alpha[len(stencil_f):len(stencil_f)+len(sbvt)], [self.faces_vt.index(ff) for ff in sbvt]

        return stencil_f, sta, alf, ala

    def init_f2f(self):

        na = self.Nf + len(self.ff_vt)
        f2f = np.zeros((2 * self.Nf, na))

        for f in range(self.Nf):
            for j in [0, 1]:
                sm = np.zeros(2 + 2**2) ; sm[j] = 1.0
                stf, sta, alf, ala = self.calc_coeffs_via_M0123(f, sm, False, False, True)
                for i, a in enumerate(stf): f2f[2 * f + j, a] = alf[i]
                for i, a in enumerate(sta): f2f[2 * f + j, self.Nf + a] = ala[i]
        self.v_f2f = sparse.csr_matrix(f2f)

        vx, vy, dxvx, dyvx, dxvy, dyvy = np.zeros((self.Nf, na)), np.zeros((self.Nf, na)),np.zeros((self.Nf, na)),np.zeros((self.Nf, na)),np.zeros((self.Nf, na)),np.zeros((self.Nf, na))
        for k, op in enumerate([vx, vy, dxvx, dyvx, dxvy, dyvy]):
            sm = np.zeros(2 + 2**2) ; sm[k] = 1.0
            for f in range(self.Nf):
                stf, sta, alf, ala = self.calc_coeffs_via_M0123(f, sm, False, False, True)
                for i, a in enumerate(stf): op[f, a] = alf[i]
                for i, a in enumerate(sta): op[f, self.Nf + a] = ala[i]
        self.vx   = sparse.csr_matrix(vx)
        self.vy   = sparse.csr_matrix(vy)
        self.dxvx = sparse.csr_matrix(dxvx)
        self.dyvx = sparse.csr_matrix(dyvx)
        self.dxvy = sparse.csr_matrix(dxvy)
        self.dyvy = sparse.csr_matrix(dyvy)

    def f2f3D(self, vn):
        vv = np.hstack([vn.getArray().toNumPyArray(), np.array(self.vta)])
        arr = ml.DataArrayDouble(self.v_f2f.dot(vv).tolist(), self.Nf, 2)
        vf = ml.MEDCouplingFieldDouble.New(ml.ON_CELLS) ; vf.setMesh(self.decale)
        vf.setName("vitesse 3D aux faces") ; vf.setArray(arr)
        return vf

    def faces2cell3D(self, vn):
        arr = ml.DataArrayDouble(self.v_f2c.dot(vn.getArray().toNumPyArray()).tolist(), self.Nc, 2)
        vc = ml.MEDCouplingFieldDouble.New(ml.ON_CELLS) ; vc.setMesh(self.scalaire)
        vc.setName("vitesse aux elements") ; vc.setArray(arr)
        return vc


    def setCL(self, cl):
        # pour chaque bord, on repere les faces
        ghost_cl = {}
        for key, [pos, type, val, hval, Tval] in cl.items():
            dir = key in ["nord", "sud"]
            cl_faces = []
            for i in range(self.Nf):
                if abs(self.bary_f[i].getValues()[dir] - pos) < 1.0e-8: cl_faces.append(i)
            cl[key].append(cl_faces)

        nb_faces_bord, nf = 0, 0
        idI, Pb, hb, Tb, faces_bord, faces_cl_v, aretes_bord = {}, [], [], [], [], [], []
        for key in ["sud", "ouest", "nord", "est"]:
            [pos, type, val, hval, Tval, faces] = cl[key]
            faces_bord.extend(faces)
            if type == "v": faces_cl_v.extend(faces)
            for f in faces:
                for a in self.aretesIds(f):
                    if a not in aretes_bord: aretes_bord.append(a)
                idI[f] = nf
                P_val = val(self.bary_f[f]) if type == "P" else 1.0e5
                Pb.append(P_val)
                hb.append(hval)
                Tb.append(Tval)
                v_val = val(self.bary_f[f].getValues()) if type == "v" else None
                P_val = val(self.bary_f[f]) if type == "P" else None
                #ghost_cl[f]   = {"id":nf, "v":v_val, "P":P_val, "h":hval, "T":Tval}
                ghost_cl[f]   = {"v":v_val, "P":P_val, "h":hval, "T":Tval, "name":key}
                nf += 1
        self.n_fb = idI
        self.tab_bord = {"P":Pb, "T":Tb}
        self.cl = ghost_cl
        self.nb_faces_bord = len(faces_bord)
        self.faces_bord = faces_bord
        self.faces_cl_v = faces_cl_v
        self.aretes_bord = aretes_bord

        self.f_dnvn  = []
        self.ff_dnvt = {}
        self.ff_vt   = {}
        self.faces_dnvt = []
        self.faces_vt   = []
        self.f_dnnvn = []
        self.vta   = []

        # vitesse normale
        for j, f in enumerate(self.faces_bord):
            if self.cl[f]["v"] is not None: self.f_dnnvn.append(f)
            else: self.f_dnvn.append(f)

        # vitesse tangentielle
        import copy
        for a in self.aretes_bord:
            fb = []
            for f in self.facesIds_a(a):
                if f in self.faces_bord: fb.append(f)
            if len(fb) != 2: raise Exception("arete de bord n'a pas 2 voisins")
            else: [f1, f2] = fb
            n1, n2 = self.normal.getArray()[f1].toNumPyArray()[0], self.normal.getArray()[f2].toNumPyArray()[0]
            if abs(np.dot(n1, n2)) < 1.0e-8:
                for f in fb:
                    #n = list(self.normal.getArray()[f].getValues()) ; n = np.array([-n[1], n[0]])
                    n = copy.deepcopy(self.normal.getArray()[f].getValues()) ; n = np.array([-n[1], n[0]])
                    if self.cl[f]["v"] is None:
                        self.faces_dnvt.append(a)
                        self.ff_dnvt[a] = {"x": self.bary_a[a], "n": n}
                    else:
                        self.ff_vt[a] = {"x": self.bary_a[a], "n": n}
                        self.faces_vt.append(a)
                        self.vta.append(np.dot(np.array(self.cl[f]["v"]), n))
            else:
                #n = list(self.normal.getArray()[f1].getValues()) ; n = np.array([-n[1], n[0]])
                n = copy.deepcopy(self.normal.getArray()[f1].getValues()) ; n = np.array([-n[1], n[0]])
                if self.cl[f1]["v"] is None:
                    self.faces_dnvt.append(a)
                    self.ff_dnvt[a] = {"x": self.bary_a[a], "n": n}
                else:
                    self.ff_vt[a] = {"x": self.bary_a[a], "n": n}
                    self.faces_vt.append(a)
                    self.vta.append(np.dot(np.array(self.cl[f1]["v"]), n))

    def get_ghost(self, c, f):
        xi, xf, nf = self.bary_s[c], self.bary_f[f], self.normal.getArray()[f]
        xf_xi_nf = ml.DataArrayDouble.Dot((xi - xf), nf)
        dn = nf.deepCopy() ; dn.applyLin(xf_xi_nf.getValues()[0], 0)
        return xf + dn

    def calc_coefficients(self):
        coefficients = []
        stencil_ghost1, stencil_ghost2, stenb1, stenb2 = np.zeros((self.Nf, self.Nc)), np.zeros((self.Nf, self.Nc)), np.zeros((self.Nf, self.nb_faces_bord)), np.zeros((self.Nf, self.nb_faces_bord))
        div = np.zeros((self.Nc, self.Nf))
        arr = [[] for k in range(self.Nf)] # juste pour visualiser les points fantomes
        arr_I = { }                        # ... et les interpolations
        # localisations scalaires (CG elements + CG faces de bord)
        loc = ml.DataArrayDouble.Aggregate([self.bary_s, self.decale.getPartBarycenterAndOwner(ml.DataArrayInt(self.faces_bord, len(self.faces_bord), 1))]).toNumPyArray().tolist()
        nghost = sum([len(self.facesIds(i)) for i in range(0, self.Nc)]); ninter = 0
        for i in range(self.Nc):
            #liste de points pour l'interpolation : [ [cellule], [voisins / faces de bords], [v/fb des voisins]... ]
            pts = [[i]]; coeff_i = {}
            for nface, [s, f] in enumerate(self.facesIds(i)):
                div[i][f] = s
                sys.stderr.write("\rinterpolation %d/%d : "%(ninter, nghost))
                ghost = self.get_ghost(i, f); gloc = np.array(ghost.getValues())
                if i not in self.scalaire.getCellsContainingPoint(ghost, 1.0e-12):
                    raise Exception("interpolation : point fantome (%f,%f) hors cellule %d!"%(gloc[0], gloc[1], i))
                arr[f].append(ghost.getValues())
                if len(self.cellsIds(f))==1: arr[f].append(self.bary_f[f].getValues())
                n2m, idxm, am, rk = 1.0e8 if (gloc - loc[i]).dot(gloc - loc[i]) > 1e-8 else 0., [ i ], [ 1.0 ], 0

                #while (n2m > 1.0e-8 and (rk == 0 or n2m < n2p)): # boucle d'extension du voisinage
                while (n2m == 1.0e8): # boucle d'extension du voisinage
                    sys.stderr.write("v"); rk += 1; n2p = n2m
                    if (len(pts) <= rk): # ajout des nouveaux voisins
                        faces = [l for sl in pts for k in sl for (_,l) in (self.facesIds(k) if k < self.Nc else [])]
                        pts.append([self.Nc + g for g in range(self.nb_faces_bord) if (self.faces_bord[g] in faces and not any(self.Nc + g in sl for sl in pts))])
                        pts[rk].extend([c for g in faces for c in self.cellsIds(g) if not any(c in sl for sl in pts)])
                    if (pts[rk] == []): break # pas de nouveaux voisins!

                    # recherche d'interpolation par des points de (pts[0], ..., pts[rk]) x pts[rk]
                    for (j, k) in [(j, k) for k in pts[rk] for r in range(0, rk + 1) for j in pts[r] if (r < rk or j < k)]:
                        idx = [i, j, k];
                        # systeme lineaire pour obtenir les coordonnees barycentriques
                        mat = np.array([loc[n]+[1.0] for n in idx]).transpose()
                        if (abs(np.linalg.det(mat)) < 1.0e-8): continue # triangle degenere -> ko
                        a = np.linalg.solve(mat, np.array(ghost.getValues()+[1.0]))
                        if (min(a) < -1.0e-8 or max(a) > 1.0 + 1.0e-8 or a[0] < 1e-8): continue # ghost hors du triangle -> ko
                        n2 = sum([a[n] * (gloc - loc[idx[n]]).dot(gloc - loc[idx[n]]) for n in range(3)])
                        if n2 < n2m - 1.0e-8: n2m, idxm, am = n2, idx, a

                if (n2m == 1.0e8): raise Exception("interpolation : pas de candidat pour (%f,%f) (maille %d)!"%(gloc[0], gloc[1], i))
                norm = sum([x for x in am if x > 1.0e-8])
                coeff_i[f] = [(j < self.Nc, j if j < self.Nc else self.faces_bord[j - self.Nc], am[n] / norm) for n, j in enumerate(idxm) if am[n] > 1.0e-8]
                arr_I[(i, f)] = [(loc[j], ghost.getValues()) for n, j in enumerate(idxm) if am[n] > 1.0e-8]
                ninter += 1
                for (iscell, cell, coef) in coeff_i[f]:
                    if iscell:
                        if s > 0.: stencil_ghost1[f][cell] += coef
                        else:      stencil_ghost2[f][cell] += coef
                    else:
                        if s >0.: stenb1[f][self.n_fb[cell]] -= coef
                        else:     stenb2[f][self.n_fb[cell]] -= coef
            coefficients.append(coeff_i)

        for f in self.faces_bord: stenb1[f][self.n_fb[f]] += 1.0

        f2c, stencil1, stencil2 = np.zeros((2 * self.Nc, self.Nf)), np.zeros((self.Nf, self.Nc)), np.zeros((self.Nf, self.Nc))
        for f in range(self.Nf):
            S_ij, x_ij = self.sec.getArray()[f], self.bary_f[f]
            for [s, c] in self.cellsIds_s(f):
                if s > 0.: stencil1[f][c] = 1.0
                else:      stencil2[f][c] = 1.0
                x_i, V_i = self.bary_s[c], self.vol.getArray()[c]
                for i in [0, 1]: f2c[2 * c + i][f] = -s * S_ij * (x_ij - x_i).getValues()[i] / V_i
        self.v_f2c = sparse.csr_matrix(f2c)

        self.coefficients = coefficients
        surf = np.array([self.sec.getArray().getValues(),] * self.Nc)
        dist = np.tile(np.transpose(np.tile(self.distD.getArray().toNumPyArray(), (1, 1))), (1, self.Nc))
        self.div = sparse.csr_matrix(surf * div)
        self.stencil1, self.stencil2 = sparse.csr_matrix(stencil1), sparse.csr_matrix(stencil2)
        self.stencil_g1, self.stencil_g2 = sparse.csr_matrix(stencil_ghost1), sparse.csr_matrix(stencil_ghost2)
        self.grad_g = sparse.csr_matrix((stencil_ghost1 - stencil_ghost2) / dist)
        self.diff   = self.div.dot(self.grad_g)
        dist = np.tile(np.transpose(np.tile(self.distD.getArray().toNumPyArray(), (1, 1))), (1, self.nb_faces_bord))
        surf = np.array([self.sec.getArray().getValues(),] * self.nb_faces_bord).transpose()
        self.interpb    = sparse.csr_matrix((stenb1 - stenb2) * surf)
        self.diff_b = sparse.csr_matrix(self.div.dot(sparse.csr_matrix((stenb1 - stenb2) / dist)))

        # pour visualiser les points fantomes...
        pts_fantomes = ml.MEDCouplingUMesh("fantome",1)
        pts_fantomes.allocateCells(self.Nf)
        coo = []
        for i, sommets in enumerate(arr):
            pts_fantomes.insertNextCell(ml.NORM_SEG2, [2 * i, 2 * i + 1])
            coo.extend(sommets[0]); coo.extend(sommets[1])
        coords = ml.DataArrayDouble(coo, self.Nf*2, 2)
        pts_fantomes.setCoords(coords)
        pts_fantomes.finishInsertingCells()
        #ml.MEDLoader.WriteUMesh("output.med", pts_fantomes, False)

        # ... et les choix d'interpolation
        pts_interp   = ml.MEDCouplingUMesh("interp",1)
        nb = sum([len(v) for k, v in arr_I.items()])
        pts_interp.allocateCells(nb)
        coo_I = []; i = 0
        for k, v in arr_I.items():
            for (x, y) in v:
                pts_interp.insertNextCell(ml.NORM_SEG2, [2 * i, 2 * i + 1])
                coo_I.extend(x); coo_I.extend(y)
                i+=1
        coords_I = ml.DataArrayDouble(coo_I, 2 * nb, 2)
        pts_interp.setCoords(coords_I)
        pts_interp.finishInsertingCells()
        #ml.MEDLoader.WriteUMesh("output.med", pts_interp, False)

        print("\rinterpolation : ok                     ", file=sys.stderr)

        mat_cl_hyd = np.zeros((self.nb_faces_bord, 2 * self.Nc + self.Nf + 2 * self.nb_faces_bord))
        mat_cl_T   = np.zeros((self.nb_faces_bord, 2 * self.Nc + self.Nf + 2 * self.nb_faces_bord))
        # conditions limites
        for i, f in enumerate(self.faces_bord):
            [c] = self.cellsIds(f)
            # hydrau
            id = self.n_fb[f]
            if self.cl[f]["v"] is not None: mat_cl_hyd[i, 2 * self.Nc + f] = 1.0
            else: mat_cl_hyd[i, 2 * self.Nc + self.Nf + i] = 1.0
            # temperature
            mat_cl_T[i, 2 * self.Nc + self.Nf + self.nb_faces_bord + i] = -(1.0 / self.distD.getArray()[f] + self.cl[f]["h"])
            for (isCell, cell, coef) in self.coefficients[c][f]:
                if isCell: mat_cl_T[i, self.Nc + cell] += coef / self.distD.getArray()[f]
                else: mat_cl_T[i, 2 * self.Nc + self.Nf + self.nb_faces_bord + self.n_fb[cell]] += coef / self.distD.getArray()[f]
        self.mat_clHyd, self.mat_clT = sparse.csr_matrix(mat_cl_hyd), sparse.csr_matrix(mat_cl_T)


    def calc_volD(self):
        arr, arr1, arr2 = [], [], []
        for i in range(self.Nf):
            dist1, dist2 = 0.0, 0.0
            for [s, c] in self.cellsIds_s(i):
                if s > 0.: dist1 = (self.bary_f[i] - self.get_ghost(c, i)).norm2()
                else:      dist2 = (self.bary_f[i] - self.get_ghost(c, i)).norm2()
            arr1.append(dist1 if dist1 > 0. else float('inf')) ; arr2.append(dist2 if dist2 > 0. else float('inf'))
            arr.append(dist1 + dist2)
        arr1 = ml.DataArrayDouble(arr1, self.Nf, 1)
        arr2 = ml.DataArrayDouble(arr2, self.Nf, 1)
        arr  = ml.DataArrayDouble(arr , self.Nf, 1)
        dist1 = self.decale.fillFromAnalytic(ml.ON_CELLS,1,"0")
        dist1.setName("dist1") ; dist1.setArray(arr1)
        dist2 = self.decale.fillFromAnalytic(ml.ON_CELLS,1,"0")
        dist2.setName("dist2") ; dist2.setArray(arr2)
        distD = self.decale.fillFromAnalytic(ml.ON_CELLS,1,"0")
        distD.setName("dist") ; distD.setArray(arr)
        volD = distD.clone(True) ; volD.setName("volume controle decale")
        volD *= self.sec
        print("Volume maillage primal :", self.vol.accumulate()[0])
        print("Volume maillage dual   :", volD.accumulate()[0] * 0.5)
        if abs(self.vol.accumulate()[0] -volD.accumulate()[0] * 0.5)>1e-6: raise Exception("Le maillage dual n'est pas bien construit")
        return distD, dist1, dist2, volD

    def compute_upwind_scalar(self, field, v):
        field_upwind = v.clone(True)
        field_upwind.applyLin(0., 1000.)
        field_upwind.setName("%s upwind"%(field.getName()))
        return field_upwind
