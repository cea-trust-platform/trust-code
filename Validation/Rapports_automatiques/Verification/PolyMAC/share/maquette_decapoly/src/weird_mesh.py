
import MEDLoader as ml
from math import pi, cos, sin


def getWeirdMesh(xmin, xmax, ymin, ymax, lx, ly, str, func=None, extr=-1, unpo=False):
    polys = []; ic = 0
    for iy, y in enumerate(ly[:-1]):
        for ix, x in enumerate(lx[:-1]):
            ic += 1; dx = lx[ix + 1] - x; dy = ly[iy + 1] - y
            c = str[ic % len(str)] if func is None else str[int(min(max(func(xmin + (xmax - xmin) * (x + dx / 2 - lx[0]) / (lx[-1] - lx[0]),
                                                                             ymin + (ymax - ymin) * (y + dy / 2 - ly[0]) / (ly[-1] - ly[0])), 0), len(str) - 1))]
            if c == "-":  # deux rectangles (horizontale)
                polys.extend([[[x + dx * i, y + dy * (j + k) / 2] for (i, j) in [(0, 0), (0, 1), (1, 1), (1, 0)]] for k in range(2)])
            elif c == "|":  # deux rectangles (verticale)
                polys.extend([[[x + dx * (i + k) / 2, y + j] for (i, j) in [(0, 0), (0, 1), (1, 1), (1, 0)]] for k in range(2)])
            elif c == "+" or c == "#" or c.isdigit():  # petits carres
                n = 2 if c == "+" else 3 if c == "#" else int(c)
                polys.extend([[[x + dx * (i + k) / n, y + dy * (j + l) / n] for (i, j) in [(0, 0), (0, 1), (1, 1), (1, 0)]] for k in range(int(n)) for l in range(int(n))])
            elif c == "x" or c == "*":  # triangles en croix
                f = 2 * pi / 8
                l = [[x + dx / 2 * (1 + cos(f * k) / max(abs(cos(f * k)), abs(sin(f * k)))), y + dy / 2 * (1 + sin(f * k) / max(abs(cos(f * k)), abs(sin(f * k))))] for k in range(-1, 8, 1 + (c == "x"))]
                polys.extend([[l[i], [x + dx / 2, y + dy / 2], l[i + 1]] for i in range(len(l) - 1)])
            elif c == "/" or c == "\\":
                polys.extend([[[x + dx * i, y + dy * (1 - j if c == "/" else j)] for (i, j) in [(0, 1), (1, 0), p]] for p in [(0, 0), (1, 1)]])
            else:  # par defaut -> carre vide
                polys.append([[x + dx * i, y + dy * j] for (i, j) in [(0, 0), (0, 1), (1, 1), (1, 0)]])

    polys = [[[xmin + (x - lx[0]) * (xmax - xmin) / (lx[-1] - lx[0]), ymin + (y - ly[0]) * (ymax - ymin) / (ly[-1] - ly[0])] for [x, y] in p] for p in polys]

    mesh = ml.MEDCouplingUMesh("mesh", 2)
    mesh.allocateCells(len(polys))
    off = 0
    for p in polys:
        mesh.insertNextCell(ml.NORM_POLYGON, len(p), [off + i for i in range(len(p))])
        off += len(p)
    mesh.finishInsertingCells()

    pts = [p for i in range(len(polys)) for p in polys[i]]
    co = ml.DataArrayDouble(pts, len(pts), 2)
    mesh.setCoords(co)

    mesh.changeSpaceDimension(3)
    mesh.orientCorrectly2DCells([0., 0., -1.], False)
    mesh.changeSpaceDimension(2)
    mesh.mergeNodes(1e-8)
    mesh.conformize2D(1e-8)
    if unpo: mesh.unPolyze()

    if extr > 0:
        mesh.changeSpaceDimension(3)
        mesh1d = ml.MEDCouplingUMesh("ex", 1)
        mesh1d.allocateCells(extr)
        coo = [0., 0., 0.]
        for i in range(extr):
            mesh1d.insertNextCell(ml.NORM_SEG2, 2, [i, i + 1])
            coo.extend([0., 0., float(i + 1) / float(extr)])
        coo_arr = ml.DataArrayDouble(coo, extr + 1, 3)
        mesh1d.setCoords(coo_arr)
        mesh = mesh.buildExtrudedMesh(mesh1d, 0)
        mesh.setName("mesh")

    mf, desc, descIndx, revDesc, revDescIndx = mesh.buildDescendingConnectivity()
    mf.setName("mesh")
    mm = ml.MEDFileUMesh.New()
    mm.setMeshAtLevel(0, mesh)
    mm.setMeshAtLevel(-1, mf)

    bf = mf.computeCellCenterOfMass()
    noms_cl = [["left", "right"]]
    if extr > 0: noms_cl.append(["front", "back"])
    noms_cl.append(["down", "up"])
    for i in range(2 + (extr > 0)):  # frontieres en x puis y (puis en z si extr)
        for j in range(2):  # min puis max
            g = []
            for idx, b in enumerate(bf):
                if abs(b[i] - [[xmin, xmax], [ymin, ymax], [0., 1.]][i][j]) < 1e-5:
                    g.append(idx)
            grp = ml.DataArrayInt.New(g)
            grp.setName(noms_cl[i][j])
            mm.addGroup(-1, grp)

    return mm

if __name__ == "__main__":

    xmin, xmax, ymin, ymax = 0., 1., 0., 1.
    n = 4

    # maillage non conforme
    getWeirdMesh(xmin, xmax, ymin, ymax, list(range(n + 1)), list(range(n + 1)), "o7/43ox5*2").write("mesh1.med", 2)
    # maillage strech en x
    getWeirdMesh(xmin, xmax, ymin, ymax, [i ** 3 for i in range(n + 1)], list(range(n + 1)), "o").write("mesh2.med", 2)
    # maillage random
    import random
    paterns = "o123456789+x*|/\\"
    f = lambda x, y: random.randint(0, len(paterns))
    getWeirdMesh(xmin, xmax, ymin, ymax, list(range(n + 1)), list(range(n + 1)), paterns, f).write("mesh3.med", 2)
    # maillage raffine en fonction d'un champ spatial
    paterns = "o123456789"
    f = lambda x, y: 9 * (x ** 2 + y ** 2)
    getWeirdMesh(xmin, xmax, ymin, ymax, list(range(n + 1)), list(range(n + 1)), paterns, f).write("mesh4.med", 2)
    # maillage 3D
    getWeirdMesh(xmin, xmax, ymin, ymax, list(range(n + 1)), list(range(n + 1)), "o7/43ox5*2", extr=4).write("mesh5.med", 2)
    # maillage raffinement local type fvca
    f = lambda x, y: 2 * (int(x > 0.5 and y < 0.5) + int(x > 0.75 and y < 0.25))
    getWeirdMesh(xmin, xmax, ymin, ymax, list(range(n + 1)), list(range(n + 1)), paterns, f).write("mesh6.med", 2)
    # maillage checkerboard
    m = n - (n % 2) + 1
    getWeirdMesh(xmin, xmax, ymin, ymax, list(range(m + 1)), list(range(m + 1)), "o+").write("mesh7.med", 2)
