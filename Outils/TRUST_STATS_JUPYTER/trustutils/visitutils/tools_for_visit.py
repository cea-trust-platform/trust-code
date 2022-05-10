"""
Victor Banon Garcia & Morad Ben Tayeb
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

We provide here a python package that can be used to execute visit commands from python script.
This package can be used with jupyter and stats package.

"""


import os
import numpy as np
import subprocess
from trustutils.jupyter.filelist import FileAccumulator
import matplotlib.pyplot as plt
import matplotlib.image as mpimg

from .message import GestionMessages


def visitTmpFile_(justFile=False):
    """ Internal ... """
    file = "tmp_visit.py"
    if justFile:
        return file
    from ..jupyter.run import BUILD_DIRECTORY

    return os.path.join(BUILD_DIRECTORY, file)


def checkProbe(lata, son, sizepoint=0.05):
    """

    Method used to plot the probes.

    Parameters
    ---------
    lata: str
        adress of the lata file.
    son: str
        adress of the son file (probe).
    taillepoint: float
        size of the probe.

    Returns
    -------

    """
    # Visualisation du maillage
    S = Show(lata, "Mesh", "dom", plotmesh=False)
    # Visualisation du probe
    S.visitCommand("dbs = ('" + son + "')")
    S.visitCommand("ActivateDatabase(dbs) ")
    S.visitCommand("AddPlot('Pseudocolor','value')")
    S.visitCommand("DrawPlots()")
    S.visitCommand("pc=PseudocolorAttributes()")
    # type de points
    S.visitCommand("pc.pointType=pc.Octahedron")
    # taille des points
    S.visitCommand("pc.pointSize=" + str(taillepoint))
    S.visitCommand("print(pc)")
    S.visitCommand("SetPlotOptions(pc)")

    S.plot()


def setFrame(self, numero=-1):
    """

    To set frame in visit

    Parameters
    ---------
    numero: int
        If numero = -1, the last frame chosen, else it select the frame in visit.

    Returns
    -------

    """
    self.time = str(numero)
    with open(visitTmpFile_(), "a") as f:
        if numero == -1:  # last frame
            f.write("SetTimeSliderState(TimeSliderGetNStates()-1)\n")
        else:
            f.write("SetTimeSliderState(" + str(numero) + ")\n")
    f.close()


def saveFile(file, field, name, active=False):
    """

    Save the used fie in the build directory(Visitfile).

    Parameters
    --------- 
    fichier : str
        The .lata file we want to plot its mesh with visit.  
    field : str
        The field we want to plot.  
    name : str
        The name of the field.  

    Returns
    -------
    None

    """
    if active:

        origin = os.getcwd()
        from ..jupyter.run import BUILD_DIRECTORY

        path = os.path.join(origin, BUILD_DIRECTORY)
        os.chdir(path)

        FileAccumulator.active = True
        if field=="Mesh" or field=="Subset":
            FileAccumulator.AppendVisuMesh(file,name)
        else:
            field, loc, dom = FileAccumulator.ParseDirectName(name)
            FileAccumulator.AppendVisuComplex(file, dom, field, loc)
            FileAccumulator.WriteToFile("used_files", mode="a")

        os.chdir(origin)


def showMesh(fichier, mesh="dom"):
    """
    Methods to plot the mesh from a .lata file.

    Parameters
    ---------
    fichier : str
        The .lata file we want to plot its mesh with visit.  
    mesh : str
        name of the mesh.  
            
    Returns
    -------
        a Visit plot 
    """

    field = Show(fichier=fichier, field="Mesh", name=mesh, mesh=mesh, plotmesh=False)
    field.plot()


def showField(fichier, field, name, mesh="dom", plotmesh=True, time=-1, size=10, max=None, min=None):
    """
    Methods to plot a field from a .lata file.

    Parameters
    ---------
    fichier : str
        The .lata file we want to plot its mesh with visit.  
    field : str
        The field we want to plot.  
    name : str
        The name of the field.  
    mesh : str
        The name of the mesh.  
            
    Returns
    -------
        a Visit plot 
    """
    field = Show(fichier, field, name, mesh=mesh, plotmesh=plotmesh, time=time, size=size, max=max, min=min)
    field.plot()


class Show(object):
    """ 
    Class Show, which allow to use visit command in a python environment
    """

    def __init__(
        self, fichier="", field="", name="", nX=1, nY=1, mesh="dom", plotmesh=True, time=-1, empty=False, size=10, title="", max=None, min=None, active=True, visitLog=False, verbose=0, show=True,
    ):
        """
        Constructeur

        Parameters
        ---------
        fichier : str
            The .lata file we want to plot its mesh with visit.  
        field : str
            The field we want to plot.  
        name : str
            The name of the field.  
        mesh : str
            The name of the mesh.  
        plotmesh : bool
            If true plot the mesh asociate with .lata file.  
        time : int
            Time of the plot. 
        empty : bool
            If True returns a empty plot.  
        size : int
            Size of the image.  
        title : str 
            title of the plot. 
        max : float
            Maximun value ploted.  
        min : float
            Minimum value ploted.  
        show : bool
            Show the image  

        Returns
        -------
        None      
        """
        #
        if not empty:
            if fichier == "" or field == "" or name == "":
                raise ValueError("Error: A file needed!!")
            else:
                # Remove former PNG files
                # Formatage du chemin
                tmp = fichier.rsplit("/", 1)
                if len(tmp) != 1:
                    tmp = tmp[0] + "/"
                else:
                    tmp = ""

                from glob import glob
                from ..jupyter.run import BUILD_DIRECTORY

                path = os.path.join(BUILD_DIRECTORY, tmp)
                for f in glob(path + "*.png"):
                    os.remove(f)

        self.gestMsg = GestionMessages(verbose, "")
        self.field = field
        self.fichier = fichier
        # Nom et adresse
        self.name = name
        self.nom = str(self.field + self.name)
        # Mesh(if true il la visualise)
        self.plotmesh = plotmesh
        # Mesh
        self.mesh = mesh
        # Temps
        self.time = str(0)
        # Coordonée
        self.xIndice = 0
        self.yIndice = 0
        # dimension 2D
        self.nX = nX
        self.nY = nY
        # dimension 1D
        self.axes = nY
        #
        self.flag = False
        # Temps
        self.time = time
        # Photo vide(if true)
        self.empty = empty
        # size image
        self.size = size
        # title plot
        self.title = title
        # max/min des valeurs du plot.
        self.max = max
        self.min = min
        self.visitLog = visitLog
        self.show = show

        if not empty:
            name = name.replace("/", "_")  # avoid error when / in name
            saveFile(fichier, field, name, active)

        self._reset()

    def _reset(self):
        """
        Reset the class

        Parameters
        ---------
        

        Returns
        -------
        
        """
        # Assure qu'il n'y a pas deja une image,  si oui, il l'efface
        if self.show:
            plt.rc("xtick", labelsize=14)  # Font size
            plt.rc("ytick", labelsize=14)
            plt.rcParams.update({"font.size": 14})
            self.fig, self.axs = plt.subplots(self.nX, self.nY, figsize=(self.size * self.nY, self.size * self.nX))
            if self.nX * self.nY != 1:
                for ax in self.axs.reshape(-1):
                    ax.axis("off")
        self.addPlot(self.coordonee(), self.title)

    def _genAddPlot(self, arg1, arg2):
        s = "try:\n"
        s += "  a = AddPlot(%s, %s)\n" % (arg1, arg2)
        s += "  if a == 0: exit(-1)\n"
        s += "except Exception as e:\n"
        s += "  print(e)\n"
        s += "  exit(-1)\n"
        return s

    def addPlot(self, coordonee, title=""):
        """ 
        
        Methode pour afficher un graphiques.
        

        Parameters
        ---------
        coordonee : array int
            Coordonée du plot
        

        Returns
        -------
        
        """
        with open(visitTmpFile_(), "w") as f:
            # Option pour les vecteur

            f.write("VectorAtts = VectorAttributes()\n")
            f.write("VectorAtts.glyphLocation = VectorAtts.AdaptsToMeshResolution  # AdaptsToMeshResolution, UniformInSpace\n")
            f.write("VectorAtts.useStride = 1\n")
            f.write("VectorAtts.stride = 1\n")
            f.write("VectorAtts.nVectors = 400\n")
            f.write("VectorAtts.lineWidth = 0\n")
            f.write("VectorAtts.scale = 0.25\n")
            f.write("VectorAtts.scaleByMagnitude = 1\n")
            f.write("VectorAtts.autoScale = 1\n")
            f.write("VectorAtts.headSize = 0.25\n")
            f.write("VectorAtts.headOn = 1\n")
            f.write("VectorAtts.colorByMag = 1\n")
            f.write("VectorAtts.useLegend = 1\n")
            f.write("VectorAtts.vectorColor = (0, 0, 0, 255)\n")
            f.write('VectorAtts.colorTableName = "Default"\n')
            f.write("VectorAtts.invertColorTable = 0\n")
            f.write("VectorAtts.vectorOrigin = VectorAtts.Tail  # Head, Middle, Tail\n")
            f.write("VectorAtts.minFlag = 0\n")
            f.write("VectorAtts.maxFlag = 0\n")
            f.write("VectorAtts.limitsMode = VectorAtts.OriginalData  # OriginalData, CurrentPlot\n")
            f.write("VectorAtts.min = 0\n")
            f.write("VectorAtts.max = 1\n")
            f.write("VectorAtts.lineStem = VectorAtts.Line  # Cylinder, Line\n")
            f.write("VectorAtts.geometryQuality = VectorAtts.Fast  # Fast, High\n")
            f.write("VectorAtts.stemWidth = 0.08\n")
            f.write("VectorAtts.origOnly = 1\n")
            f.write("VectorAtts.glyphType = VectorAtts.Arrow  # Arrow, Ellipsoid\n")
            f.write("VectorAtts.animationStep = 0\n")
            f.write("SetDefaultPlotOptions(VectorAtts)\n")

        f.close()

        if not self.empty:
            self.addField(fichier=self.fichier, field=self.field, name=self.name, mesh=self.mesh, plotmesh=self.plotmesh)

        if self.show:
            if (self.nX == 1) & (self.nY == 1):
                # raise ValueError("Use plot and not plot2!!!")
                self.flag = True
            elif (self.nX == 1) | (self.nY == 1):
                self.xIndice = coordonee
            else:
                self.xIndice = coordonee[0]
                self.yIndice = coordonee[1]

            if self.flag:
                self.subplot = self.axs
            else:
                try:
                    self.subplot = self.axs[self.coordonee()]
                except:
                    print("ERROR with " + self.name + " => the subplot does not exist, inconsistent index")
                    return

            self.subplot.grid()
            if title != "":
                self.title = title
                self.subplot.set_title(self.title)

    def addField(self, fichier=None, field=None, name=None, mesh=None, plotmesh=True, min=None, max=None):
        """ 
        
        Method for adding a Field to a plot.
        Parameters
        ---------
        fichier : str
            The .lata file we want to plot its mesh with visit.  
        field : str
            The field we want to plot.  
        name : str
            The name of the field.  
        mesh : str
            The name of the mesh.  
        plotmesh : bool
            If true plot the mesh asociate with .lata file.  
        min : float
            Minimum value ploted.  
        max : float
            Maximun value ploted.  

        Returns
        -------
        
        """
        if fichier is None:
            fichier = self.fichier
        if min is None:
            min = self.min
        if max is None:
            max = self.max
        if (field is None) | (name is None):
            raise ValueError("Error: need field or/and name!!")

        with open(visitTmpFile_(), "a") as f:
            if not fichier == None:
                f.write("dbs = ('" + fichier + "') \n")
                f.write("ActivateDatabase(dbs) \n")
            if not mesh is None and plotmesh and not field == "Mesh" and not field == "Histogram":
                f.write(self._genAddPlot("'Mesh'", "'" + mesh + "'"))
            f.write(self._genAddPlot("'" + field + "'", "'" + name + "'"))
            f.write("DrawPlots() \n")
            # Boucle if pour roter le plot 3d de 30 degre ,selon l'axe x et y (2 rotations).
            f.write("p=PseudocolorAttributes()\n")
            if not min is None:
                f.write("p.minFlag=1\n")
                f.write("p.min=" + str(min) + "\n")
            if not max is None:
                f.write("p.maxFlag=1\n")
                f.write("p.max=" + str(max) + "\n")
            f.write("SetPlotOptions(p)\n")
        f.close()

    def visitCommand(self, string):
        """ 
        
        Method for adding command lines to the .py file.
        Parameters
        ---------
        string : str
            Comand for Visit.

        Returns
        -------
        
        """
        with open(visitTmpFile_(), "a") as f:
            f.write(string + "\n")
        f.close()

    def _dumpLogTail(self):
        from ..jupyter.run import BUILD_DIRECTORY

        visLog = os.path.join(BUILD_DIRECTORY, "visit.log")
        with open(visLog) as f:
            txt = f.readlines()
            txt = "".join(txt[-7:])
            print(txt)

    def executeVisitCmds(self):
        """

        Display the visit plot in Jupyter.

        Parameters
        ---------
        field : str
            The field we want to plot.  
        name : str
            The name of the field.  


        Returns
        -------
        
        """
        from ..jupyter.run import BUILD_DIRECTORY

        name = self.name.replace("/", "_")
        with open(visitTmpFile_(), "a") as f:
            f.write("# on retire le nom du user sur les images \n")
            f.write("annotation=GetAnnotationAttributes()\nannotation.SetUserInfoFlag(0)\nSetAnnotationAttributes(annotation)\n")
            # f.write("ResizeWindow(1,160,156) \n")
            f.write("s = SaveWindowAttributes() \n")
            f.write("s.fileName = '" + self.fichier + name + str(self.time) + "_' \n")
            f.write("s.format = s.PNG \n")
            f.write("s.progressive = 1 \n")
            f.write("SetSaveWindowAttributes(s) \n")
            f.write("SaveWindow()")
        f.close()
        origin = os.getcwd()
        os.chdir(BUILD_DIRECTORY)
        outp = subprocess.run("visit -nowin -cli -s %s 1>> visit.log 2>&1" % visitTmpFile_(justFile=True), shell=True)
        os.chdir(origin)
        if outp.returncode != 0:
            print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
            print("VisIt execution error!! See tail of the VisIt output below.")
            print("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!")
            self._dumpLogTail()
        if self.visitLog:
            self._dumpLogTail()

    #         else   :
    #             os.system("visit -nowin -cli -s  tmp.py") # Faire sa independante de cette methode

    def coordonee(self):
        """

        Return plot coordinates.
        
        Parameters
        --------- 
        
        Returns
        -------
        coordonne : array int
            Coordonné.
        
        """
        if (self.nX == 1) & (self.nY == 1):
            return 0
        elif (self.nX == 1) | (self.nY == 1):
            return max(self.xIndice, self.yIndice)
        else:
            return (self.xIndice, self.yIndice)

    def insert(self):
        """

        Add a photo to the Multiple plot.
        
        Parameters
        ---------  

        Returns
        -------
        
        
        """
        from ..jupyter.run import BUILD_DIRECTORY

        name = self.name.replace("/", "_")
        fName = self.fichier + name + str(self.time)  # +"_0000.png"
        from glob import glob

        listFiles = glob(os.path.join(BUILD_DIRECTORY, fName + "*.png"))
        listFiles.sort(key=os.path.getmtime)
        pth = listFiles[-1]
        # pth = os.path.join(BUILD_DIRECTORY, fName)
        if not os.path.exists(pth):
            print("Unable to open VisIt image result '%s'!!" % pth)
            return False
        img = mpimg.imread(pth)

        self.subplot.imshow(img)
        self.subplot.axis("off")
        return True

    def add(
        self, fichier, field, name, xIndice=0, yIndice=0, time=-1, mesh="dom", title="", plotmesh=True, max=None, min=None,
    ):
        """

        Add a plot.
        
        Parameters
        ---------  
        fichier : str
            The .lata file we want to plot its mesh with visit.  
        field : str
            The field we want to plot.  
        name : str
            The name of the field.    
        xIndice : int
            Indice of the x axe.
        yIndice : int
            Indice of the y axe.
        mesh : str
            The name of the mesh.  
        plotmesh : bool
            If true plot the mesh asociate with .lata file.  
        time : int
            Time of the plot. 
        size : int
            Size of the image.  
        max : float
            Maximun value ploted.  
        min : float
            Minimum value ploted.

        Returns
        -------
        
        """
        # finalization of the previous subplot
        self.plot(show=False)

        self.xIndice = xIndice
        self.yIndice = yIndice
        self.fichier = fichier
        self.field = field
        self.name = name
        self.time = time
        self.mesh = mesh
        self.plotmesh = plotmesh
        self.title = title
        self.max = max
        self.min = min

        self.addPlot(self.coordonee(), self.title)

    def plot(self, show=True):
        """

        Visualize the graph.
        
        Parameters
        ---------  

        Returns
        -------
        
        """
        self.setFrame(self.time)
        self.executeVisitCmds()
        if self.show:
            self.insert()
        if self.show and show:
            plt.show()

    def addMesh(self, mesh):
        """

        Add another mesh to the Visit Plot. 

        Parameters
        ---------
        mesh: str
            name of the mesh

        Returns
        -------
        
        """
        with open(visitTmpFile_(), "a") as f:
            f.write(self._genAddPlot("'Mesh'", "'%s'" % mesh))
            f.write("DrawPlots() \n")
        f.close()

    def pas(self, numero=0):
        """

        Increase the time step.

        Parameters
        ---------
        field : str
            The field we want to plot.  
        name : str
            The name of the field.  


        Returns
        -------
        
        """
        self.time = str(numero)
        with open(visitTmpFile_(), "a") as f:
            f.write("SetTimeSliderState(" + str(numero) + ")\n")
        f.close()

    def slice(self, origin=[0, 0, 0], normal=[1, 0, 0], var=None, all=0, type_op="slice"):

        with open(visitTmpFile_(), "a") as f:
            f.write('AddOperator("Slice",%d)\n' % all)
            f.write("s=SliceAttributes()\n")
            f.write("s.SetOriginType(s.Point)\n")
            f.write("s.SetOriginPoint(%s,%s,%s)\n" % (origin[0], origin[1], origin[2]))
            f.write("s.SetAxisType(s.Arbitrary)\n")
            if var == "x":
                f.write("s.SetNormal(-1,0,0)\ns.SetUpAxis(0,1,0)\n")
            elif var == "y":
                f.write("s.SetNormal(0,-1,0)\ns.SetUpAxis(0,0,1)\n")
            elif var == "z":
                f.write("s.SetNormal(0,0,1)\ns.SetUpAxis(0,1,0)\n")
            else:
                f.write("s.SetNormal(%s,%s,%s)\n" % (normal[0], normal[1], normal[2]))
            if type_op == "slice":
                f.write("s.SetProject2d(0)\n")
            elif type_op == "slice2d":
                f.write("s.SetProject2d(1)\n")
            f.write("SetOperatorOptions(s,0,%d)\n" % all)
        f.close()

    def lineout(self, a, b):
        """

        Draw a line.

        Parameters
        ---------
        a : array float
            The first point of the segment.  
        b : array float
            The last  point of the segment.  


        Returns
        -------
        
        """
        with open(visitTmpFile_(), "a") as f:
            f.write("Lineout((" + str(a[0]) + "," + str(a[1]) + "),(" + str(b[0]) + "," + str(b[1]) + '),("default")) \n')  # ajouter Z
            f.write("DeleteWindow() \n")
        f.close()

    def pasSuivant(self):
        """

        Adavnce one time step.

        Parameters
        --------- 

        Returns
        -------
        
        """
        self.time = str(int(self.time) + 1)
        with open(visitTmpFile_(), "a") as f:
            f.write("TimeSliderNextState()\n")
        f.close()

    def point(self, x, y, z=0):
        """

        Draw a point.

        Parameters
        ---------
        field : str
            The field we want to plot.  
        name : str
            The name of the field.  


        Returns
        -------
        
        """
        self.time = str(int(self.time) + 1)
        with open(visitTmpFile_(), "a") as f:
            f.write('QueryOverTime("Pick")\n')
        f.close()

    def setFrame(self, numero=-1):
        """

        For setting frame in visit

        Parameters
        ---------
        numero: int
            If numero = -1, the last frame chosen, else it select the frame in visit

        Returns
        -------
        
        """
        self.time = str(numero)
        with open(visitTmpFile_(), "a") as f:
            if numero == -1:  # last frame
                f.write("SetTimeSliderState(TimeSliderGetNStates()-1)\n")
            else:
                f.write("SetTimeSliderState(" + str(numero) + ")\n")
        f.close()

    def zoom2D(self, coords=[]):

        if len(coords) != 4:
            self.gestMsg.ecrire(GestionMessages._ERR, "4 parameters expected in zoom2D options")

        with open(visitTmpFile_(), "a") as f:
            f.write("va = GetView2D()\n")
            f.write("va.SetWindowCoords(%s,%s,%s,%s)\n" % (coords[0], coords[1], coords[2], coords[3]))
            f.write("SetView2D(va)\n")
        f.close()

    def zoom3D(self, var=[]):

        if len(var) != 3:
            self.gestMsg.ecrire(GestionMessages._ERR, "3 parameters expected in zoom3D options")

        with open(visitTmpFile_(), "a") as f:
            f.write("va = GetView3D()\n")
            f.write("va.SetImagePan(%s,%s)\n" % (var[0], var[1]))
            f.write("va.SetImageZoom(%s)\n" % (var[2]))
            f.write("SetView3D(va)\n")
        f.close()

    def rotation3D(self, angle=[]):

        if len(angle) != 3:
            self.gestMsg.ecrire(GestionMessages._ERR, "3 parameters expected in rotation3D options")

        with open(visitTmpFile_(), "a") as f:
            f.write("va = GetView3D()\n")
            f.write("va.RotateAxis(1,0)\n")
            f.write("va.RotateAxis(0," + str(angle[0]) + ")\n")
            f.write("SetView3D(va)\n")
            f.write("va.RotateAxis(1," + str(angle[1]) + ")\n")
            f.write("SetView3D(va)\n")
            f.write("va.RotateAxis(2," + str(angle[2]) + ")\n")
            f.write("SetView3D(va)\n")
        f.close()

    def normal3D(self, vector=[]):

        if len(vector) != 3:
            self.gestMsg.ecrire(GestionMessages._ERR, "3 parameters expected in normal3D options")

        with open(visitTmpFile_(), "a") as f:
            f.write("va = GetView3D()\n")
            f.write("va.viewNormal=(%s,%s,%s)\n" % (vector[0], vector[1], vector[2]))
            f.write("SetView3D(va)\n")
        f.close()

    def up3D(self, coords=[]):

        if len(coords) != 3:
            self.gestMsg.ecrire(GestionMessages._ERR, "3 parameters expected in up3D options")

        with open(visitTmpFile_(), "a") as f:
            f.write("va = GetView3D()\n")
            f.write("va.viewUp=(%s,%s,%s)\n" % (coords[0], coords[1], coords[2]))
            f.write("SetView3D(va)\n")
        f.close()

    def visuOptions(self, operator=[]):

        list_base = {"no_axes": 0, "no_triad": 0, "no_bounding_box": 0, "no_databaseinfo": 0, "no_legend": 0}
        lst = list(list_base.keys())

        with open(visitTmpFile_(), "a") as f:
            for type_op in operator:
                if type_op not in lst:
                    self.gestMsg.ecrire(
                        GestionMessages._ERR, "Unknown keyword. We expect for an operator parameter, and not: %s . Expected possible keywords: %s" % (type_op, lst),
                    )
                if type_op == "no_axes":
                    f.write("annotation=GetAnnotationAttributes()\n")
                    f.write("try:\n annotation.GetAxes2D().SetVisible(0)\n")
                    f.write(" annotation.GetAxes3D().SetVisible(0)\n")
                    f.write(" annotation.GetAxes2D().SetVisible(0)\n")
                    f.write("except:\n")
                    f.write(' print "option no_axes incompatible with this version of visit"\n pass\n')
                    f.write("SetAnnotationAttributes(annotation)\n")
                elif type_op == "no_triad":
                    f.write("annotation=GetAnnotationAttributes()\n")
                    f.write("try:\n annotation.GetAxes2D().SetVisible(0)\n")
                    f.write(" annotation.GetAxes3D().SetTriadFlag(0)\n")
                    f.write("except:\n")
                    f.write(' print "option no_triad incompatible with this version of visit"\n pass\n')
                    f.write("SetAnnotationAttributes(annotation)\n")
                elif type_op == "no_bounding_box":
                    f.write("annotation=GetAnnotationAttributes()\n")

                    f.write("try: annotation.GetAxes3D().SetBboxFlag(0)\n")
                    f.write("except:\n")
                    f.write(' print "option no_boundig_box incompatible with this version of visit"\n pass\n')
                    f.write("SetAnnotationAttributes(annotation)\n")
                elif type_op == "no_databaseinfo":
                    f.write("annotation=GetAnnotationAttributes()\n")

                    f.write("annotation.SetDatabaseInfoFlag(0)\n")
                    f.write("SetAnnotationAttributes(annotation)\n")
                elif type_op == "no_legend":
                    f.write("annotation=GetAnnotationAttributes()\n")
                    f.write("annotation.SetLegendInfoFlag(0)\n")
                    f.write("SetAnnotationAttributes(annotation)\n")
        f.close()

    def blackVector(self):

        with open(visitTmpFile_(), "a") as f:
            f.write("vb=VectorAttributes()\nvb.SetColorByMag(0)\nvb.SetUseLegend(0)\n")
            f.write("SetPlotOptions(vb)\n")
        f.close()

    def visuHistogram(self, min, max, numBins):

        with open(visitTmpFile_(), "a") as f:
            f.write("p=HistogramAttributes()\n")
            f.write("p.SetMinFlag(1)\n")
            f.write("p.SetMin(%s)\n" % min)
            f.write("p.SetMaxFlag(1)\n")
            f.write("p.SetMax(%s)\n" % max)
            f.write("p.SetNumBins(%s)\n" % numBins)
            f.write("SetPlotOptions(p)\n")
        f.close()

    def meshColor(self, color="red"):
        """

        Change the Color of the mesh in red.

        Parameters
        --------- 

        Returns
        -------
        
        """
        listcolor = ["red", "green", "blue", "black"]
        if color not in listcolor:
            # from string import join
            self.gestMsg.ecrire(GestionMessages._ERR, "color expected in %s and not %s" % (listcolor, color))
        with open(visitTmpFile_(), "a") as f:
            f.write("red=(255,0,0,255);green=(0,255,0,255);black=(0,0,0,255);blue=(0,0,255,255)\n")
            f.write("MeshAtts = MeshAttributes() \n")
            f.write("MeshAtts.SetMeshColorSource(1) \n")
            f.write("MeshAtts.SetMeshColor(%s) \n" % color)
            f.write("SetPlotOptions(MeshAtts) \n")
        f.close()

    def meshTrans(self):
        """

        Turn transparent the mesh.

        Parameters
        ---------
        field : str
            The field we want to plot.  
        name : str
            The name of the field.  


        Returns
        -------
        
        """
        with open(visitTmpFile_(), "a") as f:
            f.write("MeshAtts = MeshAttributes() \n")
            f.write("MeshAtts.opaqueColorSource = MeshAtts.Background  # Background, OpaqueCustom \n")
            f.write("MeshAtts.opacity = 0.254902 \n")
            f.write("SetPlotOptions(MeshAtts) \n")
        f.close()

    def change(self, nom, atribut):
        """

        Change argument value.

        Parameters
        ---------
        nom : str
            Name of the argument..  
        atribut : str/int/float
            The new value of the argument.  


        Returns
        -------
        
        """
        flag = 0
        if nom == "field":
            self.field = atribut
            flag = 1
        if nom == "name":
            self.name = atribut
            flag = 1
        if nom == "field":
            self.mesh = atribut
            flag = 1
        if nom == "mesh":
            self.time = atribut
            flag = 1
        if nom == "fichier":
            self.fichier = atribut
            flag = 1
        if nom == "nom":
            self.nom = atribut
            flag = 1
        if flag:
            print("ERREUR: Attribut Inexistant")


class export_lata_base:
    """
    
    Class to export profile or fields from lata files.
     
    """

    def __init__(self, fichier, field, name, saveFile, frame=-1):
        """

        Reset the class

        Parameters
        ---------
        fichier : str
            The .lata file we want to plot its mesh with visit.  
        field : str
            The field we want to plot. Egg. "PSEUDOCOLOR".
        name : str
            The name of the field.   
        saveFile : str
            Path to the new (.curve, .okc, ...) file created
        frame : int 
            Current frame selected.

        Returns
        -------
        None
        
        """
        self.fichier = fichier
        self.field = field
        self.name = name
        self.saveFile = saveFile
        self.frame = frame
        self._reset()

    def _reset(self):
        """

        Reset the class
        
        """

        # Assure qu'il ne y a pas deja un fichier, si oui, il l'efface
        file = visitTmpFile_()
        if os.path.exists(file):
            os.remove(file)
        with open(file, "a") as f:
            f.write("dbs = ('" + self.fichier + "') \n")
            f.write("ActivateDatabase(dbs) \n")
        self.setFrame(self.frame)
        self.addPlot(self.field, self.name)
        f.close()

    def getFrames(self):
        """

        CMethod for getting the Frames.
        
        """
        with open(visitTmpFile_(), "a") as f:
            f.write('with open("frames.txt","w") as f:\n')
            f.write("   f.write(str(TimeSliderGetNStates()-1))\n")
            f.write("f.close()\n")
            f.write("sys.exit()\n")
        f.close()
        self.run()
        with open("frames.txt", "r") as f:
            N = f.read()
        f.close()
        os.remove("frames.txt")
        return int(N)

    def getDimensions(self):
        r"""

        Get dimensions of mesh.
        Warning, it has to launch visit and write the result in a file.  

        """
        with open(visitTmpFile_(), "a") as f:
            f.write("SetQueryOutputToString() \n")
            f.write('B = Query("Grid Information") \n')
            f.write('with open("dim.txt","w") as f:\n')
            f.write("   f.write(B) \n")
            f.write("f.close()\n")
            f.write("sys.exit()\n")
        f.close()
        self.run()
        with open("dim.txt", "r") as f:
            N = f.read()
            res = re.findall(r"[\d]*[.][\d]+|[\d]+", N)[1:]
        f.close()
        os.remove("dim.txt")
        res = [int(r) - 1 for r in res]
        return res

    def addPlot(self, field, name):
        """

        Add a new field to the plot.

        Parameters
        ---------
        field : str
            The field we want to plot.  
        name : str
            The name of the field.  


        Returns
        -------
        None
        
        """
        with open(visitTmpFile_(), "a") as f:
            f.write(self._genAddPlot("'%s'" % field, "'%s'" % name))
            f.write("DrawPlots() \n")
        f.close()

    def _genAddPlot(self, arg1, arg2):
        s = "try:\n"
        s += "  a = AddPlot(%s, %s)\n" % (arg1, arg2)
        s += "  if a == 0: exit(-1)\n"
        s += "except Exception as e:\n"
        s += "  print(e)\n"
        s += "  exit(-1)\n"
        return s

    def setFrame(self, numero=-1):
        """

        To set frame in visit

        Parameters
        ---------
        numero: int
            If numero = -1, the last frame chosen, else it select the frame in visit

        Returns
        -------
        
        """

        with open(visitTmpFile_(), "a") as f:
            if numero == -1:  # last frame
                f.write("SetTimeSliderState(TimeSliderGetNStates()-1)\n")
            else:
                f.write("SetTimeSliderState(" + str(numero) + ")\n")
        f.close()

    def lineout(self, a, b):
        """

        Draw a line.

        Parameters
        ---------
        a : float array
            First point.  
        b : float array
            Second point.  


        Returns
        -------
        None
        
        """
        dim = len(a)
        with open(visitTmpFile_(), "a") as f:
            if dim == 2:
                f.write("Lineout((" + str(a[0]) + "," + str(a[1]) + "),(" + str(b[0]) + "," + str(b[1]) + '),("default")) \n')
            if dim == 3:
                f.write("Lineout((" + str(a[0]) + "," + str(a[1]) + "," + str(a[2]) + "),(" + str(b[0]) + "," + str(b[1]) + "," + str(b[2]) + '),("default")) \n')
            f.write("SetActiveWindow(2) \n")
        f.close()

    def point(self, x, y, z=0):
        """

        Draw a point

        Parameters
        ---------
        x : float
            x coordenate.  
        y : float
            y coordenate.  
        z : float
            z coordenate.   

        Returns
        -------
        
        """
        with open(visitTmpFile_(), "a") as f:
            f.write('QueryOverTime("Pick")\n')
        f.close()

    def query(self, start_point, end_point, num_samples=10, mode="Lineout"):
        """

        Extract a section of data from a .lata file

        Parameters
        ---------
        start_point : float array
            First point 
        end_point   : float array
            Second point 
        num_samples : float array
            First point 
        mode : str
            Type of section  


        Returns
        -------
        None
        """
        if mode == "Lineout":
            with open(visitTmpFile_(), "a") as f:
                s = 'Query("{0}", end_point={1}, num_samples={2}, start_point={3}, use_sampling=0, vars=( "{4}"))\n'
                f.write(s.format(mode, end_point, num_samples, start_point, self.name))
                f.write("SetActiveWindow(2) \n")
            f.close()

    def maximun(self, name="Max", time=-1):
        """

        Extract Max of data from a .lata file

        Parameters
        --------- 
        name : str
            It tells if we should return the max or the min. 
        time : int
            Momment we calcule the max/min.. 

        Returns
        -------
        None
        """
        self.setFrame(numero=-1)
        with open(visitTmpFile_(), "a") as f:
            f.write('Query("Max", use_actual_data=1) \n')
            f.write('f=open("' + name + self.saveFile + '","w") \n')
            f.write("f.write(str(GetQueryOutputValue())) \n")
            f.write("f.close() \n")
            f.write("print(GetQueryOutputValue())\n")

            f.write("""sys.exit()""")
        self.run()

    def minimun(self, name="Min", time=-1):
        """

        Extract Max of data from a .lata file

        Parameters
        ---------
        name : str
            It tells if we should return the max or the min. 
        time : int
            Momment we calcule the max/min.. 

        Returns
        -------
        None
        """
        self.setFrame(numero=-1)
        with open(visitTmpFile_(), "a") as f:
            f.write('Query("Min", use_actual_data=1) \n')
            f.write('f=open("' + name + self.saveFile + '","w") \n')
            f.write("f.write(str(GetQueryOutputValue())) \n")
            f.write("f.close() \n")
            f.write("print(GetQueryOutputValue())\n")

            f.write("""sys.exit()""")
        self.run()

    def save(self, type="Curve2D"):
        """

        Export and saves the .curve file
        
        """
        with open(visitTmpFile_(), "a") as f:
            f.write("dbAtts = ExportDBAttributes() \n")
            f.write("dbAtts.filename = " + "'" + self.saveFile + "'" + "\n")
            f.write('dbAtts.db_type = "' + type + '" \n')
            f.write('dbAtts.variables = ("default", "vec") \n')
            f.write("ExportDatabase(dbAtts) \n")
            f.write("sys.exit()")
        f.close()
        self.run()

    def run(self):
        """

        Run the visit command in terminal.

        """
        from ..jupyter.run import BUILD_DIRECTORY

        origin = os.getcwd()
        os.chdir(BUILD_DIRECTORY)
        outp = subprocess.run("visit -nowin -cli -s %s 1>> visit.log 2>&1" % visitTmpFile_(justFile=True), shell=True)
        os.chdir(origin)
