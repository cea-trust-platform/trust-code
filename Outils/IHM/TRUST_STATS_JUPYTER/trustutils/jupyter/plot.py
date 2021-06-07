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

def get_cmap(n, name='hsv'):
    """
    
    Returns a function that maps each index in 0, 1, ..., n-1 to a distinct 
    RGB color; the keyword argument name must be a standard mpl colormap name.
    
    """
    return plt.cm.get_cmap(name, n)




class Graph:
    
    r"""
    
    Graph is a class to plot .son file
    
    """
    def __init__(self,title):
        """

        Constructor of the class Graph.
        
        Parameters
        --------- 
        title : str 
            title of the plot. 
        
        """
        self._reset()
        self.x_label=""
        self.y_label="" 
        self.title=title

    def _reset(self):  
        """ 
        
        Methode to reinitialice the plot
        
        """
        self.cmap=get_cmap(100) 
        self.ncolor=0
        plt.figure(figsize=(10, 8), dpi=100) 
        plt.grid()   
        
    def addPoint(self,data,color='k',marker="b",var="x",start=0,end=1000000000000,label="donnée"):
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
        donne =tf.SonPOINTFile(data, None) 
        # On recupere le nom des variables 
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
        if len(tmp)<2:
            z.append(tmp[2])

        df = pd.DataFrame({"x": x, "y": y})
        dt = pd.DataFrame({"Time": t})

        r=pd.merge(df.assign(key=0), dt.assign(key=0), on='key').drop('key', axis=1)
        r=r.sort_values(by=['Time','x','y'])  
        
        for i in range(len(T)):
            r[str(i)]=T[i]
        
        r=r[(r["Time"]<end)&(r["Time"]>start)]
        #print(r)
        ## On plot les données  
        plt.plot(list(r["Time"]),list(r[mon_dictionnaire[var]]),marker,label=label,color=color)
    
    
    ## Faut tester
    def visu(self,xmin,xmax,ymin,ymax):
        """
        
        Methode for visualize all the data.
        
        Parameters
        ---------  
        xmin : float
            ???????????
        xmax : float
            ???????????
        ymin : float
            ???????????
        ymax : float
            ???????????
            
        """
        plt.gca().set_xlim([xmin,xmax])
        plt.gca().set_ylim([ymin,ymax])
        plt.legend()
        ## On ajoute des titres 
        plt.xlabel(self.x_label)
        plt.ylabel(self.y_label)
        plt.title(self.title)
        plt.show()        
        
    ## addSegment
    def addSegment(self,data,antecedant,param="Time",value=0,start=0,end=1000000,color="",marker="b",label="donnée",var="x"):
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
        donne =tf.SonSEGFile(data, None) 
        # On recupere le nom des variables 
        self.y_label=donne.getEntries()[0]   
        self.x_label=donne.getXLabel()
        x=[]
        y=[]
        z=[]
        t=donne.getValues(self.y_label)[0]
        T=[]      
        for i in donne.getEntries():
            tmp=re.findall(r"[-+]?\d*\.\d+|\d+", i)
            x.append(float(tmp[0]))
            y.append(float(tmp[1]))
            if len(tmp)<2:
                z.append(tmp[2])
        
        for i in donne.getEntries(): 
            T.append(list(donne.getValues(i)[1]))
        T=np.resize(np.array(T).T,(len(T)*len(T[0])))
        df = pd.DataFrame({"x": x, "y": y})
        dt = pd.DataFrame({"Time": t}) 
        r=pd.merge(df.assign(key=0), dt.assign(key=0), on='key').drop('key', axis=1)
        r=r.sort_values(by=['Time','x','y'])   
        tempVar=0
        for i in T:
            r[str(tempVar)]=T
            tempVar+=tempVar
            
        r=r[(start<=r["Time"])&(end>=r["Time"])] 
        # On plot les données
        
        
        if(color==""):
            plt.plot(list(r[round(r[param],1)==round(value,1)][antecedant]),list(r[round(r[param],1)==round(value,1)][mon_dictionnaire[var]]),marker,label=label,color=self.cmap(self.ncolor))
            self.ncolor=self.ncolor+5
        else :
            plt.plot(list(r[round(r[param],1)==round(value,1)][antecedant]),list(r[round(r[param],1)==round(value,1)][mon_dictionnaire[var]]),marker,label=label,color=color) 
         
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
        self.x_label(x_label) 
        self.y_label(y_label)   
        
class Table: #ancian tableau  
    r"""
    
    Class to plot a board
    
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
    
     
