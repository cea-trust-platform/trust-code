# -*- coding: utf-8 -*-
from salome.geom import geomBuilder
from salome.smesh import smeshBuilder

from math import radians

geompy = geomBuilder.New()
smesh = smeshBuilder.New()

p0 = geompy.MakeVertex(0, 0, 0)
x_axis = geompy.MakeVector(p0, geompy.MakeVertex(1, 0, 0))
y_axis = geompy.MakeVector(p0, geompy.MakeVertex(0, 1, 0))
z_axis = geompy.MakeVector(p0, geompy.MakeVertex(0, 0, 1))
axes = [x_axis, y_axis, z_axis]

h = 2.
r = 0.05

def make_ogrid_section(v, d, r):
    orientation = [2, 3, 1][d]
    section_large = geompy.MakeDiskPntVecR(v, axes[d], r)
    section_small = geompy.MakeDiskPntVecR(v, axes[d], 0.75 * r)
    rectangle = geompy.MakeTranslationTwoPoints(geompy.MakeRotation(geompy.MakeFaceHW(0.6 * r, 0.6 * r, orientation), axes[d], radians(45)), p0, v)

    r_cut_1 = geompy.MakeCut(section_small, rectangle)
    r_cut_1 = geompy.MakePartition([r_cut_1], [geompy.MakePlane(v, axes[dd], 10) for dd in [ddd for ddd in [0, 1, 2] if ddd != d]])
    section_small = geompy.MakePartition([section_small], [r_cut_1])

    r_cut_2 = geompy.MakeCut(section_large, section_small)
    r_cut_2 = geompy.MakePartition([r_cut_2], [geompy.MakePlane(v, axes[dd], 10) for dd in [ddd for ddd in [0, 1, 2] if ddd != d]])
    return geompy.MakePartition([section_large], [r_cut_1, r_cut_2])

z_b, z_t = 0, h
nz = 10
points = [geompy.MakeVertex(0, 0, z) for z in [z_b + i * (z_t - z_b) / nz for i in range(nz + 1)]]
wire = geompy.MakePolyline(points)
section = make_ogrid_section(points[0], 2, r)

channel = geompy.MakePipe(section, wire)

n = 2

mcol = smesh.Mesh(channel, "mesh")

for wire in geompy.ExtractShapes(channel, geompy.ShapeType["EDGE"]):
    kos = geompy.KindOfShape(wire)
    mseg = mcol.Segment(geom=wire)
    mseg.NumberOfSegments(n)

mcol.Quadrangle(algo=smeshBuilder.QUADRANGLE) #, geom=g)
mcol.Hexahedron(algo=smeshBuilder.Hexa) #, geom=g)

if not mcol.Compute(): raise Exception("le maillage n'est pas correctement genere!")

print("\nInformation about mesh by GetMeshInfo:")
info = smesh.GetMeshInfo(mcol)
keys = list(info.keys())
keys.sort()
for i in keys:
    print("  %s   :  %d" % (i, info[i]))

mcol.ExportMED("channel.med")

import medcoupling as mc
# extract only the channel
mfum = mc.MEDFileUMesh("channel.med", "mesh")
mesh = mfum.getMeshAtLevel(0)
mesh.convertAllToPoly()
mesh.orientCorrectlyPolyhedrons()

mf, desc, descIndx, F_E, F_Ei = mesh.buildDescendingConnectivity()
mf.setName("mesh")
mm = mc.MEDFileUMesh.New()
mm.setMeshAtLevel(0, mesh)
mm.setMeshAtLevel(-1, mf)

bf = mf.computeCellCenterOfMass()
g_t, g_b, g_d = [], [], []
for i, b in enumerate(bf):
    if abs(b[2] - z_t) < 1e-5: g_t.append(i)
    elif abs(b[2] - z_b) < 1e-5: g_b.append(i)
    elif F_Ei[i + 1] == F_Ei[i] + 1 : g_d.append(i)

for (n, g) in [("top", g_t), ("bottom", g_b), ("wall", g_d)]:
    grp = mc.DataArrayInt.New(g)
    grp.setName(n)
    mm.addGroup(-1, grp)

mm.write("channel.med", 2)

