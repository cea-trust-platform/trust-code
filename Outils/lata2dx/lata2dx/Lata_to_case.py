#
# Creation d'un MEDFileData puis utilisation de CaseWriter ($MED_COUPLING_ROOT/bin/)
#

import LataLoader
import Lata_to_med
import MEDLoader
from CaseWriter import CaseWriter
from optparse import OptionParser
import os

def buildMEDFileData(latafile,lasttime):

    a=LataLoader.LataLoader(latafile)

    write_med = 0 # we not want to write meshes in med format
    medfile="no_medfile" # no medfile !
    mesh , indices, newNbNodes = Lata_to_med.BuildMergedMesh( latafile, write_med, medfile )

    liste_ite=xrange(a.GetNTimesteps())
    liste_fields=a.GetFieldNames()


    mfd = MEDLoader.MEDFileData()

    mfm = MEDLoader.MEDFileMeshes()

    med_file_mesh = MEDLoader.MEDFileUMesh()
    med_file_mesh.setMeshAtLevel(0, mesh)

    mfm.pushMesh(med_file_mesh)

    mff = MEDLoader.MEDFileFields()

    #remplissage de chaque field
    for name in liste_fields:

        #a chaque field est associe un MEDFileField1TS ou MEDFileFieldMultiTS
        if lasttime:
            liste_ite=[a.GetNTimesteps()-1]
            mf1ts = MEDLoader.MEDFileField1TS()
        
        mfmultits = MEDLoader.MEDFileFieldMultiTS()

        #boucle sur les pas de temps
        for ite in liste_ite:

            if name.find('_centerfaces')>0:
                continue
            f=a.GetFieldDouble(name,ite)
            
            #traitement des sommets
            if name.find('_SOM_')>0:
                f2 = f.deepCopy()
                f2.setMesh(mesh)
                d = f2.getArray()
                d2 = indices.invertArrayO2N2N2O(newNbNodes)
                f2.setArray(d[d2])

                if lasttime :
                    mf1ts.setFieldNoProfileSBT(f2)
                else :
                    mfmultits.appendFieldNoProfileSBT(f2)
            #cas general
            else :
                if lasttime :
                    mf1ts.setFieldNoProfileSBT(f)
                else :
                    mfmultits.appendFieldNoProfileSBT(f)
            pass
        pass #fin de la boucle sur les pas de temps

        if lasttime :
            mfmultits.pushBackTimeStep(mf1ts)

        mff.pushField(mfmultits)

    pass #fin de la boucle sur les fields

    mfd.setMeshes(mfm)
    mfd.setFields(mff)

    return mfd


if __name__=="__main__":

    parser = OptionParser()
    parser.set_usage("Convert a LATA file to a Case file.\n   %prog [options] lata_file [case_file]")
    parser.add_option("-g", "--groups", action="store_true", dest="groups", default=False,
                      help="Are groups in meshes stored in MEDFile exported in output case as subparts (default False)")
    parser.add_option("-c", "--currentdir", action="store_true", dest="here", default=False,
                      help="Are generated case,geo files generated in current directory. By default not, files are generated in directory containing the input file  (default False)")
    parser.add_option("-l", "--lasttime", action="store_true", dest="lasttime", default=False,
                      help="Store only the last time. By default all times are stores (default False)")
    (opts, args) = parser.parse_args()

    if len(args) == 1:
        latafile=os.path.abspath(args[0])
        extension_size=4
        end_of_file_without_extension=len( latafile )-extension_size
        file_without_extension=latafile[:end_of_file_without_extension]
        casefile=file_without_extension+"case"
    elif len(args) == 2:
        latafile=os.path.abspath(args[0])
        casefile=os.path.abspath(args[1])
    else:
        parser.print_usage()
        exit(1)
        pass


    try:
        cw=CaseWriter.New()
        cw.setExportingGroups(opts.groups)
        mfd=buildMEDFileData(latafile,opts.lasttime)
        cw.setMEDFileDS(mfd)
        listOfWrittenFileNames=cw.write(casefile)
    except MEDLoader.InterpKernelException as e:
        print "An error occurred during the conversion!"
        print "#######################################"
        raise e

    for l in listOfWrittenFileNames:
        print "File \"%s\" successfully written !"%(l)
        pass


