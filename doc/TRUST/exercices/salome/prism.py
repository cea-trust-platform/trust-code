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
Viscous_Layers_1 = smesh.CreateHypothesis('ViscousLayers')
Viscous_Layers_1.SetTotalThickness( 50 )
Viscous_Layers_1.SetNumberLayers( 3 )
Viscous_Layers_1.SetStretchFactor( 1.1 )
Viscous_Layers_1.SetIgnoreFaces( [ 10, 12 ] )
NETGEN_2D_Parameters = smesh.CreateHypothesis('NETGEN_Parameters_2D', 'NETGENEngine')
NETGEN_2D_Parameters.SetMaxSize( 55.0312 )
NETGEN_2D_Parameters.SetSecondOrder( 0 )
NETGEN_2D_Parameters.SetOptimize( 1 )
NETGEN_2D_Parameters.SetFineness( 2 )
NETGEN_2D_Parameters.SetMinSize( 18.3437 )
NETGEN_2D_Parameters.SetQuadAllowed( 1 )
NETGEN_2D = smesh.CreateHypothesis('NETGEN_2D', 'NETGENEngine')
NETGEN_3D = smesh.CreateHypothesis('NETGEN_3D', 'NETGENEngine')
Viscous_Layers = smesh.CreateHypothesis('ViscousLayers')
Viscous_Layers.SetTotalThickness( 30 )
Viscous_Layers.SetNumberLayers( 3 )
Viscous_Layers.SetStretchFactor( 1.1 )
Viscous_Layers.SetIgnoreFaces( [ 10, 12 ] )
NETGEN_2D_Parameters_1 = smesh.CreateHypothesis('NETGEN_Parameters_2D', 'NETGENEngine')
NETGEN_2D_Parameters_1.SetMaxSize( 55.0549 )
NETGEN_2D_Parameters_1.SetSecondOrder( 0 )
NETGEN_2D_Parameters_1.SetOptimize( 1 )
NETGEN_2D_Parameters_1.SetMinSize( 18.3516 )
NETGEN_2D_Parameters_1.SetQuadAllowed( 1 )
Refined_Mesh = smesh.Mesh(Cylindre)
status = Refined_Mesh.AddHypothesis(NETGEN_2D_Parameters_1)
status = Refined_Mesh.AddHypothesis(NETGEN_2D)
status = Refined_Mesh.AddHypothesis(Viscous_Layers)
status = Refined_Mesh.AddHypothesis(NETGEN_3D)
NETGEN_2D_Parameters_1.SetFineness( 4 )
isDone = Refined_Mesh.Compute()
Refined_Mesh.SplitVolumesIntoTetra( Refined_Mesh, 1 )
Inlet_2 = Refined_Mesh.GroupOnGeom(Inlet,'Inlet',SMESH.FACE)
Outlet_2 = Refined_Mesh.GroupOnGeom(Outlet,'Outlet',SMESH.FACE)
Wall_2 = Refined_Mesh.GroupOnGeom(Wall,'Wall',SMESH.FACE)
smesh.SetName(Refined_Mesh, 'Refined_Mesh')
Refined_Mesh.ExportMED( r'Refined_Mesh.med', 0, SMESH.MED_V2_2, 1 )


## Set names of Mesh objects
smesh.SetName(NETGEN_2D3D.GetAlgorithm(), 'NETGEN_2D3D')
smesh.SetName(NETGEN_3D, 'NETGEN_3D')
smesh.SetName(NETGEN_2D, 'NETGEN_2D')
smesh.SetName(NETGEN_2D_Parameters, 'NETGEN 2D Parameters')
smesh.SetName(Viscous_Layers_1, 'Viscous Layers_1')
smesh.SetName(NETGEN_2D_Parameters_1, 'NETGEN 2D Parameters')
smesh.SetName(Viscous_Layers, 'Viscous Layers')
smesh.SetName(Inlet_1, 'Inlet')
smesh.SetName(Outlet_1, 'Outlet')
smesh.SetName(Wall_1, 'Wall')
smesh.SetName(Mesh.GetMesh(), 'Mesh')
smesh.SetName(Refined_Mesh.GetMesh(), 'Refined_Mesh')
smesh.SetName(Outlet_2, 'Outlet')
smesh.SetName(Wall_2, 'Wall')
smesh.SetName(Inlet_2, 'Inlet')


if salome.sg.hasDesktop():
    salome.sg.updateObjBrowser(1)
