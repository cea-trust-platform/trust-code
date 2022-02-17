#!/usr/bin/env python3
# -*-coding:utf-8 -*
from string import Template
from weird_mesh import getWeirdMesh
import os

def build_mesh(type_elem, xmin, xmax, ymin, ymax, nx, ny, path_to_folder):
    maillages = {"cartesian": "o", "triangle": "x", "NCa": "o3", "NCr": "o3"}
    func_mesh = {"cartesian": None, "triangle": None, "NCa": lambda x, y: 5 * int(y > 0.4 and y < 0.6), "NCr": lambda x, y: 5 * int(x > 0.4 and x < 0.6)}
    getWeirdMesh(xmin, xmax, ymin, ymax, list(range(nx + 1)), list(range(ny + 1)), maillages[type_elem], unpo=(not type_elem.startswith("NC")), func=func_mesh[type_elem]).write("{}/mesh.med".format(path_to_folder), 2)

if __name__ == "__main__":
    lx, ly = 1, 1
    pin, pout = 400, 100
    mu, vwall = 2.5, 1
    ny = 41
    implicit = """Schema_euler_implicite sch Read sch
    {
        tinit 0
        dt_max 1
        facsec 1e30
        seuil_statio 1e-5
        solveur implicite { solveur petsc cholesky { } }
    }"""
    explicit = """Schema_euler_explicite sch Read sch
    {
        tinit 0
        dt_max 0.1
        dt_start dt_fixe 1e-4
        facsec 1
        seuil_statio 1e-5
    }"""
    uzawa = "implicite solveur petsc Cholesky { impr }"
    fields = {"const" : f"champ_uniforme 1 {mu}", "var" : f"champ_fonc_xyz dom 1 {mu}"}
    scheme = {"explicit" : [explicit, ""], "implicit" : [implicit, ""], "uzawa" : [explicit, uzawa]}
    l = []
    for kf, vf in fields.items():
        for ks, vs in scheme.items():
            l.append(f"{kf}/{ks}")
            nx = int(lx / ly * ny)
            os.system(f"mkdir -p {kf}/{ks}")
            build_mesh("cartesian", 0, lx, 0, ly, nx, ny, f"{kf}/{ks}")
            dic = {
                "vwall" : vwall,
                "lx" : lx,
                "ly" : ly,
                "pin" : pin,
                "pout" : pout,
                "mu" : mu,
                "scheme" : vs[0],
                "uzawa" : vs[1]
            }

            with open("jdd.data", "r") as file:
                filedata = Template(file.read())
            result = filedata.substitute(dic)
            with open(f"{kf}/{ks}/jdd.data", "w") as file:
                file.write(result)

