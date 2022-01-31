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
    # assert (lx / ly).is_integer(), "lx must be a multiple of ly!"
    meshes = [6, 12, 24, 36]
    # dic_dis = {"cartesian" : ["VDF", "PolyMAC", "CoviMAC"]}
    solveur = {"VDF" : "solveur implicite { solveur petsc cholesky { } }",
    "VEFPreP1B" : "solveur implicite { solveur petsc cholesky { } }",
    "CoviMAC" : "solveur solveur_U_P { solveur petsc cholesky { cli { -mat_mumps_icntl_23 2000 } } }",
    "PolyMAC" : "solveur solveur_U_P { solveur petsc cholesky { cli { -mat_mumps_icntl_23 2000 } } }",
    }
    dic_dis = {"cartesian" : ["VDF", "PolyMAC", "CoviMAC"], "triangle" : ["VEFPreP1B", "PolyMAC", "CoviMAC"], "NCa" : ["PolyMAC", "CoviMAC"], "NCr" : ["PolyMAC", "CoviMAC"]}
    list_dis, list_meshes = [], []
    for k, v in dic_dis.items():
        for d in v:
            list_dis.append("{}/{}".format(k, d))
            for ny in meshes:
                nx = int(lx / ly * ny)
                list_meshes.append("{}x{}".format(nx, ny))
                os.system("mkdir -p {}/{}/maillage_{}x{}".format(k, d, nx, ny))
                build_mesh(k, 0, lx, 0, ly, nx, ny, "{}/{}/maillage_{}x{}".format(k, d, nx, ny))
                dic = {
                    "vwall" : vwall,
                    "dis" : d,
                    "lx" : lx,
                    "ly" : ly,
                    "pin" : pin,
                    "pout" : pout,
                    "mu" : mu,
                    "solveur" : solveur[d]
                }

                with open("jdd.data", "r") as file:
                    filedata = Template(file.read())
                result = filedata.substitute(dic)
                with open("{}/{}/maillage_{}x{}/jdd.data".format(k, d, nx, ny), "w") as file:
                    file.write(result)

    with open("extract_convergence", "r") as file:
        fileconv = Template(file.read())
    result = fileconv.safe_substitute(list_dis=" ".join(sorted(list(set(list(list_dis))))))

    with open("extract_convergence", "w") as file:
        file.write(result)
