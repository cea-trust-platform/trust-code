import MEDLoader as ml
import numpy as np
from scipy import sparse
from scipy.sparse.linalg import dsolve

class Operateur():
    def __init__(self, mesh, fields, variables, N, upd, dt):
        self.mesh  = mesh
        self.dt  = dt
        self.fields= fields
        self.N     = N
        self.res   = np.zeros( N )
        self.dEdP  = sparse.csr_matrix(np.zeros((N, mesh.Nc)))
        self.dEdT  = sparse.csr_matrix(np.zeros((N, mesh.Nc)))
        self.dEdv  = sparse.csr_matrix(np.zeros((N, mesh.Nf)))
        self.dEdPb = sparse.csr_matrix(np.zeros((N, mesh.nb_faces_bord)))
        self.dEdTb = sparse.csr_matrix(np.zeros((N, mesh.nb_faces_bord)))
        self.upd = upd
        self.mat = None
        self.assembler([variables[f] for f in fields])

    def assembler(self, fields=[]):
        self.mat = sparse.hstack([self.dEdP, self.dEdT, self.dEdv, self.dEdPb, self.dEdTb])

    def upd_mat(self, fields=[]):
        if self.upd:
            self.dEdP  = sparse.csr_matrix(np.zeros((self.N, self.mesh.Nc)))
            self.dEdT  = sparse.csr_matrix(np.zeros((self.N, self.mesh.Nc)))
            self.dEdv  = sparse.csr_matrix(np.zeros((self.N, self.mesh.Nf)))
            self.dEdPb = sparse.csr_matrix(np.zeros((self.N, self.mesh.nb_faces_bord)))
            self.dEdTb = sparse.csr_matrix(np.zeros((self.N, self.mesh.nb_faces_bord)))
            self.assembler(fields)

    def upd_res(self, fields):
        pass

#class Derivee_temps_scal(Operateur):
#
#    def assembler(self, fields):
#        m = self.mesh
#        vol_m = np.array([m.vol.getArray().toNumPyArray(), ] * m.Nc).transpose()
#        self.dEdT = sparse.csr_matrix(sparse.identity(m.Nc, format='csr').multiply(vol_m / dt))
#        self.mat = sparse.hstack([self.dEdP, self.dEdT, self.dEdv, self.dEdPb, self.dEdTb])
#
#    def upd_res(self, fields):
#        [fp, fm], m = fields, self.mesh
#        dv = fp.getArray().toNumPyArray() - fm.getArray().toNumPyArray()
#        vec = np.zeros(2 * m.Nc + m.Nf + 2 * m.nb_faces_bord)
#        vec[m.Nc:2 * m.Nc] = dv
#        self.res = -self.mat.dot(vec)

class Derivee_temps_decale(Operateur):

    def assembler(self, fields):
        [rho, vn, vn0], m = fields, self.mesh
        rho_m, vol_m = np.array([rho.getArray().toNumPyArray(), ] * m.Nf).transpose(), np.array([m.volD.getArray().toNumPyArray(), ] * m.Nf).transpose()
        self.dEdv = sparse.csr_matrix(sparse.identity(m.Nf, format='csr').multiply(rho_m * vol_m / self.dt))
        self.mat = sparse.hstack([self.dEdP, self.dEdT, self.dEdv, self.dEdPb, self.dEdTb])

    def upd_res(self, fields):
        [rho, vn, vn0], m = fields, self.mesh
        dv = vn.getArray().toNumPyArray() - vn0.getArray().toNumPyArray()
        vec = np.zeros(2 * m.Nc + m.Nf + 2 * m.nb_faces_bord)
        vec[2 * m.Nc:2 * m.Nc + m.Nf] = dv
        self.res = -self.mat.dot(vec)

class Conv_decale(Operateur):

    def upd_res(self, fields):
        [vn], m = fields, self.mesh
        vf = np.hstack([vn.getArray().toNumPyArray(), np.array(m.vta)])
        mat = np.zeros((m.Nf, m.Nf + len(m.vta)))
        for f in range(m.Nf):
            [nx, ny] = m.normal.getArray()[f].getValues()
            self.res[f] = -(nx * (m.vx[f,:].dot(vf) * m.dxvx[f,:].dot(vf) + m.vy[f,:].dot(vf) * m.dyvx[f,:].dot(vf))
                          + ny * (m.vx[f,:].dot(vf) * m.dxvy[f,:].dot(vf) + m.vy[f,:].dot(vf) * m.dyvy[f,:].dot(vf)))* 1000. * m.volD.getArray()[f]


class Diff_decale(Operateur):
    def assembler(self, fields):

        mu, m = 2.5, self.mesh
        mat = np.zeros((m.Nf, m.Nf + len(m.vta)))
        for f in range(m.Nf): mat[f, :] += -2.0 * mu * m.laplacien_face(f) * m.volD.getArray()[f]
        self.dEdv = sparse.csr_matrix(mat[:, :m.Nf])
        self.mat = sparse.hstack([self.dEdP, self.dEdT, self.dEdv, self.dEdPb, self.dEdTb])
        self.mat_r = sparse.csr_matrix(mat)

    def upd_res(self, fields):
        [vn], m = fields, self.mesh
        vf = np.hstack([vn.getArray().toNumPyArray(), np.array(m.vta)])
        self.res = -self.mat_r.dot(vf)


class GradP(Operateur):

    def assembler(self, fields):
        m = self.mesh
        surf = np.array([m.sec.getArray().getValues(),] * m.Nc).transpose()
        self.dEdP  = -(m.stencil_g1 - m.stencil_g2).multiply(surf)
        self.dEdPb = m.interpb
        self.mat = sparse.hstack([self.dEdP, self.dEdT, self.dEdv, self.dEdPb, self.dEdTb])

    def upd_res(self, fields):
        m = self.mesh
        [P_mcfd] = fields
        P = P_mcfd.getArray().toNumPyArray()
        vec = np.zeros(2 * m.Nc + m.Nf + 2 * m.nb_faces_bord)
        vec[:m.Nc] = P
        vec[2 * m.Nc + m.Nf:2 * m.Nc + m.Nf + m.nb_faces_bord] = m.tab_bord["P"]
        self.res = -self.mat.dot(vec)

class Conv_scal(Operateur):

    def assembler(self, fields):
        [phi_mcfd, v], m = fields, self.mesh
        phi  = np.array([phi_mcfd.getArray().getValues(),] * m.Nc)
        self.dEdv = m.div.multiply(phi)
        self.mat = sparse.hstack([self.dEdP, self.dEdT, self.dEdv, self.dEdPb, self.dEdTb])

    def upd_res(self, fields):
        [rho, v_mcfd], m = fields, self.mesh
        v = v_mcfd.getArray().toNumPyArray()
        vec = np.zeros(2 * m.Nc + m.Nf + 2 * m.nb_faces_bord)
        vec[2 * m.Nc:2 * m.Nc + m.Nf] = v
        self.res = -self.mat.dot(vec)

class Diff_scal(Operateur):

    def assembler(self, fields):
        # TODO : dans la matrice, mettre Pb a cote de P, et Tb a cote de T, et faire qu'une matrice de div grad et diff
        m = self.mesh
        self.dEdT  = -m.diff
        self.dEdTb = m.diff_b
        self.mat = sparse.hstack([self.dEdP, self.dEdT, self.dEdv, self.dEdPb, self.dEdTb])

    def upd_res(self, fields):
        [T_mcfd], m = fields, self.mesh
        T = T_mcfd.getArray().toNumPyArray()
        vec = np.zeros(2 * m.Nc + m.Nf + 2 * m.nb_faces_bord)
        vec[m.Nc:2 * m.Nc] = T
        vec[2 * m.Nc + m.Nf + m.nb_faces_bord:2 * m.Nc + m.Nf + 2 * m.nb_faces_bord] = m.tab_bord["T"]
        self.res = -self.mat.dot(vec)

class Gravite(Operateur):

    def upd_res(self, fields):
        m, [rho], g_ = self.mesh, fields, ml.DataArrayDouble([0.0, -9.81], 1, 2)
        self.res   = np.zeros( m.Nf )
        for f in range(m.Nf):
            g, vol = ml.DataArrayDouble.Dot(g_, m.normal.getArray()[f]), m.volD.getArray()[f]
            self.res[f] = rho.getArray()[f] * vol * g[0]
