# -*- coding: utf-8 -*-

###
### This file is generated automatically by SALOME v7.6.0 with dump python functionality
###

import sys
import salome

salome.salome_init()
theStudy = salome.myStudy

import salome_notebook
notebook = salome_notebook.NoteBook(theStudy)
sys.path.insert( 0, r'/export/home/mr757596/Documents/Trio_U/DI/2015/DI3886_tuto_latex/test/MR/salome/exo3')

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
Cylinder_1 = geompy.MakeCylinderRH(0.5, 5)
Cylinder_2 = geompy.MakeCylinderRH(0.3, 3)
Rotation_1 = geompy.MakeRotation(Cylinder_2, OY, 90*math.pi/180.0)
Translation_1 = geompy.MakeTranslation(Rotation_1, 0, 0, 1.5)
Fuse_1 = geompy.MakeFuseList([Cylinder_1, Translation_1], True, True)
[Outlet] = geompy.SubShapes(Fuse_1, [2])
[Outlet] = geompy.SubShapes(Fuse_1, [22])
[Inlet_x] = geompy.SubShapes(Fuse_1, [24])
[Inlet_z] = geompy.SubShapes(Fuse_1, [3])
[Corner] = geompy.SubShapes(Fuse_1, [10])
[geomObj_1] = geompy.SubShapes(Fuse_1, [10])
Wall = geompy.CreateGroup(Fuse_1, geompy.ShapeType["FACE"])
geompy.UnionIDs(Wall, [7, 17])
geomObj_1 = geompy.MakeVertex(0, 0, 0)
geomObj_2 = geompy.MakeVectorDXDYDZ(1, 0, 0)
geomObj_3 = geompy.MakeVectorDXDYDZ(0, 1, 0)
geomObj_4 = geompy.MakeVectorDXDYDZ(0, 0, 1)
geompy.addToStudy( O, 'O' )
geompy.addToStudy( OX, 'OX' )
geompy.addToStudy( OY, 'OY' )
geompy.addToStudy( OZ, 'OZ' )
geompy.addToStudy( Cylinder_1, 'Cylinder_1' )
geompy.addToStudy( Cylinder_2, 'Cylinder_2' )
geompy.addToStudy( Rotation_1, 'Rotation_1' )
geompy.addToStudy( Translation_1, 'Translation_1' )
geompy.addToStudy( Fuse_1, 'Fuse_1' )
geompy.addToStudyInFather( Fuse_1, Outlet, 'Outlet' )
geompy.addToStudyInFather( Fuse_1, Inlet_x, 'Inlet_x' )
geompy.addToStudyInFather( Fuse_1, Inlet_z, 'Inlet_z' )
geompy.addToStudyInFather( Fuse_1, Wall, 'Wall' )
geompy.addToStudyInFather( Fuse_1, Corner, 'Corner' )

###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

from salome.StdMeshers import StdMeshersBuilder

smesh = smeshBuilder.New(theStudy)
Mesh_1 = smesh.Mesh(Fuse_1)
NETGEN_2D = Mesh_1.Triangle(algo=smeshBuilder.NETGEN_1D2D)
NETGEN_2D_Parameters = NETGEN_2D.Parameters()
NETGEN_2D_Parameters.SetMaxSize( 0.6 )
NETGEN_2D_Parameters.SetSecondOrder( 0 )
NETGEN_2D_Parameters.SetOptimize( 1 )
NETGEN_2D_Parameters.SetFineness( 5 )
NETGEN_2D_Parameters.SetGrowthRate( 0.1 )
NETGEN_2D_Parameters.SetNbSegPerEdge( 2 )
NETGEN_2D_Parameters.SetNbSegPerRadius( 4 )
NETGEN_2D_Parameters.SetMinSize( 0 )
NETGEN_2D_Parameters.SetUseSurfaceCurvature( 1 )
NETGEN_2D_Parameters.SetFuseEdges( 1 )
NETGEN_2D_Parameters.SetQuadAllowed( 0 )
NETGEN_2D_Parameters.SetLocalSizeOnShape(Corner, 0.01)
NETGEN_3D = Mesh_1.Tetrahedron()
Viscous_Layers_1 = NETGEN_3D.ViscousLayers(0.05,3,1.1,[ 7, 17 ],0,StdMeshersBuilder.NODE_OFFSET)
isDone = Mesh_1.Compute()
Mesh_1.SplitVolumesIntoTetra( Mesh_1, 1 )
Outlet_1 = Mesh_1.GroupOnGeom(Outlet,'Outlet',SMESH.FACE)
Inlet_x_1 = Mesh_1.GroupOnGeom(Inlet_x,'Inlet_x',SMESH.FACE)
Inlet_z_1 = Mesh_1.GroupOnGeom(Inlet_z,'Inlet_z',SMESH.FACE)
Wall_1 = Mesh_1.GroupOnGeom(Wall,'Wall',SMESH.FACE)
smesh.SetName(Mesh_1, 'Mesh_1')
Mesh_1.ExportMED( r'/export/home/mr757596/Documents/Trio_U/DI/2015/DI3886_tuto_latex/test/MR/salome/exo3/Mesh_1.med', 0, SMESH.MED_V2_2, 1, None ,1)


## Set names of Mesh objects
smesh.SetName(NETGEN_2D.GetAlgorithm(), 'NETGEN_2D')
smesh.SetName(NETGEN_3D.GetAlgorithm(), 'NETGEN_3D')
smesh.SetName(NETGEN_2D_Parameters, 'NETGEN 2D Parameters')
smesh.SetName(Viscous_Layers_1, 'Viscous Layers_1')
smesh.SetName(Outlet_1, 'Outlet')
smesh.SetName(Inlet_x_1, 'Inlet_x')
smesh.SetName(Inlet_z_1, 'Inlet_z')
smesh.SetName(Wall_1, 'Wall')
smesh.SetName(Mesh_1.GetMesh(), 'Mesh_1')


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser(1)
