import sys
from weird_mesh import getWeirdMesh
argument = sys.argv[1]

maillages = {"cartesian": "oo", "triangle": "o/", "non_conformal": "o973o5"}
xmin, xmax, ymin, ymax = 0., 2., 0., 1.
nx, ny = 20, 10
getWeirdMesh(xmin, xmax, ymin, ymax, list(range(nx + 1)), list(range(ny + 1)), maillages[argument]).write("mesh.med", 2)
