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
from trustutils.jupyter.run import BUILD_DIRECTORY, saveFileAccumulator

pd.set_option("display.notebook_repr_html", True)
pd.set_option("display.max_rows", None)

def loadText(data, index_column=0, nb_column=-1, transpose=True, dtype="float", skiprows=0):
    """
    Method for loading and saving files.
        
    Parameters
    --------- 
    data : str 
        name of the file we want to save. 
    index_column : int 
        Index of the first column (default=0) 
    nb_column : int 
        Number of columns loaded (default=None) 
    transpose : bool
        if we transpose the matrix (default=True)
    dtype : str
        type of the data (default="float")
    skiprows : int
        initial lines skip when reading (default=0)

    Returns
    ------- 
    matrix : array
        matrix.
    """
    origin = os.getcwd()
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
    os.chdir(origin)

    return matrix

def read_csv(data, comment="#", delim_whitespace=True,**kwargs):
    """
    Method for loading files (wrap pandas.read_csv() function).
        
    Parameters
    --------- 
    data : str 
        name of the file we want to save. 
    comment : char, default = "#"
        Character indicating that the remainder of line should not be parsed.
    delim_whitespace : bool, default = True
        Specifies whether or not whitespace (e.g. ' ' or '\t') will be used as the sep delimiter.
    kwargs : dictionary
        additional properties available in pandas.read_csv() options

    Returns
    ------- 
    pandas Dataframe
    """
    origin = os.getcwd()
    os.chdir(BUILD_DIRECTORY)

    df = pd.read_csv(data, comment=comment, delim_whitespace=delim_whitespace,**kwargs)

    saveFileAccumulator(data)
    os.chdir(origin)

    return df


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

    """
    
    Graph is a class to plot .son file
    
    """

    def __init__(self, title=None, subtitle=None, nX=1, nY=1, size=[12, 10], label_size=14, title_size=24, legend_size=14):
        """

        Constructor of the class Graph.
        
        Parameters
        --------- 
        title : str 
            title of the Graph 
        subtitle : str 
            title of the first subplot. 
        nX : int 
            number of plot in the x axe (default=1) 
        nY : int 
            number of plot in the Y axe (default=1)
        size : [float,float] 
            Image's size (x,y) of the plot (default=[12,10])
        
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
        if (nX==1 and nY==1):
            self.subtitle = title
        elif subtitle == None:
            self.subtitle = ""
        else:
            self.subtitle=subtitle
        self.flag = False
        self.label_font_size = label_size
        self.title_font_size = title_size
        self.legend_font_size = legend_size
        self._reset()

    def coordonee(self):
        """ 
        
        Method to add a data into the board.

        Parameters
        --------- 
        None

        Returns
        ------- 
        The coordinates  of the subplot.
        
        """
        if self.nX == 1 and self.nY == 1:
            return 1
        elif (self.nX == 1) or (self.nY == 1):
            return max(self.xIndice, self.yIndice)
        else:
            return (self.xIndice, self.yIndice)

    def _reset(self):
        """ 
        
        Method to reinitialize the plot.
        
        """
        plt.rc("xtick", labelsize=self.label_font_size)  # Font size
        plt.rc("ytick", labelsize=self.label_font_size)
        plt.rcParams.update({"font.size": self.legend_font_size})
        self.fig, self.axs = plt.subplots(self.nX, self.nY, figsize=(self.size[0] * self.nY, self.size[1] * self.nX))
        if self.nX * self.nY != 1:
            for ax in self.axs.reshape(-1):
                ax.axis("off")
            self.fig.suptitle(self.title, fontsize=self.title_font_size)
        self.addPlot(self.coordonee(), self.subtitle)

    def add(self, x, y, marker="-", label=None, title=None, xIndice=None, yIndice=None, **kwargs):
        """
        
        Method to add a curve to the plot from a point probe.
        
        Parameters
        --------- 
        x : float array
            x coordinates.  
        y : float array
            data to plot.  
        marker : str
            symbol of the ploted line (default="-")
        label : str
            label of the curve.
        title : str
            title of the curve.  
        xIndice : int
            first coordinates of the subplot list 
        yIndice : int
            second coordinates of the subplot list 
        kwargs : dictionary
            additional properties available in matplotlib.pyplot.plot
            
        """
        if not xIndice is None:
            self.xIndice = xIndice
        if not yIndice is None:
            self.yIndice = yIndice

        self.addPlot(self.coordonee(), title)

        ### On plot les données ###
        self.subplot.plot(x, y, marker, label=label, **kwargs)

        if label:
            self.subplot.legend()

        ## On ajoute des titres
        self.subplot.set(xlabel=self.x_label, ylabel=self.y_label)

    def addPlot(self, coordonee, title=""):
        """
        
        Method to add a plot/subplot.
        
        Parameters
        --------- 
        coordonee : str
            Adress of the file. 
        title : str
            Title of the plot. 
            
        """
        self.subtitle = title

        if self.nX == 1 and self.nY == 1:
            self.flag = True
        elif (self.nX == 1) or (self.nY == 1):
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
        self.subplot.set_title(self.subtitle)

    def addPoint(self, data, marker="-", compo=0, label="", func=None, **kwargs):
        """

        Method to add a curve to the plot from a point probe.

        Parameters
        ---------
        data : str
            Adress of the file.
        marker : str
            symbol of the ploted line (default="-")
        compo :  str
            component we want to plot (for vector fields) (default=0)
        label : str
            title of the curve .
        func : function
            function to apply to data before plot (ex: computation of error)
        kwargs : dictionary
            additional properties available in matplotlib.pyplot.plot

        """
        path = os.path.join(BUILD_DIRECTORY, data)
        if not os.path.exists(path): raise ValueError(f"addPoint : The path ({path}) does not exist!")
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

        Method to add a curve to the plot from a segment of probes.

        Parameters
        ---------
        data : str
            Adress of the file.
        time : double
            time of the parameter. If None, it equals to zero.
        marker: str
            symbol of the ploted line (default="-")
        label : str
            title of the curve .
        compo :  str
            component we want to plot (for vector fields) (default=0)
        func : function Y = func(X,Y)
            function to apply to data before plot (ex: computation of error)
        funcX : function X = funcX(X)
            function to apply to xcoordinates before plot
        kwargs : dictionary
            additional properties available in matplotlib.pyplot.plot

        Returns
        ------- 
        
        """

        path = os.path.join(BUILD_DIRECTORY, data)
        if not os.path.exists(path): raise ValueError(f"addSegment : The path ({path}) does not exist!")
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
        
        Method to add a curve to the plot from a dt_ev file.
        
        Parameters
        --------- 
        data : str
            Adress of the file.
        marker : str
            symbol of the ploted line.
        var :  str
            coordinate we want to plot (default="residu=max|Ri|")
        start : float
            Time start.
        end :  float
            Time end.
        label : str
            title of the curve .
        kwargs : dictionary
            additional properties available in matplotlib.pyplot.plot
            
        """

        if label == "":
            label = data.split(".")[0]

        path = os.path.join(BUILD_DIRECTORY, data)
        donne = tf.DTEVFile(path, None)

        saveFileAccumulator(data)
        ### On recupere le nom des variables ###
        self.y_label = var
        self.x_label = donne.getXLabel()

        r = pd.DataFrame({"Time": donne.getValues(var)[0], var: donne.getValues(var)[1]})

        r = r[(r["Time"] <= end) & (r["Time"] >= start)]

        ### On plot les donnees ###
        self.subplot.plot(list(r["Time"]), list(r[self.y_label]), marker, label=label, **kwargs)

        if not label is None:
            self.subplot.legend()
        ## On ajoute des titres
        self.subplot.set(xlabel=self.x_label, ylabel=self.y_label)

    def scale(self, xscale="linear", yscale="linear"):
        """
        xscale : str
            The axis scale type to apply to the x-axis (default="linear")
        yscale : str
            The axis scale type to apply to the y-axis (default="linear")
        """

        self.subplot.set_xscale(xscale)
        self.subplot.set_yscale(yscale)

    def visu(self, xmin=None, xmax=None, ymin=None, ymax=None):
        """
        
        Method to visualize all the data.
        
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
        
        Method to change labels.

        Parameters
        ---------  
        x_label : str 
            Label of x.
        y_label : str
            Label of y.
        kwargs : dictionary
            additional properties available in matplotlib.pyplot.subplot
        
        Returns
        ------- 
        
        """
        self.subplot.set(xlabel=x_label, ylabel=y_label, **kwargs)

    def legend(self, **kwargs):
        """
        
        Method to change labels.

        Parameters
        ---------  
        **kwargs :
            option of matplotlib.pyplot.legend

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
        
        Method to reinitialize the board.
        
        """
        self.df = pd.DataFrame(columns=self.columns)

    def addLine(self, ligne, name):
        """
        
        Method to add a row to the board.

        Parameters
        --------- 
        ligne : list
            Row of the board.
        name : str 
            Name of the board. 

        Returns
        ------- 
        
        """
        dftmp = pd.DataFrame(ligne, columns=self.columns, index=[name])
        self.df = self.df.append(dftmp)

    def setTitle(self,title):
        """
        
        Method to add title to the board.

        Parameters
        --------- 
        title : str 
            caption to ass. 

        Returns
        ------- 
        
        """
        self.df = self.df.style.set_caption(title)
        

    def load(self, data, name):
        """ 
        
        Method to add a data into the board.

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
        self.addLine([list(np.loadtxt(path, dtype=float))], name)
        saveFileAccumulator(data)

    def loadPoint(self, data, name):
        """ 
        
        Method to add a data into the board.

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
        self.addLine([[a]], name)

    def sum(self, name):
        """ 
        
        Method to sum data in a column.

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
        self.addLine([row], "Total")

    def _repr_latex_(self):
        """ This method is invoked by 'display()' when generating a PDF """
        # Note the escape=False to preserve math formulas ...
        s= self.df.to_latex()
        s = s.replace(r"\textbackslash", "")
        s = s.replace(r"\$","$")
        s = "\\begin{center} \n %s \n \end{center}" % s
        return s

    def _repr_html_(self):
        """ This method is invoked by 'display()' when generating the HTML view of
        the notebook. """
        return self.df.to_html()
