# -*- coding: utf-8 -*-

###
### This file is generated automatically by SALOME v7.5.0 with dump python functionality
###

import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

import salome_notebook
notebook = salome_notebook.NoteBook(theStudy)
#sys.path.insert( 0, r'/users/triou/VUES/beta/TRUST/doc/TRUST/exercices/salome')

###
### GEOM component
###

import GEOM
from salome.geom import geomBuilder
import math
import SALOMEDS


geompy = geomBuilder.New(theStudy)

O = geompy.MakeVertex(0, 0, 0)
OX = geompy.MakeVectorDXDYDZ(1, 0, 0)
OY = geompy.MakeVectorDXDYDZ(0, 1, 0)
OZ = geompy.MakeVectorDXDYDZ(0, 0, 1)
Vertex_1 = geompy.MakeVertex(0, 0, 0)
Vertex_2 = geompy.MakeVertex(1, 0, 0)
Vertex_3 = geompy.MakeVertex(1, 0, 0.3)
Vertex_4 = geompy.MakeVertex(0.75, 0, 0.3)
Vertex_5 = geompy.MakeVertex(0.375, 0, 1)
Vertex_6 = geompy.MakeVertex(0.75, 0, 1.6)
Vertex_7 = geompy.MakeVertex(1, 0, 1.6)
Vertex_8 = geompy.MakeVertex(1, 0, 2)
Vertex_9 = geompy.MakeVertex(0, 0, 2)
Line_1 = geompy.MakeLineTwoPnt(Vertex_1, Vertex_2)
Line_2 = geompy.MakeLineTwoPnt(Vertex_2, Vertex_3)
Line_3 = geompy.MakeLineTwoPnt(Vertex_3, Vertex_4)
Line_4 = geompy.MakeLineTwoPnt(Vertex_6, Vertex_7)
Line_5 = geompy.MakeLineTwoPnt(Vertex_7, Vertex_8)
Line_6 = geompy.MakeLineTwoPnt(Vertex_8, Vertex_9)
Line_7 = geompy.MakeLineTwoPnt(Vertex_9, Vertex_1)
Arc_1 = geompy.MakeArc(Vertex_4, Vertex_5, Vertex_6)
Wire_1 = geompy.MakeWire([Line_1, Line_2, Line_3, Line_4, Line_5, Line_6, Line_7, Arc_1], 1e-07)
Face_1 = geompy.MakeFaceWires([Wire_1], 1)
Cylinder_1 = geompy.MakeRevolution(Face_1, OZ, 360*math.pi/180.0)
Wall = geompy.CreateGroup(Cylinder_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Wall, [27, 17, 7])
Inlet = geompy.CreateGroup(Cylinder_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Inlet, [32])
Outlet = geompy.CreateGroup(Cylinder_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Outlet, [3])
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Vertex_1, 'Vertex_1' )
geompy.addToStudy( Vertex_2, 'Vertex_2' )
geompy.addToStudy( Vertex_3, 'Vertex_3' )
geompy.addToStudy( Vertex_4, 'Vertex_4' )
geompy.addToStudy( Vertex_5, 'Vertex_5' )
geompy.addToStudy( Vertex_6, 'Vertex_6' )
geompy.addToStudy( Vertex_7, 'Vertex_7' )
geompy.addToStudy( Vertex_8, 'Vertex_8' )
geompy.addToStudy( Vertex_9, 'Vertex_9' )
geompy.addToStudy( Line_1, 'Line_1' )
geompy.addToStudy( Line_2, 'Line_2' )
geompy.addToStudy( Line_3, 'Line_3' )
geompy.addToStudy( Line_6, 'Line_6' )
geompy.addToStudy( Wire_1, 'Wire_1' )
geompy.addToStudy( Line_5, 'Line_5' )
geompy.addToStudy( Line_7, 'Line_7' )
geompy.addToStudy( Line_4, 'Line_4' )
geompy.addToStudy( Arc_1, 'Arc_1' )
geompy.addToStudy( Face_1, 'Face_1' )
geompy.addToStudy( Cylinder_1, 'Cylinder_1' )
geompy.addToStudyInFather( Cylinder_1, Wall, 'Wall' )
geompy.addToStudyInFather( Cylinder_1, Inlet, 'Inlet' )
geompy.addToStudyInFather( Cylinder_1, Outlet, 'Outlet' )


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(1)
