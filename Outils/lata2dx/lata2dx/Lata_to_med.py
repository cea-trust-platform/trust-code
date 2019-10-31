## source /home/l-fb153746/Logiciels/TrioU171/Trio_U/Outils/lata2dx/lata2dx/install/env.sh

import LataLoader
import MEDLoader


EPS=1e-5       # used for merging nodes
EPS_DUAL=1e-7  # used for merging nodes (dual mesh)

def short_name(m):
    m2=m.split("_boundaries/")[0]
    m2=m2.split("_dual")[0]
    m2=m2.split("_centerfaces")[0]
    return m2

def BuildMergedMesh( latafile , write_med, medfile, domain_name ):
    """
    Load single mesh from latafile and write it to medfile, potentially removing duplicate nodes.
    @param latafile
    @param medfile
    @param  domain_name mesh name in the LATA file. Can be None in which case the first (and only one) mesh is taken.
    @return mesh MEDCoupling object representing the mesh
    @return indices renumbering array if nodes were merged. None otherwise
    @return newNbnodes new nb of nodes if renumbering was done. -1 otherwise
    @return meshType string indicating mesh type (UMesh or CMesh)
    """
    a=LataLoader.LataLoader(latafile)
    meshes=a.GetMeshNames()

    meshname=short_name(meshes[0])
    for m in meshes:
        if (short_name(m)!=meshname and domain_name is None):
            raise Exception( "not implemented for differents meshes ('"+meshname +"' and '"+ short_name(m) + "') if you don't specify a domain name explicitly")
    if not domain_name is None:
        meshname = domain_name
    mesh=a.GetMesh(meshname,0)

    typ_nam = type( mesh ).__name__
    if typ_nam not in ["MEDCouplingUMesh", "MEDCouplingCMesh"]:
        raise Exception( "Only implemented for meshes of type MEDCouplingUMesh, or MEDCouplingCMesh ( not "+ type( mesh ).__name__ +" )" )

    indices, newNbNodes = None, -1
    if typ_nam == "MEDCouplingUMesh":
        print("Dealing with an unstructured mesh - merging nodes")
        indices, areMerged, newNbNodes = mesh.mergeNodes( EPS )
        if areMerged:
            print("Merged nodes in the domain '%s'!" % meshname)
        else:
            print("No nodes to be merged in '%s'!" % meshname)

    # Write mesh
    if write_med :
        MEDLoader.WriteMesh(medfile,mesh,True)

    meshnamedual=meshname+"_dual"
    if meshnamedual in meshes:
        print("A dual mesh '%s' has been found." % meshnamedual)
        mesh_dual=a.GetMesh(meshnamedual,0)
        if typ_nam == "MEDCouplingUMesh":
            indices_dual, areMerged_dual, newNbNodes_dual = mesh_dual.mergeNodes( EPS_DUAL )
            if areMerged_dual:
                print("Merged nodes in the dual domain '%s'" % meshnamedual)
        # Write dual mesh
        MEDLoader.WriteMesh(medfile,mesh_dual,False)

    return mesh , indices, newNbNodes, typ_nam


def convert(latafile,medfile,domain_name,mesh_only,lasttime=0):

    a=LataLoader.LataLoader(latafile)

    write_med = 1 # we want to write meshes in med format
    mesh , indices, newNbNodes, meshType = BuildMergedMesh( latafile, write_med, medfile, domain_name )

    if mesh_only:
        return

    liste_ite=range(a.GetNTimesteps())

    # liste_fields=('VITESSE_ELEM_dom', 'VITESSE_SOM_dom', 'TEMPERATURE_ELEM_dom', 'TEMPERATURE_SOM_dom', 'DIVERGENCE_U_ELEM_dom', 'VITESSE_FACES_dom_dual',  'TEMPERATURE_FACES_dom_dual')
    # Ligne suivante a commenter si besoin. Voir si la methode fonctionne

    liste_fields=a.GetFieldNames()

    # on prend que le dernier temps
    if lasttime:
        liste_ite=[a.GetNTimesteps()-1]

    if meshType == "MEDCouplingUMesh":
        for ite in liste_ite:
            for name in liste_fields:

                if name.find('_centerfaces')>0:
                    continue

                f=a.GetFieldDouble(name,ite)
                if name.find('_SOM_')>0:
                    f2 = f.deepCopy()
                    if not indices is None: # some renumbering has to be performed (nodes have been merged)
                        d = f2.getArray()
                        d2 = indices.invertArrayO2N2N2O(newNbNodes)
                        f2.setArray(d[d2])

                    MEDLoader.WriteFieldUsingAlreadyWrittenMesh(medfile,f2)
                else:
                    MEDLoader.WriteFieldUsingAlreadyWrittenMesh(medfile,f)
                    pass
                pass
            pass
        pass
    else:
        print("WARNING: not extracting any field value on a Cartesian mesh (was never tested)")


def usage():
    print("usage: Lata_to_med.py latafile [ medfile ] [ domain_name ] [ mesh_only ] ")

    print("   where:")
    print("      <latafile>   : is the latafile that needs to be converted")
    print("      <medfile>    : is the final med file to written (optional)")
    print("      <domain_name>: is the name of the domain to be treated")
    print("      <mesh_only>  : if set to 1, only the mesh will be written, not the fields")
    print("")

if __name__=="__main__":
    import sys
    usage()
    len_args=len(sys.argv)
    if (len_args<2) or (len_args>5):
        raise Exception("wrong number of arguments")
    print(len_args)
    latafile=sys.argv[1]
    medfile=latafile+".med"
    domain_name=None
    mesh_only=False
    if (len_args>2):
        medfile=sys.argv[2]
    if (len_args>3):
        domain_name=sys.argv[3]
    if (len_args>4):
        mesh_only = (sys.argv[4] == "1")

    convert(latafile,medfile, domain_name, mesh_only)

    print("File "+str(medfile)+" successfully written !")


    pass
