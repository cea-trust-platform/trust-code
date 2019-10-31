from libMEDMEM_Swig import *

def modifie_nom(file,format):
    med=MED()
    #file="test4.med"
    medDriver = MED_MED_RDONLY_DRIVER(file,med)
    medDriver.open()
    medDriver.readFileStruct()
    medDriver.read()
    nb_mesh=med.getNumberOfMeshes()
    for i in range (nb_mesh):
        mesh_name=med.getMeshName(i)
        mesh=med.getMesh(mesh_name)
        if mesh.getMeshDimension() ==3:
            typ=MED_FACE

        else:
            typ=MED_EDGE
            pass
        nb_fam= mesh.getNumberOfFamilies(typ)
        if (nb_fam==0):
            typ=MED_CELL
            nb_fam= mesh.getNumberOfFamilies(typ)
            pass
        for fam in range(nb_fam):
            Fam=mesh.getFamily(typ,fam+1)
            fam_name=Fam.getName()
            from string import split
            print fam_name
            fam_morceaux=split(fam_name,"_")
            fam_name_new=fam_morceaux[2]
            for morc in fam_morceaux[3:]:
                fam_name_new+="_"+morc
            if (fam_name_new!=fam_name):
                print fam_name,"->" ,fam_name_new
                pass
            Fam.setName(fam_name_new)
            pass
        pass
    # medDriverw = MED_MED_WRONLY_DRIVER("mod_"+file,med)
    medFileVersion = getMedFileVersionForWriting()
    if ((medFileVersion == V22) and (format=="med21")):
        setMedFileVersionForWriting(V21)
    elif ((medFileVersion == V21) and (format=="med22")):
        setMedFileVersionForWriting(V22)
        pass
    i=med.addDriver(MED_DRIVER,"mod_"+file,MED_ECRI)
    med.write(i)
    from os import system;
    system("mv "+file+" "+file+".sa ; mv mod_"+file+" "+file)

    # med.close()
    pass


def change_name_fam(file,format="med21"):
    modifie_nom(file,format)
    pass


def write_pour_trio(tetra,name,format="med21",tolerance=1.e-5):
    if name[-4:]!=".med" and format!="unv":
        name+=".med"
        pass

    tetra_mesh = tetra.GetMesh()
    tetra_edit = tetra_mesh.GetMeshEditor()

    tetra_noeuds = tetra_edit.FindCoincidentNodes(tolerance )
    print "noeuds coincidants ", tetra_noeuds
    tetra_edit.MergeNodes(tetra_noeuds)
    domname=name[:-4]
    f=open(domname+".data","w")
    if (format!="unv"):
        from os import system
        #print dir(tetra)
        mesh_name=tetra.GetName()
        system("rm -f Conv_"+name+" "+name+" mod_"+name)
        tetra.ExportMED(name)
        from change_name_fam import change_name_fam
        try:
            med=MED()
            change_name_fam(name,format)
        except:
            # on laisse le fichier tel quel ....
            pass

        tstr="dimension 3 \nDomaine "+domname+"  Lire_Med short_family_names "+domname+ " \""+mesh_name+"\" "+ name +"\n"
    else:
        tetra.ExportUNV(name)
        tstr="dimension 3 \nDomaine "+domname+"  Lire_ideas "+domname+ " "+ name +"\n"
    pass
    tstr+="ecrire_fichier "+domname+"  "+domname+".geom \n"
    f.write(tstr)
    f.close()

    pass
if __name__ == '__main__':
    from sys import *

    change_name_fam(argv[1])
    pass
