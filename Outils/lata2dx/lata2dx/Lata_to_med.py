## source /home/l-fb153746/Logiciels/TrioU171/Trio_U/Outils/lata2dx/lata2dx/install/env.sh

import LataLoader
import MEDLoader


def convert(latafile,medfile,lasttime=0):

    a=LataLoader.LataLoader(latafile)
    meshes=a.GetMeshNames()

    def nom_court(m):
        m2=m.split("_boundaries/")[0]
        m2=m2.split("_dual")[0]
        m2=m2.split("_centerfaces")[0]
        return m2
    meshname=nom_court(meshes[0])
    for m in meshes:
        if nom_court(m)!=meshname:
            raise Exception( "not implemented for differents meshes "+meshname +" "+ nom_court(m))
    mesh=a.GetMesh(meshname,0)
    eps=1e-5
    indices, areMerged, newNbNodes = mesh.mergeNodes(eps) 
    if areMerged:
        print "on corrige les champs Mesh=dom"
        pass
    MEDLoader.MEDLoader.WriteMesh(medfile,mesh,1)
    meshnamedual=meshname+"_dual"
    if meshnamedual in meshes:
        mesh_dual=a.GetMesh(meshnamedual,0)
        eps_dual=1.0e-7
        indices_dual, areMerged_dual, newNbNodes_dual = mesh_dual.mergeNodes(eps_dual)
        if areMerged_dual:
            print "on corrige les champs Mesh=dom_dual"
            pass
        MEDLoader.MEDLoader.WriteMesh(medfile,mesh_dual,0)
        pass
    
    liste_ite=xrange(a.GetNTimesteps())

    liste_fields=('VITESSE_ELEM_dom', 'VITESSE_SOM_dom', 'TEMPERATURE_ELEM_dom', 'TEMPERATURE_SOM_dom', 'DIVERGENCE_U_ELEM_dom', 'VITESSE_FACES_dom_dual',  'TEMPERATURE_FACES_dom_dual')
    # Ligne suivante a commenter si besoin. Voir si la methode fonctionne
    liste_fields=a.GetFieldNames()
    # on prend que le dernier temps
    if lasttime:
        liste_ite=[a.GetNTimesteps()-1]
    for ite in liste_ite:
        for name in liste_fields:
            if name.find('_centerfaces')>0:
                continue
            f=a.GetFieldDouble(name,ite)
            if name.find('_SOM_')>0:
            
                f2 = f.deepCpy()
                f2.setMesh(mesh)   # needed?
                d = f2.getArray()
                # print d
                d2 = indices.invertArrayO2N2N2O(newNbNodes)
                f2.setArray(d[d2])
        
                # print f2.getNumberOfTuples()
                MEDLoader.MEDLoader.WriteFieldUsingAlreadyWrittenMesh(medfile,f2)
            else:
            
                MEDLoader.MEDLoader.WriteFieldUsingAlreadyWrittenMesh(medfile,f)
                pass
            pass
        pass
    pass


if __name__=="__main__":
    import sys
    print "usage: Lata_to_med.py latafile [ medfile ]"
    len_args=len(sys.argv)
    if (len_args<2) or (len_args>3):
        raise Exception("wrong arguments")
    print len_args
    latafile=sys.argv[1]
    medfile=latafile+".med"
    if (len_args>2):
        medfile=latafile=sys.argv[2]
    
    
    convert(latafile,medfile)
    
    
    pass
    
