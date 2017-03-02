#
# Common options and functions to .lata conversion 
# Lata_to_med.py
# Lata_to_case.py
#

import LataLoader
import MEDLoader


# common options 

EPS=1e-5       # used for merging nodes
EPS_DUAL=1e-7  # used for merging nodes (dual mesh)


# common functions

def short_name(m):
    m2=m.split("_boundaries/")[0]
    m2=m2.split("_dual")[0]
    m2=m2.split("_centerfaces")[0]
    return m2

def BuildMergedMesh( latafile , write_med , medfile ):

    a=LataLoader.LataLoader(latafile)
    meshes=a.GetMeshNames()

    meshname=short_name(meshes[0])
    for m in meshes:
        if short_name(m)!=meshname:
            raise Exception( "not implemented for differents meshes "+meshname +" "+ short_name(m))
    mesh=a.GetMesh(meshname,0)

    indices, areMerged, newNbNodes = mesh.mergeNodes( EPS ) 
    if areMerged:
        print "Correcting fields Mesh=dom"
        pass

    if write_med :
        MEDLoader.MEDLoader.WriteMesh(medfile,mesh,1)

    meshnamedual=meshname+"_dual"
    if meshnamedual in meshes:
        mesh_dual=a.GetMesh(meshnamedual,0)
        indices_dual, areMerged_dual, newNbNodes_dual = mesh_dual.mergeNodes( EPS_DUAL )
        if areMerged_dual:
            print "Correcting fields Mesh=dom_dual"
            pass
        if write_med:
            MEDLoader.MEDLoader.WriteMesh(medfile,mesh_dual,0)
        pass

    return mesh , indices, newNbNodes


