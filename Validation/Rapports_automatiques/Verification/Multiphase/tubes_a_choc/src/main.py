# -*-coding:utf-8 -*

######################################################################################################################
#   This file runs a set of Riemann problems for the one dimensional Euler equations with stiffened gas equation of state.
#   It uses the class exact_rs_stiffenedgas to compute the exact solution of the Riemann Problems.
#
#   Author: Michael Ndjinga
#   Date:   19/02/2021
#   Description : Translated from C++ package developped by Murray Cutforth
#######################################################################################################################

import exact_rs_stiffenedgas
from math import fabs
import os


def stiffenedgas_e(rho, p, gamma, pinf):
    return (p + gamma * pinf) / (rho * (gamma - 1))


def stiffenedgas_h(rho, p, gamma, pinf):
    return gamma * (p + pinf) / (rho * (gamma - 1))


def run_Riemann_problem(TC, name, numsamples=100):
    # Output test solution for many different Riemann problems

    #print("")
    #print(
    #    "Determination of the exact solution of a Riemann problem for the Euler equations on "
    #    + str(numsamples)
    #    + " sample points."
    #)

    xmin = 0.0
    xmax = 1.0

    RS = exact_rs_stiffenedgas.exact_rs_stiffenedgas(TC["gamma"], TC["gamma"], TC["pinf"], TC["pinf"])
    RS.solve_RP(TC["WL"], TC["WR"])

    #print("")
    #print("Solved Riemann problem for TC = ", name)
    #print("Star state pressure calculated as ", RS.P_STAR)
    #print("Star state velocity calculated as ", RS.S_STAR)
    #print("Left star state density calculated as ", RS.rho_star_L)
    #print("Right state state density calculated as ", RS.rho_star_R)
    #print("Left shock speed calculated as ", RS.S_L)
    #print("Right shock speed calculated as ", RS.S_R)
    #print("Left rarefaction head speed calculated as ", RS.S_HL)
    #print("Left rarefaction tail speed calculated as ", RS.S_TL)
    #print("Right rarefaction head speed calculated as ", RS.S_HR)
    #print("Right rarefaction tail speed calculated as ", RS.S_TR)

    delx = (xmax - xmin) / numsamples

    outfile = open("{}.ex".format(name), "w")
    outfile.write("x               RHO                 V                 P              EINT\n")

    t = TC["tmax"]
    offset = TC["x"]
    x = xmin
    maxc = 0
    while x <= xmax:
        S = x / t
        soln = RS.sample_solution(TC["WL"], TC["WR"], S - offset / t)
        thisgamma = TC["gamma"]
        thispinf = TC["pinf"]
        thisz = 1.0 if S - offset / t < RS.S_STAR else 0.0
        outfile.write(
            str(x)
            + " "
            + str(soln[0])
            + " "
            + str(soln[1])
            + " "
            + str(soln[2])
            + " "
            + str(stiffenedgas_e(soln[0], soln[2], thisgamma, thispinf))
            + " "
            + str(stiffenedgas_h(soln[0], soln[2], thisgamma, thispinf))
            + " "
            + str(fabs(soln[1]) / RS.a(soln[0], soln[2], thisgamma, thispinf))
            + " "
            + str(thisz)
            + "\n"
        )
        maxc = max(maxc, fabs(soln[1] + RS.a(soln[0], soln[2], thisgamma, thispinf)))
        x += delx
    outfile.close()
    return maxc


if __name__ == "__main__":

    TCs = {
        "Toro1": {"WL": [1, 0.75, 1], "WR": [0.125, 0, 0.1], "tmax": 0.2, "gamma": 1.4, "pinf": 0, "x": 0.3},
        "Toro2": {"WL": [1, -2, 0.4], "WR": [1, 2, 0.4], "tmax": 0.15, "gamma": 1.4, "pinf": 0, "x": 0.5},
        "Toro3": {"WL": [1, 0, 1000], "WR": [1, 0, 0.01], "tmax": 0.012, "gamma": 1.4, "pinf": 0, "x": 0.5},
        "Toro4": {
            "WL": [5.99924, 19.5975, 460.894],
            "WR": [5.99242, -6.19633, 46.0950],
            "tmax": 0.035,
            "gamma": 1.4,
            "pinf": 0,
            "x": 0.4,
        },
        "Toro5": {
            "WL": [1, -19.5975, 1000],
            "WR": [1, -19.59745, 0.01],
            "tmax": 0.012,
            "gamma": 1.4,
            "pinf": 0,
            "x": 0.8,
        },
        "Toro6": {"WL": [1.4, 0, 1], "WR": [1.4, 0, 1], "tmax": 2, "gamma": 1.4, "pinf": 0, "x": 0.5},
        "Toro7": {"WL": [1.4, 0.1, 1], "WR": [1, 0.1, 1], "tmax": 2, "gamma": 1.4, "pinf": 0, "x": 0.5},
        "PWR1": {
            "WL": [700, 0, 155e5],
            "WR": [700, 0, 1e5],
            "tmax": 3e-4,
            "gamma": 1.58,
            "pinf": 353637173.0,
            "x": 0.5,
        },
        "PWR2": {
            "WL": [700, 0, 155e5],
            "WR": [700, 20, 155e5],
            "tmax": 3e-4,
            "gamma": 1.58,
            "pinf": 353637173.0,
            "x": 0.5,
        },
        "PWR3": {
            "WL": [700, 0, 155e5],
            "WR": [650, 0, 155e5],
            "tmax": 3e-4,
            "gamma": 1.58,
            "pinf": 353637173.0,
            "x": 0.5,
        },
    }
    meshes = [100, 200, 400, 800]
    for n in meshes:
        os.system("mkdir n{}".format(n))

    columns = ["$$\gamma$$", "$$p_\infty$$", "$$\rho_L$$", "$$u_L$$", "$$p_L$$", "$$\rho_R$$", "$$u_R$$", "$$p_R$$","$$x_d$$"]
    tab = plot.Table(columns)
    for name in ["Toro1", "Toro2", "Toro3", "Toro4", "Toro5", "Toro6", "Toro7", "PWR1", "PWR2", "PWR3"]:

        with open("jdd.data", "r") as file:
            filedata = file.read()

        # Replace the target string
        for i, n in enumerate(["__rl__", "__vl__", "__pl__"]):
            filedata = filedata.replace(n, str(TCs[name]["WL"][i]))
        for i, n in enumerate(["__rr__", "__vr__", "__pr__"]):
            filedata = filedata.replace(n, str(TCs[name]["WR"][i]))
        filedata = filedata.replace(
            "__Tl__", str((TCs[name]["WL"][2] + TCs[name]["pinf"]) / (TCs[name]["WL"][0] * 8.31446261815324) -273.15 )
        )
        filedata = filedata.replace(
            "__Tr__", str((TCs[name]["WR"][2] + TCs[name]["pinf"]) / (TCs[name]["WR"][0] * 8.31446261815324) -273.15 )
        )
        
        tab.addLigne( [[TCs[name]["gamma"], TCs[name]["pinf"]] + TCs[name]["WL"] + TCs[name]["WR"] + [TCs[name]["x"]]], name)

        for n in ["tmax", "gamma", "pinf", "x"]:
            filedata = filedata.replace("__{}__".format(n), str(TCs[name][n]))

        c = run_Riemann_problem(TCs[name], name, 1000)

        # Write the file out again
        for n in meshes:
            dt = TCs[name]["tmax"] / n
            # dt = 0.1 * 1.0 / n / c
            filedata_ = filedata.replace("__n__", str(n + 1))
            filedata_ = filedata_.replace("__dt__", str(dt))
            with open("n{}/{}.data".format(n, name), "w") as file:
                file.write(filedata_)
