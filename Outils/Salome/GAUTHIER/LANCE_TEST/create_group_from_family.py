from libMEDMEM_Swig import *

def modifie_nom(file):
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
            pass
        for typ in [MED_CELL,MED_EDGE,MED_FACE]:
            nb_fam= mesh.getNumberOfFamilies(typ)
            print(nb_fam,"i")
            for fam in range(nb_fam):
                Fam=mesh.getFamily(typ,fam+1)
                fam_name=Fam.getName()
                gr=GROUP()
                gr.setFamilies([Fam])
                gr.setName("group_"+fam_name)
                Fam.setNumberOfGroups(1)
                Fam.setGroupsNames(["group_"+fam_name])
                print(fam_name)
                pass
        pass
    from os import system;
    system("rm mod_"+file)
    # medDriverw = MED_MED_WRONLY_DRIVER("mod_"+file,med)
    i=med.addDriver(MED_DRIVER,"mod_"+file,MED_ECRI)
    med.write(i)


    # med.close()
    pass

if __name__ == '__main__':
    from sys import *

    modifie_nom(argv[1])
    pass
