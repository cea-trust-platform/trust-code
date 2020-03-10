import medcoupling as ml
from Maillage import Maillage
import numpy as np
from scipy import sparse
from scipy.sparse.linalg import dsolve
from scipy.linalg import norm
from math import sqrt
from eos import compute_rho
from equations import EquationSystem
import sys
import time
sign = lambda a : float((a>0) - (a<0))

np.set_printoptions(suppress=True, linewidth=300, precision=2, threshold=1e10)
def sortie_med(list_field, t, io, outfile):
    for field in list_field:
        field.setTime(t, io, 0)
        ml.WriteFieldUsingAlreadyWrittenMesh(outfile, field)
    return io + 1

def solve(mcum, out_file, dic_CL, eqs, mesh, vn, P, T, facsec, dt_max):

    #1/ conditions aux limites
    P_imp = False
    for key, [pos, type, val, hval, Tval] in dic_CL.items():
        if type == "P": P_imp = True
    mesh.setCL(dic_CL)

    # 2/ mise en place du processus d'interpolation
    mesh.calc_coefficients()
    mesh.init_f2f()
    vf = mesh.f2f3D(vn)
    vc = mesh.faces2cell3D(vn)

    rho = compute_rho(T, P)
    rhoD = mesh.compute_upwind_scalar(rho, vn)

    # 3/ sauvegarde des conditions initiales
    dt_init = 1.0
    for f in range(mesh.Nf):
        if vn.getArray()[f]: dt_init = min(dt_init, mesh.distD.getArray()[f] / abs(vn.getArray()[f]))
    t, io, dt_init = 0.0, 0, facsec * min(dt_init, dt_max)
    io = sortie_med([vc, vn, vf, P, T], t, io, out_file)

    variables = {"P" : P, "rho" : rho, "rhoD" : rhoD, "v" : vn, "vm" : vn, "T": T, "Tm" : T}
    NS = EquationSystem(mesh, eqs, variables, dt_init)
    # variables du newton : P | T | vn | Pbord | Tbord
    print("  Time  |  dt    | itN |residual | steady  | CPU t", file=sys.stderr)
    print("--------+--------+-----+---------+---------+--------", file=sys.stderr)

    # 4/ boucle en temps
    steady_res = 1.e30
    it = 0
    while (steady_res > 1e-6 and NS.dt > 1e-6):
        ta = time.clock()
        iter, res = 0, 1.0e30
        T0  =  T.clone(True)
        P0  =  P.clone(True)
        vn0 = vn.clone(True)

        # 5/ iterations du newton
        while(iter < 40 and res > 1.0e-5):

            variables = {"P" : P, "rho" : rho, "rhoD" : rhoD, "v" : vn, "vm" : vn0, "T": T, "Tm" : T0}

            NS.upd_mat(variables)
            NS.upd_res(variables)

            # conditions limites
            for i, f in enumerate(mesh.faces_bord):
                S_ij, A_ij = mesh.sec.getArray()[f], mesh.distD.getArray()[f]
                # hydrau
                if mesh.cl[f]["v"] is not None:
                    vn_cl = ml.DataArrayDouble.Dot(ml.DataArrayDouble(mesh.cl[f]["v"], 1, 2), mesh.normal.getArray()[f]).getValues()[0]
                    NS.res[2 * mesh.Nc + mesh.Nf + i] = -vn.getArray()[f] + vn_cl
                else: NS.res[2 * mesh.Nc + mesh.Nf + i] = -mesh.tab_bord["P"][i] + mesh.cl[f]["P"]

                # temperature
                [c] = mesh.cellsIds(f)
                dTdn = 0.
                for (isCell, cell, coef) in mesh.coefficients[c][f]:
                    if isCell: inc_val = T.getArray().getValues()[cell]
                    else: inc_val = mesh.tab_bord["T"][mesh.n_fb[cell]]
                    dTdn += coef * inc_val
                dTdn -= mesh.tab_bord["T"][i]
                dTdn /= A_ij
                NS.res[2 * mesh.Nc + mesh.Nf + mesh.nb_faces_bord + i] = 0.0#dTdn + mesh.cl[f]["h"] * ( mesh.cl[f]["T"] - mesh.tab_bord["T"][i])

            if not P_imp: NS.mat[0, 0], NS.res[0] = 1.0, 1.0e5 - P.getArray()[0]

            delta = dsolve.spsolve(NS.mat, NS.res.A1)
            res = norm(delta, np.inf)

            DParr = ml.DataArrayDouble(delta[:mesh.Nc].tolist(), mesh.Nc, 1)
            DTarr = ml.DataArrayDouble(delta[mesh.Nc:2*mesh.Nc].tolist(), mesh.Nc, 1)
            Dvarr = ml.DataArrayDouble(delta[2*mesh.Nc:2*mesh.Nc+mesh.Nf].tolist(), mesh.Nf, 1)
            Parr, Tarr, varr = P.getArray(), T.getArray(), vn.getArray()
            Parr += DParr ; P.setArray(Parr)
            Tarr += DTarr ; T.setArray(Tarr)
            varr += Dvarr ; vn.setArray(varr)

            # maj des tableaux faces bord
            DParr = ml.DataArrayDouble(delta[2*mesh.Nc+mesh.Nf:2*mesh.Nc+mesh.Nf+mesh.nb_faces_bord].tolist(), mesh.nb_faces_bord, 1)
            Parr  = ml.DataArrayDouble(mesh.tab_bord["P"], mesh.nb_faces_bord, 1)
            Parr += DParr ; mesh.tab_bord["P"] = Parr.getValues()

            DTarr = ml.DataArrayDouble(delta[2*mesh.Nc+mesh.Nf+mesh.nb_faces_bord:].tolist(), mesh.nb_faces_bord, 1)
            Tarr  = ml.DataArrayDouble(mesh.tab_bord["T"], mesh.nb_faces_bord, 1)
            Tarr += DTarr ; mesh.tab_bord["T"] = Tarr.getValues()
            sys.stderr.write("\r%5.2f s | %3d | %.1e"%(t, iter, res))

            iter += 1 ; init = False

        t += NS.dt
        steady_res = max((P-P0).normMax(), max((T-T0).normMax(), (vn-vn0).normMax()))
        print("\r%5.2f s | %5.2f s | %3d | %.1e | %.1e | %4.2f"%(t, NS.dt, iter, res, steady_res, time.clock() - ta), file=sys.stderr)
        vc = mesh.faces2cell3D(vn)
        vf = mesh.f2f3D(vn)
        io = sortie_med([vc, vn, vf, P, T], t, io, out_file)
        it += 1
        dt = 1.0e30
        for f in range(mesh.Nf):
            if vn.getArray()[f]: dt = min(dt, mesh.distD.getArray()[f] / abs(vn.getArray()[f]))
        NS.upd_dt(min(dt_max, facsec * dt))

    print("termine!")

    fic = open("pression.txt",'w')
    for i in range(mesh.Nc): fic.write("%f %f\n"%(mesh.scalaire.computeCellCenterOfMass()[i].getValues()[1], P.getArray()[i]))
    fic.close()
    fic = open("temperature.txt",'w')
    for i in range(mesh.Nc): fic.write("%f %f\n"%(mesh.scalaire.computeCellCenterOfMass()[i].getValues()[1], T.getArray()[i]))
    fic.close()
    fic = open("vitesse.txt",'w')
    for i in range(mesh.Nf): fic.write("%f %f %f\n"%(mesh.decale.computeCellCenterOfMass()[i].getValues()[1],
                                                     vf.getArray()[i].getValues()[0], vf.getArray()[i].getValues()[1]))

    return P, T, vn, vf
