# -*-coding:utf-8 -*

######################################################################################################################
#   This file runs a set of Ransom problems 
#
#   Author: Jerome Francescatto
#   Date:   29/08/2022
#   Description : robinet de Ransom
#######################################################################################################################


import os


if __name__ == "__main__":

    TCs = {
        "ransom_StiffenedGas": {
            "nb_Phases": 2,
            "nombre procs": 1
        },
        "ransom_StiffenedGasMUSIG": {
            "nb_Phases": 12,
            "nombre procs": 1
        },
    }
    names = ["ransom_StiffenedGas", "ransom_StiffenedGasMUSIG"] # noms des cas test avant modif

    # !! Gestion du parallel !! #
    directory = names.copy() # nom du repertoire avec PAR_ si nécessaire
    for i in range(len(directory)) :
        if TCs[names[i]]["nombre procs"] > 1 :
            directory[i] = "PAR_"+directory[i]
        os.system("mkdir {}".format(directory[i]))
    # !! ------------------- !! #


    columns = ["nb_Phases", "nb procs"]
    tab = plot.Table(columns)

    
    for i in range(len(directory)) :
        name = names[i]

        with open(name+".data", "r") as file:
            filedata = file.read()

        
        tab.addLigne( [[TCs[name]["nb_Phases"],  TCs[name]["nombre procs"]]], name)

        with open("{}/{}.data".format(directory[i], names[i]), "w") as file:
            file.write(filedata)

    # !! Gestion du parallel !! #
    for i in range(len(names)) :
        if TCs[names[i]]["nombre procs"] > 1 :
            os.chdir("{}".format(directory[i]))
            os.system("make_PAR.data %s.data " %names[i]+str(TCs[name]["nombre procs"]))  #decoupage du jdd
            TCs["PAR_"+names[i]] = TCs[names[i]] # on renonmme le test
            del TCs[names[i]] # et on supprime l'ancient
            os.chdir(build_dir)
    names = directory.copy() # on fini par mettre à jour les noms de cas test. Utiliser ce tableau pour faire référence aux cas test
    del directory
    # !! ------------------- !! #

