import sys
from weird_mesh import getWeirdMesh
argument = sys.argv[1]

maillages = {"cartesian": "o", "triangle": "/", "non_conformal": "o37o"}
xmin, xmax, ymin, ymax = 0., 1., 0., 1.
nx, ny = 10, 10
getWeirdMesh(xmin, xmax, ymin, ymax, range(nx + 1), range(ny + 1), maillages[argument]).write("mesh.med", 2)
