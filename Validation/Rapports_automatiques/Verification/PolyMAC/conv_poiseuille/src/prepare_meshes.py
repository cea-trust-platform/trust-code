import sys
from weird_mesh import getWeirdMesh
argument = sys.argv[1]

maillages = {"cartesian": "o", "triangle": "/\\//", "non_conformal": "o973o5"}
xmin, xmax, ymin, ymax = 0., 1., 0., 1.
n = __resol__ if argument == "non_conformal" else 2 * __resol__ if argument == "triangle" else 3 * __resol__
getWeirdMesh(xmin, xmax, ymin, ymax, list(range(n + 1)), list(range(n + 1)), maillages[argument]).write("mesh.med", 2)
