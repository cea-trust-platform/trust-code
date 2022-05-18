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
    mu, vwall = 2.5, 1
    # assert (lx / ly).is_integer(), "lx must be a multiple of ly!"
    ny = 41
    dic_dis = {"cartesian" : ["VDF", "PolyMAC", "PolyMAC_P0"], "triangle" : ["VEFPreP1B", "PolyMAC", "PolyMAC_P0"], "NCa" : ["PolyMAC", "PolyMAC_P0"], "NCr" : ["PolyMAC", "PolyMAC_P0"]}
    list_dis = []
    for k, v in dic_dis.items():
        for d in v:
            list_dis.append(f"{k}/{d}")
            nx = int(lx / ly * ny)
            os.system(f"mkdir -p {k}/{d}")
            build_mesh(k, 0, lx, 0, ly, nx, ny, f"{k}/{d}")
            dic = {
                "vwall" : vwall,
                "dis" : d,
                # "lx" : lx,
                # "ly" : ly,
                "mu" : mu,
            }

            with open("jdd.data", "r") as file:
                filedata = Template(file.read())
            result = filedata.substitute(dic)
            with open(f"{k}/{d}/jdd.data", "w") as file:
                file.write(result)

