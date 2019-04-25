# -*- coding: utf-8 -*-

###
### This file is generated automatically by SALOME v8.5.0 with dump python functionality
###

import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

import salome_notebook
notebook = salome_notebook.NoteBook(theStudy)

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
Box_1 = geompy.MakeBoxDXDYDZ(200, 200, 400)
Vertex_1 = geompy.MakeVertex(100, 100, 0)
Cylinder_1 = geompy.MakeCylinder(Vertex_1, OZ, 40, 400)
Cut_1 = geompy.MakeCutList(Box_1, [Cylinder_1], True)
Partition_1 = geompy.MakePartition([Cylinder_1, Cut_1], [], [], [], geompy.ShapeType["SOLID"], 0, [], 0)
Solid = geompy.CreateGroup(Partition_1, geompy.ShapeType["SOLID"])
geompy.UnionIDs(Solid, [15])
Fluid = geompy.CreateGroup(Partition_1, geompy.ShapeType["SOLID"])
geompy.UnionIDs(Fluid, [2])
Fluid_inlet = geompy.CreateGroup(Partition_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Fluid_inlet, [13])
Fluid_outlet = geompy.CreateGroup(Partition_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Fluid_outlet, [11])
Solid_top = geompy.CreateGroup(Partition_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Solid_top, [34])
Solid_bottom = geompy.CreateGroup(Partition_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Solid_bottom, [45])
Solid_lateral_walls = geompy.CreateGroup(Partition_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Solid_lateral_walls, [27, 49, 40, 17])
Solid_Fluid_Interface = geompy.CreateGroup(Partition_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Solid_Fluid_Interface, [4])
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Box_1, 'Box_1' )
geompy.addToStudy( Vertex_1, 'Vertex_1' )
geompy.addToStudy( Cylinder_1, 'Cylinder_1' )
geompy.addToStudy( Cut_1, 'Cut_1' )
geompy.addToStudy( Partition_1, 'Partition_1' )
geompy.addToStudyInFather( Partition_1, Solid, 'Solid' )
geompy.addToStudyInFather( Partition_1, Fluid, 'Fluid' )
geompy.addToStudyInFather( Partition_1, Fluid_inlet, 'Fluid_inlet' )
geompy.addToStudyInFather( Partition_1, Fluid_outlet, 'Fluid_outlet' )
geompy.addToStudyInFather( Partition_1, Solid_top, 'Solid_top' )
geompy.addToStudyInFather( Partition_1, Solid_bottom, 'Solid_bottom' )
geompy.addToStudyInFather( Partition_1, Solid_lateral_walls, 'Solid_lateral_walls' )
geompy.addToStudyInFather( Partition_1, Solid_Fluid_Interface, 'Solid_Fluid_Interface' )

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New(theStudy)
Mesh_1 = smesh.Mesh(Partition_1)
NETGEN_1D_2D_3D = Mesh_1.Tetrahedron(algo=smeshBuilder.NETGEN_1D2D3D)
NETGEN_3D_Parameters_1 = NETGEN_1D_2D_3D.Parameters()
NETGEN_3D_Parameters_1.SetMaxSize( 48.9898 )
NETGEN_3D_Parameters_1.SetSecondOrder( 0 )
NETGEN_3D_Parameters_1.SetOptimize( 1 )
NETGEN_3D_Parameters_1.SetFineness( 3 )
NETGEN_3D_Parameters_1.SetChordalError( 0.1 )
NETGEN_3D_Parameters_1.SetChordalErrorEnabled( 0 )
NETGEN_3D_Parameters_1.SetMinSize( 6.97246 )
NETGEN_3D_Parameters_1.SetUseSurfaceCurvature( 1 )
NETGEN_3D_Parameters_1.SetFuseEdges( 1 )
NETGEN_3D_Parameters_1.SetQuadAllowed( 0 )
isDone = Mesh_1.Compute()
Solid_1 = Mesh_1.GroupOnGeom(Solid,'Solid',SMESH.VOLUME)
Fluid_1 = Mesh_1.GroupOnGeom(Fluid,'Fluid',SMESH.VOLUME)
Fluid_inlet_1 = Mesh_1.GroupOnGeom(Fluid_inlet,'Fluid_inlet',SMESH.FACE)
Fluid_outlet_1 = Mesh_1.GroupOnGeom(Fluid_outlet,'Fluid_outlet',SMESH.FACE)
Solid_top_1 = Mesh_1.GroupOnGeom(Solid_top,'Solid_top',SMESH.FACE)
Solid_bottom_1 = Mesh_1.GroupOnGeom(Solid_bottom,'Solid_bottom',SMESH.FACE)
Solid_lateral_walls_1 = Mesh_1.GroupOnGeom(Solid_lateral_walls,'Solid_lateral_walls',SMESH.FACE)
Solid_Fluid_Interface_1 = Mesh_1.GroupOnGeom(Solid_Fluid_Interface,'Solid_Fluid_Interface',SMESH.FACE)
smesh.SetName(Mesh_1, 'Mesh_1')
try:
  Mesh_1.ExportMED( r'Mesh_1.med', 0, SMESH.MED_MINOR_2, 1, None ,1)
  pass
except:
  print 'ExportToMEDX() failed. Invalid file name?'


## Set names of Mesh objects
smesh.SetName(NETGEN_1D_2D_3D.GetAlgorithm(), 'NETGEN 1D-2D-3D')
smesh.SetName(NETGEN_3D_Parameters_1, 'NETGEN 3D Parameters_1')
smesh.SetName(Fluid_inlet_1, 'Fluid_inlet')
smesh.SetName(Fluid_outlet_1, 'Fluid_outlet')
smesh.SetName(Solid_top_1, 'Solid_top')
smesh.SetName(Solid_bottom_1, 'Solid_bottom')
smesh.SetName(Solid_lateral_walls_1, 'Solid_lateral_walls')
smesh.SetName(Solid_Fluid_Interface_1, 'Solid_Fluid_Interface')
smesh.SetName(Mesh_1.GetMesh(), 'Mesh_1')
smesh.SetName(Fluid_1, 'Fluid')
smesh.SetName(Solid_1, 'Solid')


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(True)
