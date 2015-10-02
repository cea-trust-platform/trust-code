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
Cylindre = geompy.MakeCylinderRH(150, 350)
listSubShapeIDs = geompy.SubShapeAllIDs(Cylindre, geompy.ShapeType["FACE"])
listSubShapeIDs = geompy.SubShapeAllIDs(Cylindre, geompy.ShapeType["FACE"])
listSubShapeIDs = geompy.SubShapeAllIDs(Cylindre, geompy.ShapeType["FACE"])
listSubShapeIDs = geompy.SubShapeAllIDs(Cylindre, geompy.ShapeType["EDGE"])
Inlet = geompy.CreateGroup(Cylindre, geompy.ShapeType["FACE"])
geompy.UnionIDs(Inlet, [10])
Outlet = geompy.CreateGroup(Cylindre, geompy.ShapeType["FACE"])
geompy.UnionIDs(Outlet, [12])
Wall = geompy.CreateGroup(Cylindre, geompy.ShapeType["FACE"])
geompy.UnionIDs(Wall, [3])
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Cylindre, 'Cylindre' )
geompy.addToStudyInFather( Cylindre, Inlet, 'Inlet' )
geompy.addToStudyInFather( Cylindre, Outlet, 'Outlet' )
geompy.addToStudyInFather( Cylindre, Wall, 'Wall' )

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New(theStudy)
Mesh = smesh.Mesh(Cylindre)
NETGEN_2D3D = Mesh.Tetrahedron(algo=smeshBuilder.NETGEN_1D2D3D)
smesh.SetName(Mesh, 'Mesh')
Mesh.ExportMED( r'Mesh.med', 0, SMESH.MED_V2_2, 1 )
isDone = Mesh.Compute()
Inlet_1 = Mesh.GroupOnGeom(Inlet,'Inlet',SMESH.FACE)
Outlet_1 = Mesh.GroupOnGeom(Outlet,'Outlet',SMESH.FACE)
Wall_1 = Mesh.GroupOnGeom(Wall,'Wall',SMESH.FACE)
smesh.SetName(Mesh, 'Mesh')
Mesh.ExportMED( r'Mesh.med', 0, SMESH.MED_V2_2, 1 )
Inlet_1.SetColor( SALOMEDS.Color( 1, 0.666667, 0 ))
Outlet_1.SetColor( SALOMEDS.Color( 1, 0.666667, 0 ))
Wall_1.SetColor( SALOMEDS.Color( 1, 0.666667, 0 ))


## Set names of Mesh objects
smesh.SetName(NETGEN_2D3D.GetAlgorithm(), 'NETGEN_2D3D')
smesh.SetName(Inlet_1, 'Inlet')
smesh.SetName(Outlet_1, 'Outlet')
smesh.SetName(Wall_1, 'Wall')
smesh.SetName(Mesh.GetMesh(), 'Mesh')


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(1)
