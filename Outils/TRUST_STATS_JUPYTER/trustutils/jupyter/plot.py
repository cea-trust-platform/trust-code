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
import numpy as np
import os
import re
import math
from trustutils.jupyter.filelist import FileAccumulator
from trustutils.jupyter.run import BUILD_DIRECTORY

pd.set_option("display.notebook_repr_html", True)
pd.set_option("display.max_rows", None)

def _repr_latex_(self):
    return "\\begin{center} \n %s \n \end{center}" % self.to_latex()

pd.DataFrame._repr_latex_ = _repr_latex_ # To display pandas table as latex table into report

def saveFileAccumulator(data):
    """
    Method for saving files.
        
    Parameters
    --------- 
    data : str 
        name of the file we want to save. 

    Returns
    ------- 
    """

    path = os.getcwd()
    os.chdir(BUILD_DIRECTORY)

    FileAccumulator.active = True
    FileAccumulator.Append(data)
    FileAccumulator.WriteToFile("used_files", mode="a")

    os.chdir(path)


def loadText(data, index_column=0, nb_column=-1, transpose=True, dtype="float", skiprows=0):
    """
    Method for loading and saving files.
        
    Parameters
    --------- 
    data : str 
        name of the file we want to save. 
    index_column : int 
        Index of the first column. 
    nb_column : int 
        Number of columns loaded. 
    transpose : bool
        if we transpose the matrix.
    dtype : str
        type of the data.
    skiprows : int
        lines skip when reading.

    Returns
    ------- 
    matrix : array
        matrix.
    """

    os.chdir(BUILD_DIRECTORY)

    if nb_column == -1:
        nb = None
    else:
        nb = index_column + nb_column

    try:
        if transpose:
            matrix = np.loadtxt(data, dtype=dtype, skiprows=skiprows).T[index_column:nb]
        else:
            matrix = np.loadtxt(data, dtype=dtype, skiprows=skiprows)[index_column:nb]
    except:
        matrix = np.loadtxt(data, dtype=dtype, skiprows=skiprows)

    saveFileAccumulator(data)
    return matrix


def readFile(name):
    """
    Method for loading and saving files.
        
    Parameters
    --------- 
    data : str 
        name of the file we want to save. 

    Returns
    ------- 
    list of lines
    """
    origin = os.getcwd()
    os.chdir(BUILD_DIRECTORY)

    f = open(name, "r")
    data = f.read().splitlines()

    os.chdir(origin)
    saveFileAccumulator(name)

    return data


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
    f = open(name, "r")
    return f.readlines()[-1].split(" ")[0]


def timeArray(name):
    """

    Method to return the time step list.
        
    Parameters
    --------- 
    name : str 
        name of the file. 

    Returns
    ------- 
    The last time of the plot.
        
    """
    f = open(name, "r")
    time_list = []
    for i in f.readlines():
        tmp = i.split(" ")[0]
        if tmp != "#":
            time_list.append(float(tmp))
            # time_list.append(tmp)

    return time_list


class Graph:

    r"""
    
    Graph is a class to plot .son file
    
    """

    def __init__(self, title=None, subtitle=None, nX=1, nY=1, size=[12, 10]):
        """

        Constructor of the class Graph.
        
        Parameters
        --------- 
        title : str 
            title of the plot. 
        subtitle : str 
            subtitle of the first subplot. 
        nX : int 
            number of plot in the x axe. 
        nY : int 
            number of plot in the Y axe. 
        size : float*2 
            Image's size (x,y) of the plot. 
        
        """
        self.x_label = ""
        self.y_label = ""
        self.nX = nX
        self.nY = nY
        self.size = size
        self.xIndice = 0
        self.yIndice = 0
        if title == None:
            self.title = ""
        else:
            self.title = title
        if subtitle == None:
            self.subtitle = ""
        else:
            self.subtitle = subtitle
        self.flag = False
        self._reset()

    def coordonee(self):
        """ 
        
        Methode to add a data into the board.

        Parameters
        --------- 
        None

        Returns
        ------- 
        The coordinates  of the subplot.
        
        """
        if self.nX == 1 & self.nY == 1:
            return 1
        elif (self.nX == 1) | (self.nY == 1):
            return max(self.xIndice, self.yIndice)
        else:
            return (self.xIndice, self.yIndice)

    def _reset(self):
        """ 
        
        Methode to reinitialize the plot.
        
        """
        plt.rc("xtick", labelsize=14)  # Font size
        plt.rc("ytick", labelsize=14)
        #plt.rcParams.update({"font.size": 14})
        self.fig, self.axs = plt.subplots(self.nX, self.nY, figsize=(self.size[0] * self.nY, self.size[1] * self.nX))
        if self.nX * self.nY != 1:
            for ax in self.axs.reshape(-1):
                ax.axis("off")
            self.fig.suptitle(self.subtitle, fontsize=24)
        self.addPlot(self.coordonee())

    def add(self, x, y, marker="-", label=None, title=None, xIndice=None, yIndice=None, **kwargs):
        """
        
        Methode to add a curve to the plot from a point sonde.
        
        Parameters
        --------- 
        x : float array
            x coordinates.  
        y : float array
            y coordinates.  
        coordonee : int or int array
            coordinates in the subplotlist.  
        label : str
            label of the curve.
        title : str
            title of the curve.  
        xlabel : str
            label of the xaxis.
        ylabel : str
            label of the yaxis.
        kwargs : dictionary
            properties of line for matplotlib.pyplot.plot
            
        """
        if not title is None:
            self.title = title
        if not xIndice is None:
            self.xIndice = xIndice
        if not yIndice is None:
            self.yIndice = yIndice

        self.addPlot(self.coordonee())

        ### On plot les données ###
        self.subplot.plot(x, y, marker, label=label, **kwargs)

        if label:
            self.subplot.legend()

        ## On ajoute des titres
        self.subplot.set(xlabel=self.x_label, ylabel=self.y_label)

    def addPlot(self, coordonee, title=""):
        """
        
        Methode to add a plot/subplot.
        
        Parameters
        --------- 
        coordonee : str
            Adress of the file. 
        title : str
            Title of the plot. 
            
        """
        if title != "":
            self.title = title

        if self.nX == 1 & self.nY == 1:
            self.flag = True
        elif (self.nX == 1) | (self.nY == 1):
            self.xIndice = coordonee
        else:
            self.xIndice = coordonee[0]
            self.yIndice = coordonee[1]

        if self.flag:
            self.subplot = self.axs
        else:
            self.subplot = self.axs[self.coordonee()]

        self.subplot.axis("on")
        self.subplot.grid(visible=True)
        self.subplot.set_title(self.title)

    def addPoint(self, data, marker="-", compo=0, label="", func=None, **kwargs):
        """

        Methode to add a curve to the plot from a point sonde.

        Parameters
        ---------
        data : str
            Adress of the file.
        marker : str
            symbol of the ploted line.
        compo :  str
            component we want to plot (for vector fields).
        label : str
            title of the curve .
        func : function
            function to apply to data before plot (ex: computation of error)
        kwargs : dictionary
            properties of line for matplotlib.pyplot.plot

        """
        path = os.path.join(BUILD_DIRECTORY, data)
        donne = tf.SonPOINTFile(path, None)

        saveFileAccumulator(data)

        ### On recupere le nom des variables ###
        entries = donne.getEntries()

        nCompo = donne.getnCompo()
        nPoints = donne.getnPoints()

        for k in range(nPoints):
            entry = donne.getEntries()[k*nCompo + compo]
            self.y_label = entry.split()[0]
            zeLabel = label if label != "" else entry.split()[1][1:-1]
            self.x_label = donne.getXLabel()

            X, Y = donne.getValues(entry)

            if not func is None:
                Y = func(X, Y)

            ### On plot les donnees ###
            self.subplot.plot(X, Y, marker, label=zeLabel, **kwargs)

        if label != "":
            self.subplot.legend()

        ## On ajoute des titres
        self.subplot.set(xlabel=self.x_label, ylabel=self.y_label)

    def addSegment(self, data, time=None, marker="-", label="", compo=0, func=None, funcX=None, **kwargs):
        """

        Methode to add a curve to the plot from a segment sonde.

        Parameters
        ---------
        data : str
            Adress of the file.
        time : double
            time of the parameter. If None, it equals to zero.
        marker: str
            symbol of the ploted line.
        label : str
            title of the curve .
        compo :  str
            component we want to plot (for vector fields).
        nb : float
            erreur tolerated between the comparaison to find the right data.
        func : function
            function to apply to data before plot (ex: computation of error)
        kwargs : dictionary
            properties of line for matplotlib.pyplot.plot

        Returns
        ------- 
        
        """

        path = os.path.join(BUILD_DIRECTORY, data)
        if label == "":
            label = data.split(".")[0]
        # On plot le dernier instant
        donne = tf.SonSEGFile(path, None)

        saveFileAccumulator(data)

        # On recupere le nom des variables, leur dimension et la coordonnee que l'on veut tracer
        ncompo = donne.getnCompo()

        entries = donne.getEntries()

        self.y_label = entries[compo].split()[0]
        self.x_label = donne.getXLabel()
        self.start, self.end = donne.getXTremePoints()

        t = donne.getValues(entries[0])[0]
        ## find closest value of t
        if time == None:
            idx = -1
        else:
            idx = (np.abs(t - time)).argmin()

        X = donne.getXAxis()
        if funcX is not None: X = funcX(X)

        Y = []
        for i in entries[compo::ncompo]:
            Y.append(list(donne.getValues(i)[1])[idx])

        if not func is None:
            Y = func(X, Y)

        self.subplot.plot(X, Y, marker, label=label, **kwargs)

        if not label is None:
            self.subplot.legend()
        ## On ajoute des titres
        self.subplot.set(xlabel=self.x_label, ylabel=self.y_label)

    def addResidu(self, data, marker="-", var="residu=max|Ri|", start=0, end=1000000000000, label="", **kwargs):
        """
        
        Methode to add a curve to the plot from a dt_ev file.
        
        Parameters
        --------- 
        data : str
            Adress of the file.
        color : str
            Color of the curve.
        marker : str
            symbol of the ploted line.
        var :  str
            coordinate we want to plot.
        start : float
            Time start.
        end :  float
            Time end.
        label : str
            title of the curve .
            
        """

        if label == "":
            label = data.split(".")[0]

        path = os.path.join(BUILD_DIRECTORY, data)
        donne = tf.DTEVFile(path, None)

        saveFileAccumulator(data)
        # filelist.FileAccumulator.AppendFromProbe("build/"+data)
        ### On recupere le nom des variables ###
        self.y_label = var
        self.x_label = donne.getXLabel()

        r = pd.DataFrame({"Time": donne.getValues(var)[0], var: donne.getValues(var)[1]})

        r = r[(r["Time"] <= end) & (r["Time"] >= start)]

        ### On plot les donn��es ###
        self.subplot.plot(list(r["Time"]), list(r[self.y_label]), marker, label=label, **kwargs)

        if not label is None:
            self.subplot.legend()
        ## On ajoute des titres
        self.subplot.set(xlabel=self.x_label, ylabel=self.y_label)

    def scale(self, xscale="linear", yscale="linear"):
        """
        xscale : str
            The axis scale type to apply to the x-axis
        yscale : str
            The axis scale type to apply to the y-axis
        """

        self.subplot.set_xscale(xscale)
        self.subplot.set_yscale(yscale)

    def visu(self, xmin=None, xmax=None, ymin=None, ymax=None):
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
        coordonee : 
            coordinates in the subplotlist.  
            
        """
        self.subplot.set_xlim([xmin, xmax])
        self.subplot.set_ylim([ymin, ymax])

    def label(self, x_label, y_label, **kwargs):
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
        self.subplot.set(xlabel=x_label, ylabel=y_label, **kwargs)

    def legend(self, **kwargs):
        """
        
        Methode to change labels.  

        Parameters
        ---------  
        coordonee : 
            coordinates in the subplotlist.
        **kwargs :
            option of ax.legend

        Returns
        ------- 
        
        """
        self.subplot.legend(**kwargs)


class Table:  # ancien tableau
    r"""
    
    Class to plot a Table.
    
    """

    def __init__(self, columns):
        """
        
        Constructor of the class Tableau. 

        Parameters
        ---------  
        columns : str 
            Name of the columns. 

        Returns
        ------- 
        
        """
        self.columns = columns
        self._reset()

    def _reset(self):
        """ 
        
        Methode to reinitialice the board 
        
        """
        self.df = pd.DataFrame(columns=self.columns)

    def addLigne(self, ligne, name):
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
        dftmp = pd.DataFrame(ligne, columns=self.columns, index=[name])
        self.df = self.df.append(dftmp)

    def load(self, data, name):
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
        path = os.path.join(BUILD_DIRECTORY, data)
        self.addLigne([list(np.loadtxt(path, dtype=float))], name)
        saveFileAccumulator(data)

    def loadPoint(self, data, name):
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
        path = os.path.join(BUILD_DIRECTORY, data)
        tmp = tf.SonPOINTFile(path, None)
        saveFileAccumulator(data)

        a = tmp.getValues(tmp.getEntries()[0])[1][1]
        self.addLigne([[a]], name)

    def sum(self, name):
        """ 
        
        Methode to sum data in a column.

        Parameters
        --------- 
        name : str 
            Name of the column. 

        Returns
        ------- 
        
        """

        self.df[name] = self.df[name].astype(np.float64)
        total = self.df[name].sum()
        row = [""] * len(self.df.columns)
        index = self.df.columns.get_loc(name)
        row[index] = total
        self.addLigne([row], "Total")

    def _repr_latex_(self):
        """ This method is invoked by 'display()' when generating a PDF """
        # Note the escape=False to preserve math formulas ...
        s= self.df.to_latex(escape=False)
        s = s.replace("$$", "$")
        return s

    def _repr_html_(self):
        """ This method is invoked by 'display()' when generating the HTML view of
        the notebook. """
        return self.df.to_html()
