import medcoupling as ml
from make_mesh import getCartesianMesh
from Maillage import Maillage
import numpy as np
from scipy import sparse
from scipy.sparse.linalg import dsolve
from scipy.linalg import norm
from math import sqrt
from operateurs import *
import time
sign = lambda a : float((a>0) - (a<0))


class Equation():
    def __init__(self, mesh, loc, termes, variables, dt):
        self.loc = loc
        self.dt = dt
        self.mesh = mesh
        self.ninc = mesh.Nc if loc == "scalaire" else mesh.Nf
        self.operateurs = []
        for (upd, o, fields) in termes:
            ta = time.clock()
            self.operateurs.append(globals()[o](self.mesh, fields, variables, self.ninc, upd, dt))
            print("Operateur %6s : %5.2f s"%(o, time.clock() - ta))
        self.res = np.zeros((self.ninc, 1))
        self.build_mat()

    def upd_dt(self, dt):
        self.dt = dt
        for op in self.operateurs:
            op.dt = dt

    def upd_mat(self, variables):
        change = False
        for op in self.operateurs:
            if op.upd: change = True
            op.upd_mat([variables[f] for f in op.fields])
        if change: self.build_mat()
        return change

    def upd_res(self, variables):
        self.res = np.zeros((self.ninc, 1))
        for op in self.operateurs:
            op.upd_res([variables[f] for f in op.fields])
            self.res += op.res.reshape((self.ninc, 1))

    def build_mat(self):
        self.mat = sparse.csr_matrix(np.zeros((self.ninc, 2 * self.mesh.Nc + self.mesh.Nf + 2 * self.mesh.nb_faces_bord)))
        for i, op in enumerate(self.operateurs): self.mat += op.mat

class EquationSystem():
    def __init__(self, mesh, eqs, variables, dt_init):
        self.equations = []
        self.dt = dt_init
        self.mesh = mesh
        for [loc, terms] in eqs: self.equations.append(Equation(mesh, loc, terms, variables, dt_init))
        self.mat = sparse.vstack([eq.mat for eq in self.equations] + [mesh.mat_clHyd, mesh.mat_clT])
        self.res = np.bmat([[eq.res] for eq in self.equations] + [[np.zeros((mesh.nb_faces_bord, 1))], [np.zeros((mesh.nb_faces_bord, 1))]])

    def upd_dt(self, dt):
        self.dt = dt
        for eq in self.equations: eq.upd_dt(dt)

    def upd_mat(self, variables):
        change = False
        for eq in self.equations:
            change_eq = eq.upd_mat(variables)
            eq.upd_res(variables)
            if change_eq: change = True
        self.mat = sparse.vstack([eq.mat for eq in self.equations] + [self.mesh.mat_clHyd, self.mesh.mat_clT])

    def upd_res(self, variables):
        for eq in self.equations: eq.upd_res(variables)
        self.res = np.bmat([[eq.res] for eq in self.equations] + [[np.zeros((self.mesh.nb_faces_bord, 1))], [np.zeros((self.mesh.nb_faces_bord, 1))]])
