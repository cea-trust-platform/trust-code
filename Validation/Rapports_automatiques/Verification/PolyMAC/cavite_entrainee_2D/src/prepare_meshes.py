import sys
from weird_mesh import getWeirdMesh
argument = sys.argv[1]

maillages = {"cartesian": "o", "triangle": "///\\//\\", "non_conformal": "o743o52"}
xmin, xmax, ymin, ymax = 0., 1., 0., 1.
n = 12 if argument == "non_conformal" else 26 if argument == "triangle" else 32
getWeirdMesh(xmin, xmax, ymin, ymax, list(range(n + 1)), list(range(n + 1)), maillages[argument]).write("mesh.med", 2)
