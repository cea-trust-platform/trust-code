"""
Victor Banon Garcia
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

We provide here a python package that can be used to run validation files from jupyterlab.

"""

import os
from subprocess import Popen, PIPE, STDOUT
import subprocess
from trustutils.jupyter import plot
from datetime import date
    
def Introduction(auteur):
    from IPython.display import display, Markdown
    today = date.today()
    dat = today.strftime("%d/%m/%Y")

    display(Markdown('## Introduction \n Validation made by : '+auteur+'\n \n Report generated '+dat))
    
def Description(text): 
    from IPython.display import display, Markdown
    display(Markdown('### Description \n'+text))
    
def Parameters_TRUST(Version="",Param=[]): 
    from IPython.display import display, Markdown
    Binary="Binary"
    
    text='### Parameters TRUST \n * Version TRUST:'+Version+'\n Binary:'+Binary
    for i in Param:
        text=text+"\n"+i
    display(Markdown(text)) 
    
def Test_cases(): 
    from IPython.display import display, Markdown
    text='### Test cases \n'
    display(Markdown(text)) 
    
listCases = []
##trustrun.py -> que l’on utilisera à terme pour encapsuler un run d’un cas TRUST depuis Jupyter (Victor je t’expliquerai)

##On veut codé ici tout la manipulation du .data 
class TRUSTCase(object):
    """
    Class which allows the user to execute the validation cases 
    
    """
    def __init__(self, directory, dataName, caseName, nbProcs=1): ### Victor coment: nbProcs perhaps should be a string
        """
        Initialisation of the class  

        Parameters
        ---------

        directory: str
            Adress of the file
        caseName : str
            Name of the case we want to run.
        nbProcs : int
        Number of Processeurs we want to use.

        Returns
        -------
        None
        """
        self.dir_ = directory
        self.data = dataName
        self.name_ = caseName
        self.nbProcs_ = nbProcs
         
    def substitute(self,subtitue,remplassant):
        """
        Substitue a part of the plot   

        Parameters
        --------- 

        None

        Returns
        -------

        None

        """ 
        
        origin=os.getcwd(); 
        path=origin+"/build/"+self.dir_
        os.chdir(path)
        
        flag=True
        f=open(self.name_,"r") 
        texte=f.readlines()
        txt=""
        
        for i in texte:
            
            if i == subtitue : 
                i=remplassant
                txt=txt+i
                flag=False
            else:txt=txt+i
        if flag:
            raise ValueError("Not Found: "+subtitue)
        
        #close the input file
        f.close()
        #open the input file in write mode
        f = open(self.name_, "wt")
        # overwrite the input file with the resulting data
        f.write(txt)
        # close the file
        f.close()
        
        os.chdir(origin)
            

    def copy(self,directory=None,dataName=None,caseName=None,nbProcs=None):
        """
        Substitue a part of the plot   

        Parameters
        ---------

        None

        Returns
        -------

        None

        """ 

        if directory==None: 
            directory="Copy_"+self.dir_ 
        if dataName==None: 
            directory="Copy_"+self.data 
        if caseName==None:  
            caseName ="Copy_"+self.name_ 
        if nbProcs  ==None: 
            nbProcs  =self.nbProcs_   
        
        
        return(addCase(directoryOrCase=directory,dataName=self.data,caseName=caseName, nbProcs=nbProcs) ) 

    def showDataset(self):
        """
        Show data file   

        Parameters
        ---------

        None

        Returns
        -------

        None

        """
        dumpData(self.name_)

    def addfile(self,file,complementDir=""):
        """
        Loremp Ipsum

        Parameters
        ---------

        None

        Returns
        -------

        None

        """ 

        #directory data file
        dir1="./src/"+complementDir
        #directory in which the file will be executed
        dir2="./build/"+self.dir_+"/" 
        #print('cp '+dir1+file+' '+dir2+file)
        status = subprocess.call('cp '+dir1+file+' '+dir2+file, shell=True)


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
    # Voici un lien qui explique comment on manipule les couleurs #
    #                                                             #
    # https://stackabuse.com/how-to-print-colored-text-in-python/ #
    ###############################################################
    
    
    ### Class that I found interesting to change theirs colors. Victor ###
    list_Trust_classe=["fields","Partition","Read","Conduction","solveur implicite","solveur gmres","Post_processing","diffusion","initial_conditions","boundary_conditions","Probes"]

    ### Import and change de layout of TRUST Keyword ###
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
    
    ### Import and Underline important words of the data file  ###
    f =open("build/" + fiche,"r")
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
    
    ### Print the coloured .data file ###
    print(''.join(test)) 

def addCase(directoryOrCase,dataName ,caseName=None , nbProcs=1,complementDir=""):
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
    case: TRUSTcase
        Case Trust we want to lauch.
    
    """
    global listCases
    
    if caseName==None: caseName=dataName
    
    if caseName is None:
        if nbProcs is None:
            ### test if there is an error. ###
            raise ValueError("addCase method must be either called with a TRUSTCase instance or with a directory, and case name!!")
        case = directoryOrCase
    else:
        case = TRUSTCase(directoryOrCase,dataName, caseName, nbProcs)
    listCases.append(case)

    origin=os.getcwd();
    # creation fichier build
    path=origin+"/build"  
    #print(path)
    if not os.path.exists(path): subprocess.run("cp -ar src/ build", shell=True)
    os.chdir(path) 
    
    # creation des fichier      
    if directoryOrCase!="." :arborescence(directoryOrCase) 
    os.chdir(origin)  
    
    #directory data file
    dir1="./src/"+complementDir
    #directory in which the file will be executed
    dir2="./build/"+directoryOrCase+"/" 
    #print('cp '+dir1+dataName+' '+dir2+caseName)
    status = subprocess.call('cp '+dir1+dataName+' '+dir2+caseName, shell=True)
        

    return case

def runCases(verbose=0,baltik=False):
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

    stream = os.popen('echo Returned output')
    if verbose: print(stream.read())
        
    ### Change the root to build file ###
    origin=os.getcwd();
    path=origin+"/build"  
    os.chdir(path)

    ### Run Prepare ###
    if os.path.exists("./prepare"):
        subprocess.check_output("chmod u+x prepare", shell=True)
        
        output = subprocess.check_output("./prepare" , shell=True)
        if verbose: 
            print("./prepare")
            print(output.decode('ascii')) 
 
    for case in listCases:
        direc = case.dir_

        if direc=="." or direc is None:
            direc=case.name_
        else:
            direc=direc+"/"+ case.name_

        direct=path+"/"+case.dir_

        ### Root Case directory ### 
        try: 
            os.chdir(direct) 
        except:
            print("An exception occurred") 
        
        ### Run pre_run ###
        if os.path.exists("./pre_run"):
            subprocess.check_output("chmod u+x pre_run", shell=True)
            ### Mit en comentaire parce que ./pre_run ne retourne rien et produit un bug  ###
            #output = subprocess.check_output("./pre_run" , shell=True)
            subprocess.run("./pre_run" , shell=True)
            if verbose:
                print("."+direct+"/pre_run")
            #    print(output.decode('ascii'))
        
        ### Run Case ###
        if not baltik:  
                baseName = case.name_.split(".")[0]
                err_file = baseName + ".err"
                out_file = baseName + ".out"
                comande = "trust %s %s 2>%s 1>%s"  % (case.name_, str(case.nbProcs_), err_file, out_file) 
                output = subprocess.call(comande , shell=True)
                if verbose:
                    print(comande)
                    print(output.decode('ascii'))  
            
        ### baltik ###
        if baltik: 
            os.chdir('/export/home/vicban/projects/Gitspace/TRUST_perte_charge/') 
            comande="./TRUST_perte_charge  tests/Reference/New/build/"+direc
            print(comande)
            output = subprocess.check_output(comande , shell=True)
            if verbose:
                print(comande)
                print(output.decode('ascii'))  
            
        ### Run post_run ###
        if os.path.exists("./post_run"):
            process = Popen("chmod u+x post_run", shell=True, stdout=PIPE)
            if verbose==0:
                subprocess.run("./post_run "+path+'/'+direc, shell=True, stdout=PIPE)
            else:
                process = Popen("./post_run "+path+'/'+direc, shell=True,stdout=PIPE)
                print(process.stdout.read())

        ### Root in build ###
        os.chdir(origin)
        
        
def tablePerf():
    """
    
    Show the table of performance
    
    Parameters
    ---------
    None
        
    Returns
    -------
    None
    
    """
    global listCases 
    
    ### Change the root to build file ###
    origin=os.getcwd();
    path = origin + "/build" 
    
    columns=["host",'system','Total CPU Time','CPU time/step', 'number of cell']
    Table=plot.Table(columns)  


    for case in listCases:
        direc = case.dir_

        if direc=="." or direc is None:
            direc=case.name_
        else:
            direc=direc+"/"+ case.name_

        direct=path+"/"+case.dir_
        
        os.chdir(direct) 
        
        nom=case.name_.split(".")[0]
        
        comande=os.environ["TRUST_ROOT"]
        comande=comande+"/Validation/Outils/Genere_courbe/scripts/extract_perf "+ nom 
        subprocess.run(comande , shell=True) 
        
        ### Run post_run ###
        f=open(nom+".perf","r") 
        
        row=(f.readlines()[0].replace("\n","").split(" ")[1:])

        ### Root in build ###
        Table.addLigne([row],case.dir_+"/"+nom) 
        
        
        os.chdir(path)

    os.chdir(origin) 
    return(Table.df)
        
        
def dumpList():
    """
    
    Show the list of cases
    
    Parameters
    ---------
    None
        
    Returns
    -------
    None
    
    """
    
    global listCases 
    return(listCases)


def arborescence(directoryOrCase):
    """
    
    Show the list of cases
    
    Parameters
    ---------
    None
        
    Returns
    -------
    None
    
    """
    
    direct=directoryOrCase.split("/")
    if (direct[0]==".")|(direct[0]==""): direct=direct[1:]
    for i in direct:
        if not os.path.exists(i):
            if not os.path.exists(i):subprocess.call("mkdir "+i, shell=True)
            os.chdir(i)  
