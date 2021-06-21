"""
Victor Banon Garcia
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

We provide here a python package that can be used to graph and plot TRUSt data within jupyterlab.
This work is based on the files package (a TRUST package that reads the son files).

"""

from trustutils import files as tf
import matplotlib.pyplot as plt
import pandas as pd
import numpy  as np 
import os
import re
mon_dictionnaire = {"x": "0", "y": "1", "z": "2",  "Temperature": "0"}


def lastMoment(name):
    """

    Method to return the last moment
        
    Parameters
    --------- 
    name : str 
        name of the file. 

    Returns
    ------- 
    The last time of the plot.
        
    """
    f=open(name,"r")
    return(f.readlines()[-1].split(" ")[0])

class Graph:
    
    r"""
    
    Graph is a class to plot .son file
    
    """
    def __init__(self,title=None,subtitle=None,nX=1,nY=1):
        """

        Constructor of the class Graph.
        
        Parameters
        --------- 
        title : str 
            title of the plot. 
        nX : int 
            number of plot in the x axe. 
        nY : int 
            number of plot in the Y axe. 
        
        """
        self.x_label=""
        self.y_label="" 
        self.nX=nX
        self.nY=nY 
        self.xIndice=0
        self.yIndice=0 
        if title==None:    self.title=""
        else:              self.title=title 
        if subtitle==None: self.subtitle=""
        else:              self.subtitle=subtitle
        self.flag=False
        self._reset()

    def coordonee(self):
        """ 
        
        Methode to add a data into the board.

        Parameters
        --------- 
        data : str
            Adress of the file

        Returns
        ------- 
        The coordinates  of the subplot.
        
        """
        if(self.nX==1 & self.nY==1):
            #raise ValueError("Use plot and not plot2!!!")
            return(1) 
        elif((self.nX==1)|(self.nY==1)): 
            return(max(self.xIndice,self.yIndice))
        else: 
            return(self.xIndice,self.yIndice)
        
        
    def _reset(self):  
        """ 
        
        Methode to reinitialice the plot
        
        """  
        self.fig,self.axs=plt.subplots(self.nX,self.nY,figsize=(15,15))
        self.addPlot(self.coordonee()) 
        if self.nX*self.nY!=1: self.fig.suptitle(self.subtitle) 

    def add(self,x,y,coordonee,marker="-",color=None,label="",title=None):
        """
        
        Methode to add a curve to the plot from a point sonde.
        
        Parameters
        --------- 
        data : str
            Adress of the file. 
            title of the curve .
            
        """ 
        if not (title is None): self.title=title
        
        self.addPlot(coordonee) 
        ### On plot les données ### 
        
        if color is None: 
                self.subplot.plot(x,y,marker,label=label)
        else: 
                self.subplot.plot(x,y,marker,label=label,color=color) 
        

        self.subplot.legend()
        ## On ajoute des titres 
        self.subplot.set(xlabel=self.x_label,ylabel=self.y_label)
            
    def addPlot(self,coordonee):   
        """
        
        Methode to add a plot/subplot.
        
        Parameters
        --------- 
        coordonee : str
            Adress of the file. 
            
        """  
        
        if(self.nX==1 & self.nY==1):
            #raise ValueError("Use plot and not plot2!!!")
            self.flag=True
        elif (self.nX==1)|(self.nY==1): self.xIndice=coordonee
        else: 
            self.xIndice=coordonee[0]
            self.yIndice=coordonee[1]
            
        if self.flag : self.subplot=self.axs
        else: self.subplot=self.axs[self.coordonee()]
        self.subplot.grid()  
        self.subplot.set_title(self.title)  
        
    def addPoint(self,data,color=None,marker="-",var="x",start=0,end=1000000000000,label=None,param="Time"):
        """
        
        Methode to add a curve to the plot from a point sonde.
        
        Parameters
        --------- 
        data : str
            Adress of the file.
        color : str
            Color of the curve.
        marker : str
            symbol of the ploted line.
        var :  str
            coordinate we want to plor.
        start : float
            Time start.
        end :  float
            Time end.
        label : str
            title of the curve .
            
        """
        
        if label==None:
            label=data
        
        donne =tf.SonPOINTFile(data, None) 
        ### On recupere le nom des variables ### 
        self.y_label=donne.getEntries()[0]   
        self.x_label=donne.getXLabel()
        T=[]   
        x=[]
        y=[]
        z=[]
        for i in donne.getEntries(): 
            T.append(donne.getValues(i)[1])
            
        t=donne.getValues(self.y_label)[0] 
        
        tmp=re.findall(r"[-+]?\d*\.\d+|\d+", donne.getEntries()[0])
        x.append(float(tmp[0]))
        y.append(float(tmp[1]))
        varTMP=len(y) ### Variable pour connaitre la taille du vecteur ### 
        if len(tmp)<2:
            z.append(tmp[2])
        else:  
            z.append(list(np.zeros(varTMP))[0])

        df = pd.DataFrame({"x": x, "y": y,"z":z})
        dt = pd.DataFrame({"Time": t})
        
        ### Jointure interne ###
        r=pd.merge(df.assign(key=0), dt.assign(key=0), on='key').drop('key', axis=1)
        r=r.sort_values(by=['Time','x','y','z'])  
        
        for i in range(len(T)):
            r[str(i)]=T[i]
        
        r=r[(r["Time"]<end)&(r["Time"]>start)] 
        ### On plot les données ### 
        if color is None: 
                self.subplot.plot(list(r[param]),list(r[mon_dictionnaire[var]]),marker,label=label)
        else: 
                self.subplot.plot(list(r[param]),list(r[mon_dictionnaire[var]]),marker,label=label,color=color) 
        

        self.subplot.legend()
        ## On ajoute des titres 
        self.subplot.set(xlabel=self.x_label,ylabel=self.y_label)
    
    ## Faut tester
    def visu(self,xmin=None,xmax=None,ymin=None,ymax=None):
        """
        
        Methode for visualize all the data.
        
        Parameters
        ---------  
        xmin : float
            Minimun of the ploted interval of x.
        xmax : float
            Maximun of the ploted interval of x.
        ymin : float
            Minimun of the ploted interval of y.
        ymax : float
            Maximun of the ploted interval of y.
            
        """ 
        if self.nY*self.nX==1: 
            plt.legend()
            plt.gca().set_xlim([xmin,xmax])
            plt.gca().set_ylim([ymin,ymax]) 
            
        plt.show()   
        
    ## addSegment
    def addSegment(self,data,antecedant,param="Time",value=0,start=0,end=1000000,color=None,marker="-",label=None,var="x",nb=1):
        """
        
        Methode to add a curve to the plot from a segment sonde.
        
        Parameters
        ---------  
        data : str
            Adress of the file.
        color: str
            Color of the curve.
        Marker: str
            symbol of the ploted line.
        var :
            coordinate we want to plor.
        start : float
            Time start.
        end : float 
            Time end.
        label : str
            title of the curve .
        antecedant : str
            ??????????????????

        Returns
        ------- 
        
        """ 
        
        if label==None:
            label=data
        # On plot le dernier instant
        if(value==0)&(param=="Time"):
            value=float(lastMoment(data)) 
        donne =tf.SonSEGFile(data, None) 
        # On recupere le nom des variables 
        self.y_label=donne.getPointEntries()[0]   
        self.x_label=donne.getXLabel()
        x=[]
        y=[]
        z=[]
        t=donne.getValues(self.y_label)[0]
        T=[]      
        for i in donne.getPointEntries():
            tmp=re.findall(r"[-+]?\d*\.\d+|\d+", i)
            x.append(float(tmp[0]))
            y.append(float(tmp[1]))
            varTMP=len(y) ### Variable pour connaitre la taille du vecteur ### 
            if len(tmp)>2:
                z.append(tmp[2])
            else:   
                z.append(list(np.zeros(varTMP))[0])
        
        for i in donne.getPointEntries(): 
            T.append(list(donne.getValues(i)[1]))
        T=np.resize(np.array(T).T,(len(T)*len(T[0])))
        df = pd.DataFrame({"x": x, "y": y,"z":z})
        dt = pd.DataFrame({"Time": t}) 
        r=pd.merge(df.assign(key=0), dt.assign(key=0), on='key').drop('key', axis=1)
        r=r.sort_values(by=['Time','x','y','z'])   
        tempVar=0
        for i in T:
            r[str(tempVar)]=T
            tempVar+=tempVar
            
        r=r[(start<=r["Time"])&(end>=r["Time"])] 
        # On plot les données
        #print(r)
        
        condition=(round(r[param],nb)==round(value,nb))
        #condition=(abs(r[param]-value)<0.00000000000000001)
        
        if(color==""):
            self.subplot.plot(list(r[condition][antecedant]),list(r[condition][mon_dictionnaire[var]]),marker,label=label)
        else :
            self.subplot.plot(list(r[condition][antecedant]),list(r[condition][mon_dictionnaire[var]]),marker,label=label,color=color)  



        self.subplot.legend()
        ## On ajoute des titres 
        self.subplot.set(xlabel=self.x_label,ylabel=self.y_label)
         
    def label(self,x_label,y_label):
        """
        
        Methode to change labels.  

        Parameters
        ---------  
        x_label : str 
            Label of x.
        y_label :str
            Label of y.

        Returns
        ------- 
        
        """
        self.subplot.set(xlabel=x_label,ylabel=y_label)
        
class Table: #ancian tableau  
    r"""
    
    Class to plot a Table.
    
    """
    def __init__(self,columns): 
        """
        
        Constructor of the class Tableau. 

        Parameters
        ---------  
        columns : str 
            Name of the columns. 

        Returns
        ------- 
        
        """
        self.columns=columns 
        self._reset()

    def _reset(self):
        """ 
        
        Methode to reinitialice the board 
        
        """
        self.df=pd.DataFrame(columns=self.columns)
    
    def addLigne(self,ligne,name):        
        """
        
        Methode to add a a row to the board. 

        Parameters
        --------- 
        data : ligne
            Row of the board.
        name : str 
            Name of the board. 

        Returns
        ------- 
        
        """
        dftmp=pd.DataFrame(ligne, columns=self.columns,index=[name])
        self.df=self.df.append(dftmp)
        
    def load(self,data,name):
        """ 
        
        Methode to add a data into the board.

        Parameters
        --------- 
        data : str
            Adress of the file
        name : str 
            Name of the row. 

        Returns
        ------- 
        
        """
        self.addligne([list(np.loadtxt(data, dtype=double ))],"name")
    
     
