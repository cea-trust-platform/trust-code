"""
Victor Banon Garcia
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

We provide here a python package that can be used to run validation files from jupyterlab.

"""

import os
from IPython.display import Image
from subprocess import Popen, PIPE, STDOUT
import subprocess

listCases = []
##trustrun.py -> que l’on utilisera à terme pour encapsuler un run d’un cas TRUST depuis Jupyter (Victor je t’expliquerai)

##On veut codé ici tout la manipulation du .data

class TRUSTCase(object):
    """
    Class which allows the user to execute the validation cases 
    """
    def __init__(self, directory, caseName, nbProcs=1):
        self.dir_ = directory
        self.name_ = caseName
        self.nbProcs_ = nbProcs

    def substitute(self):
        pass

    def changeMesh(self, initialMeshName, newMeshName):
        pass

    def showDataset(self):
        return None


def dumpData(fiche,list_Trust_user_word=[]):
    """
    Visualize .data file.
    
    Parameters
    ---------
    
    fiche: str
        Adress of the file
    list_Trust_user_word : str array
        List of word the user wants to color in red.
    
    Returns
    -------
    
    A plot of the .data file.
    
    """
    ###############################################################
    # Voici un lien qui explique comment on manipule les couleur  #
    #                                                             #
    # https://stackabuse.com/how-to-print-colored-text-in-python/ #
    ###############################################################
    list_Trust_classe=["fields","Partition","Read","Conduction","solveur implicite","solveur gmres","Post_processing","diffusion","initial_conditions","boundary_conditions","Probes"]

    f=open(os.getenv("TRUST_ROOT")+"/doc/TRUST/Keywords.txt","r")
    tmp=[]
    for i in f.readlines():
        i=i.replace("|"," ")
        i=i.replace("\n"," ")
        if len(i)==2:
            i=i.replace(i,"")
        else:
            tmp.append(i)

    list_Trust_keywork =tmp
    f =open(fiche,"r")
    tmp=f.readlines()
    test=[]
    for j in tmp:
        flag=0
        if j[0]=="#":
            flag=1
            j="\033[38;5;88m"+j+"\033[0;0m "
        j=" "+j
        j=j.replace("\t","\t ")
        if flag==0:
            for i in list_Trust_keywork:
                j=j.replace(i, "\033[38;5;28m"+i+"\033[0;0m ")
        if flag==0:
            for i in list_Trust_classe:
                j=j.replace(i, "\033[38;5;4m"+i+"\033[0;0m ")
        if flag==0:
            for i in list_Trust_user_word:
                #tmp=tmp.replace(i, "\033[196;5;30m"+i+"\033[0;0m ") # en gras
                j=j.replace(i, "\033[38;5;196m"+i+"\033[0;0m ")      # en rouge
        test.append(j)
        #print(j)
    print(''.join(test))

def editData(fiche):
    """

    Lauch Gedit
    
    Parameters
    ---------
    fiche: str
        Adress of the file 
    
    Returns
    -------
    None
    
    """
    os.system('gedit '+ fiche)

    return

def addCase(directoryOrCase, caseName=None, nbProcs=None):
    """
    
    Add a case to run.
    
    Parameters
    ---------
    directoryOrCase: str
        Adress of the file 
    caseName : str
        name of the file
    nbProcs : int
        Number of proceseurs
        
    Returns
    -------
    None
    
    """
    global listCases
    if caseName is None:
        if nbProcs is None:
            raise ValueError("addCase method must be either called with a TRUSTCase instance or with a directory, and case name!!")
        case = directoryOrCase
    else:
        case = TRUSTCase(directoryOrCase, caseName, nbProcs)
    listCases.append(case)
    return case

def runCases(verbose=0,parallel=False):
    """
    
    Lance trust pour une fiche de Validation
    
    Parameters
    ---------
    verbose: bool
        Whether to print the console output of the run.
        
    Returns
    -------
    None
    
    """
    global listCases
    
    stream = os.popen("cp -ar src/ build")

    origin=os.getcwd();

    path=origin+"/build"

    stream = os.popen('echo Returned output')

    if verbose: print(stream.read())

    os.chdir(path)

    if os.path.exists("./prepare"):
        subprocess.check_output("chmod u+x prepare", shell=True)
        
        output = subprocess.check_output("./prepare " , shell=True)
        if verbose:
            print(output.decode('ascii'))
        
    # On attend pour que python aille le temps de creer le fichier avant qu'il aille le rechercher.

    for case in listCases:
        direc = case.dir_

        if direc=="." or direc is None:
            direc=case.name_
        else:
            direc=direc+"/"+ case.name_

        direct=path+"/"+case.dir_

        os.chdir(direct)

        if os.path.exists("./pre_run"):
            process = Popen("chmod u+x pre_run", shell=True)
            if verbose==0:
                subprocess.run("./pre_run "+path+'/'+direc, shell=True, stdout=PIPE)
            else:
                process = Popen("./pre_run "+path+'/'+direc, shell=True, stdout=PIPE)
                print(process.stdout.read())
                
        if parallel: comande="trust "+path+'/'+direc+" PAR_jdd "+str(case.nbProcs_)
        else:        comande="trust "+path+'/'+direc
                
        if verbose==0:
            subprocess.run("trust "+path+'/'+direc, shell=True, stdout=PIPE)
        else:
            process = Popen("trust "+path+'/'+direc, shell=True, stdout=PIPE)
            print(process.stdout.read().decode('ascii'))

        if os.path.exists("./post_run"):
            process = Popen("chmod u+x post_run", shell=True, stdout=PIPE)
            if verbose==0:
                subprocess.run("./post_run "+path+'/'+direc, shell=True, stdout=PIPE)
            else:
                process = Popen("./post_run "+path+'/'+direc, shell=True,stdout=PIPE)
                print(process.stdout.read())

        os.chdir(path)
