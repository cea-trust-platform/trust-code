## source /home/l-fb153746/Logiciels/TrioU171/Trio_U/Outils/lata2dx/lata2dx/install/env.sh

import LataConversion
import LataLoader
import MEDLoader


def convert(latafile,medfile,lasttime=0):

    a=LataLoader.LataLoader(latafile)

    write_med = 1 # we want to write meshes in med format
    mesh , indices, newNbNodes = LataConversion.BuildMergedMesh( latafile, write_med, medfile )
    
    liste_ite=xrange(a.GetNTimesteps())

    # liste_fields=('VITESSE_ELEM_dom', 'VITESSE_SOM_dom', 'TEMPERATURE_ELEM_dom', 'TEMPERATURE_SOM_dom', 'DIVERGENCE_U_ELEM_dom', 'VITESSE_FACES_dom_dual',  'TEMPERATURE_FACES_dom_dual')
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
        medfile=sys.argv[2]
    
    
    convert(latafile,medfile)
    
    print "File "+str(medfile)+" successfully written !"

    
    pass
    
