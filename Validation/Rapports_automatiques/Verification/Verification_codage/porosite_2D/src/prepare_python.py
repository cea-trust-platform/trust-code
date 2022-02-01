#!/usr/bin/env python3
# -*-coding:utf-8 -*
from string import Template
from weird_mesh import getWeirdMesh
import os

def build_mesh(type_elem, xmin, xmax, ymin, ymax, nx, ny, path_to_folder):
    maillages = {"cartesian": "o", "triangle": "x", "NCa": "o3", "NCr": "o3"}
    func_mesh = {"cartesian": None, "triangle": None, "NCa": lambda x, y: 5 * int(y > 1 and y < 2), "NCr": lambda x, y: 5 * int(x > 0.8 and x < 1.2)}
    getWeirdMesh(xmin, xmax, ymin, ymax, list(range(nx + 1)), list(range(ny + 1)), maillages[type_elem], unpo=(not type_elem.startswith("NC")), func=func_mesh[type_elem]).write("{}/mesh.med".format(path_to_folder), 2)

if __name__ == "__main__":
    lx, ly = 2, 3
    meshes = [30]
    dic_dis = {"cartesian" : ["VDF", "PolyMAC", "CoviMAC"], "triangle" : ["VEFPreP1B", "PolyMAC", "CoviMAC"], "NCa" : ["PolyMAC", "CoviMAC"], "NCr" : ["PolyMAC", "CoviMAC"]}
    list_dis, list_meshes = [], []
    for k, v in dic_dis.items():
        for d in v:
            list_dis.append(f"{k}/{d}")
            for ny in meshes:
                nx = int(lx / ly * ny)
                list_meshes.append(f"{nx}x{ny}")
                os.system(f"mkdir -p {k}/{d}/maillage_{nx}x{ny}")
                build_mesh(k, 0, lx, 0, ly, nx, ny, f"{k}/{d}/maillage_{nx}x{ny}")
                with open("jdd.data", "r") as file:
                    filedata = Template(file.read())
                result = filedata.substitute({"dis" : d})
                with open(f"{k}/{d}/maillage_{nx}x{ny}/jdd.data", "w") as file:
                    file.write(result)
                run.addCase(f"{k}/{d}/maillage_{nx}x{ny}", "jdd.data")

    with open("extract_convergence", "r") as file:
        fileconv = Template(file.read())
    result = fileconv.safe_substitute(list_dis=" ".join(sorted(list(set(list(list_dis))))))

    with open("extract_convergence", "w") as file:
        file.write(result)
