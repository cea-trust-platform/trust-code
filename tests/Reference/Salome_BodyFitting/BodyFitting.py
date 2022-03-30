#!/usr/bin/env python

###
### This file is generated automatically by SALOME v9.8.0 with dump python functionality
###

import sys
import salome

salome.salome_init()

###
### SHAPER component
###

from salome.shaper import model

model.begin()
partSet = model.moduleDocument()

### Create Part
Part_1 = model.addPart(partSet)
Part_1_doc = Part_1.document()

### Create Cylinder
Cylinder_1 = model.addCylinder(Part_1_doc, model.selection("VERTEX", "PartSet/Origin"), model.selection("EDGE", "PartSet/OZ"), 5, 10)

### Create Group
Group_1 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Cylinder_1_1/Face_2")])
Group_1.setName("Outlet")
Group_1.result().setName("Outlet")

### Create Group
Group_2 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Cylinder_1_1/Face_3")])
Group_2.setName("Inlet")
Group_2.result().setName("Inlet")

### Create Group
Group_3 = model.addGroup(Part_1_doc, "Faces", [model.selection("FACE", "Cylinder_1_1/Face_1")])
Group_3.setName("Wall")
Group_3.result().setName("Wall")

model.end()

###
### SHAPERSTUDY component
###

model.publishToShaperStudy()
import SHAPERSTUDY
Cylinder_1_1, Outlet, Inlet, Wall, = SHAPERSTUDY.shape(model.featureStringId(Cylinder_1))
###
### SMESH component
###

import  SMESH, SALOMEDS
from salome.smesh import smeshBuilder

smesh = smeshBuilder.New()
#smesh.SetEnablePublish( False ) # Set to False to avoid publish in study if not needed or in some particular situations:
                                 # multiples meshes built in parallel, complex and numerous mesh edition (performance)

BodyFitting = smesh.Mesh(Cylinder_1_1)
Outlet_1 = BodyFitting.GroupOnGeom(Outlet,'Outlet',SMESH.FACE)
Inlet_1 = BodyFitting.GroupOnGeom(Inlet,'Inlet',SMESH.FACE)
Wall_1 = BodyFitting.GroupOnGeom(Wall,'Wall',SMESH.FACE)

Cartesian_3D = BodyFitting.BodyFitted()

Body_Fitting_Parameters_1 = Cartesian_3D.SetGrid([ [ '1.73205' ], [ 0, 1 ]],[ [ '1.73205' ], [ 0, 1 ]],[ [ '1.73205' ], [ 0, 1 ]],4,1)
Body_Fitting_Parameters_1.SetSizeThreshold( 200 )
Body_Fitting_Parameters_1.SetToAddEdges( 1 )
Body_Fitting_Parameters_1.SetToCreateFaces( 1 )
Body_Fitting_Parameters_1.SetToConsiderInternalFaces( 1 )
Body_Fitting_Parameters_1.SetToUseThresholdForInternalFaces( 1 )
Body_Fitting_Parameters_1.SetGridSpacing( [ '0.5' ], [ 0, 1 ], 0 )
Body_Fitting_Parameters_1.SetGridSpacing( [ '0.5' ], [ 0, 1 ], 1 )
Body_Fitting_Parameters_1.SetGridSpacing( [ '0.5' ], [ 0, 1 ], 2 )

isDone = BodyFitting.Compute()
[ Outlet_1, Inlet_1, Wall_1 ] = BodyFitting.GetGroups()

smesh.SetName(BodyFitting, 'BodyFitting')
try:
  BodyFitting.ExportMED( r'./BodyFitting.med', 0, 41, 1, BodyFitting, 1, [], '',-1, 1 )
  pass
except:
  print('ExportPartToMED() failed. Invalid file name?')

## Set names of Mesh objects
smesh.SetName(Inlet_1, 'Inlet')
smesh.SetName(Wall_1, 'Wall')
smesh.SetName(Outlet_1, 'Outlet')
smesh.SetName(Cartesian_3D.GetAlgorithm(), 'Cartesian_3D')
smesh.SetName(BodyFitting.GetMesh(), 'BodyFitting')
smesh.SetName(Body_Fitting_Parameters_1, 'Body Fitting Parameters_1')


if salome.sg.hasDesktop():
  salome.sg.updateObjBrowser()
