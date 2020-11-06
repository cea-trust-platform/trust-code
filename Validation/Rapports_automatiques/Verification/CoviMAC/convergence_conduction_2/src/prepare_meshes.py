import sys
from weird_mesh import getWeirdMesh

argument = sys.argv[1]
n = int(sys.argv[2])

maillages = {"cartesian": "o", "triangle": "/\\//", "checkerboard": "o+", "raf_loc": "o123456789"}
xmin, xmax, ymin, ymax = 0., 1., 0., 1.

if argument == "raf_loc": f = lambda x, y: 2 * (int(x > 0.5 and y < 0.5) + int(x > 0.75 and y < 0.25))
else: f = None
if argument == "checkerboard": n += 1

getWeirdMesh(xmin, xmax, ymin, ymax, list(range(n + 1)), list(range(n + 1)), maillages[argument], func=f).write("mesh.med", 2)
