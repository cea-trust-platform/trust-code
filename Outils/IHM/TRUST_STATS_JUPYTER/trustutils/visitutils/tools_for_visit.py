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

def checkProbe(lata,son,sizepoint=0.05):
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
    S=Show(lata,"Mesh","dom",plotmesh=False)
    # Visualisation du probe
    S.visitCommand("dbs = ('build/"+son+"')")
    S.visitCommand("ActivateDatabase(dbs) ")
    S.visitCommand("AddPlot('Pseudocolor','value')")
    S.visitCommand("DrawPlots()" )
    S.visitCommand("pc=PseudocolorAttributes()")   
    # type de points
    S.visitCommand("pc.pointType=pc.Octahedron")  
    # taille des points
    S.visitCommand("pc.pointSize="+str(taillepoint))  
    S.visitCommand("print(pc)")  
    S.visitCommand("SetPlotOptions(pc)")   

    S.plot()
    
def setFrame(self,numero=-1):
    """

    To set frame in visit

    Parameters
    ---------
    numero: int
        If numero = -1, the last frame chosen, else it select the frame in visit.

    Returns
    -------

    """
    self.time=str(numero)
    with open("tmp.py", "a") as f:
        if numero == -1: # last frame
            f.write("SetTimeSliderState(TimeSliderGetNStates()-1)\n")
        else:
            f.write("SetTimeSliderState("+ str(numero) +")\n")
    f.close() 
    
def saveFile(file,field,name,active=False):
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
        
        origin=os.getcwd(); 
        path=origin+"/build/"
        os.chdir(path) 
        
        FileAccumulator.active = True
        if field=="Mesh": 
            FileAccumulator.AppendVisuMesh(path+"/"+file,name) 
        else:
            field, loc, dom = FileAccumulator.ParseDirectName(name)
            FileAccumulator.AppendVisuComplex(path+"/"+file,dom, field, loc) 
            FileAccumulator.WriteToFile("used_files", mode="a") 
        
        os.chdir(origin)
    
def showMesh(fichier,mesh="dom",dim=2,rotx=0,roty=0,rotz=0):
    """
    Methods to plot the mesh from a .lata file.

    Parameters
    ---------
    fichier : str
        The .lata file we want to plot its mesh with visit.  
    mesh : str
        name of the mesh.  
    dim : int
        Dimension of the plot.  
    rotx : int
        Rotation along the x axis.  
    roty : int
        Rotation along the y axis.  
    rotz : int
        Rotation along the z axis.  
            
    Returns
    -------
        a Visit plot 
    """

    Field=Show(fichier=fichier,field="Mesh",name=mesh,mesh=mesh,dim=dim,rotx=rotx,roty=roty,rotz=rotz,plotmesh=False)  
    Field.plot();

def showField(fichier,field,name,mesh="dom",dim=2,rotx=-45,roty=45,rotz=0,plotmesh=True,time=-1,win=False,size=100,max=None,min=None):
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
    dim : int
        Dimension of the plot.  
    rotx : int
        Rotation along the x axis.  
    roty : int
        Rotation along the y axis.  
    rotz : int
        Rotation along the z axis.  
            
    Returns
    -------
        a Visit plot 
    """ 
    Field=Show(fichier,field,name,mesh=mesh,dim=dim,rotx=rotx,roty=roty,rotz=rotz,plotmesh=plotmesh,time=-1,size=size,max=max,min=min) 
    Field.plot(); 

class Show:
    """ 
    Class Show, which allow to use visit command in a python enviroment
    """
    def __init__(self,fichier="",field="",name="",nX=1,nY=1,dim=2,mesh="dom",rotx=45,roty=45,rotz=45,plotmesh=True,time=-1,empty=False,size=100,max=None,min=None,active=True):
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
        dim : int
            Dimension of the plot.  
        rotx : int
            Rotation along the x axis.  
        roty : int
            Rotation along the y axis.  
        rotz : int
            Rotation along the z axis.  
        plotmesh : bool
            If true plot the mesh asociate with .lata file.  
        time : int
            Time of the plot. 
        empty : bool
            If True returns a empty plot.  
        size : int
            Size of the image.  
        max : float
            Maximun value ploted.  
        min : float
            Minimum value ploted.  

        Returns
        -------
        None      
        """
        #
        if not empty:
            if fichier=="" or field=="" or name=="":
                raise ValueError("Error: A file needed!!")
            else:
                # Remove former PNG files
                # Formatage du chemin
                tmp = fichier.rsplit("/",1)
                if len(tmp)!=1:
                    tmp=tmp[0]+"/"
                else:
                    tmp=""

                from glob import glob
                for f in glob("build/" + tmp + '*.png'):
                    os.remove(f)

        self.field=field
        self.fichier = "build/" + fichier
        # Nom et adresse
        self.name=name
        self.nom=str(self.field+self.name)
        # Mesh(if true il la visualise)
        self.plotmesh=plotmesh
        # Mesh
        self.mesh=mesh
        # Temps
        self.time=str(0)
        # Dimension
        self.dim=dim
        # Rotation
        self.rotx=rotx
        self.roty=roty
        self.rotz=rotz
        # Coordonée 
        self.xIndice=0
        self.yIndice=0
        # dimension 2D 
        self.nX=nX
        self.nY=nY 
        # dimension 1D 
        self.axes=nY 
        #   
        self.flag=False
        # Temps 
        self.time=time
        #
        self.key=-1
        # Photo vide(if true)
        self.empty=empty
        # size image
        self.size=size
        # max/min des valeurs du plot. 
        self.max    = max
        self.min    = min
        
        saveFile(fichier,field,name,active)
        
        self._reset() 

    def _reset(self):
        """
        Reset the class

        Parameters
        ---------
        

        Returns
        -------
        
        """
        # Assure qu'il ne y a pas deja une image,  si oui, il l'efface
        
        self.fig,self.axs=plt.subplots(self.nY,self.nX,figsize=(self.size,self.size))
        if not self.empty: self.addPlot(self.coordonee()) 
        #self.fig.suptitle(self.suptitle)
        
    def addPlot(self,coordonee):   
        """ 
        
        Methode pour afficher un graphiques.
        

        Parameters
        ---------
        coordonee : array int
            Coordonée du plot
        

        Returns
        -------
        
        """  
        self.key=self.key+1
        path=os.getcwd()
        # Assure qu'il ne y a pas deja un fichier, si oui, il l'efface
        path=""#os.getcwd()
        file = path+"tmp.py"
        if os.path.exists(file):
            os.remove(file)
        with open("tmp.py", "a") as f:
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
            f.write("VectorAtts.colorTableName = \"Default\"\n")
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
            
            #
            f.write("dbs = ('"+self.fichier+"') \n")
            f.write("ActivateDatabase(dbs) \n")
            f.write("AddPlot('"+self.field+"','"+ self.name+"')\n")
            f.write("DrawPlots() \n")
            if self.plotmesh:
                f.write("AddPlot('Mesh', '"+self.mesh+"')\n")
                f.write("DrawPlots() \n")
            # Boucle if pour roter le plot 3d de 30 degre ,selon l'axe x et y (2 rotations).
            if self.dim==3: 
                f.write("va = GetView3D()\n")
                f.write("va.RotateAxis(1,0)\n")
                f.write("va = GetView3D()\n")
                f.write("va.RotateAxis(0,"+str(self.rotx)+")\n")
                f.write("SetView3D(va)\n")
                f.write("va.RotateAxis(1,"+str(self.roty)+")\n")
                f.write("SetView3D(va)\n")
                f.write("va.RotateAxis(2,"+str(self.rotz)+")\n")
                f.write("SetView3D(va)\n")
            if not self.min==None:
                f.write("p=PseudocolorAttributes()\n")  
                f.write("p.minFlag=1\n")
                f.write("p.min="+str(self.min)+"\n") 
                f.write("SetPlotOptions(p)\n")   
            if not self.max==None:
                f.write("p=PseudocolorAttributes()\n")    
                f.write("p.maxFlag=1\n") 
                f.write("p.max="+str(self.max)+"\n")
                f.write("SetPlotOptions(p)\n")   
        f.close()   
        
        if(self.nX==1)&(self.nY==1):
            #raise ValueError("Use plot and not plot2!!!")
            self.flag=True 
        elif (self.nX==1)|(self.nY==1): self.xIndice=coordonee
        else: 
            self.xIndice=coordonee[0]
            self.yIndice=coordonee[1]
            
        if self.flag : self.subplot=self.axs 
        else: self.subplot=self.axs[self.coordonee()] 
        
        self.subplot.grid()  
        
        if (self.key!=0)|(self.nX*self.nY!=1):
            self.setFrame(self.time)
            self.save()        
            self.insert()
        
    def _3d(self):   
        """ 
        
        Method for 3D plot.
        Parameters
        ---------         

        Returns
        -------
        
        """      
        
        with open("tmp.py", "a") as f: 
            f.write("va = GetView3D()\n")
            f.write("va.RotateAxis(1,0)\n")
            f.write("va = GetView3D()\n")
            f.write("va.RotateAxis(0,"+str(self.rotx)+")\n")
            f.write("SetView3D(va)\n")
            f.write("va.RotateAxis(1,"+str(self.roty)+")\n")
            f.write("SetView3D(va)\n")
            f.write("va.RotateAxis(2,"+str(self.rotz)+")\n")
            f.write("SetView3D(va)\n")
        f.close()    
        
    def addField(self,fichier=None,field=None,nom=None,mesh=None):   
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
        dim : int
            Dimension of the plot.        

        Returns
        -------
        
        """     
        if fichier is None:
            fichier=self.fichier
        if (field is None)|(nom is None):
            raise ValueError("Error: need field or/and name!!") 
        
        with open("tmp.py", "a") as f: 
            if not fichier==None:
                fichier="build/" +fichier
                f.write("dbs = ('"+fichier+"') \n")
                f.write("ActivateDatabase(dbs) \n")
            if not mesh==None:
                f.write("AddPlot('Mesh', '"+mesh+"')\n") 
            f.write("AddPlot('"+field+"','"+ nom+"')\n")
            f.write("DrawPlots() \n") 
        f.close()    
        
    def visitCommand(self,string):   
        """ 
        
        Method for adding command lines to the .py file.
        Parameters
        ---------
        string : str
            Comand for Visit.

        Returns
        -------
        
        """      
        with open("tmp.py", "a") as f: 
            f.write(string+"\n") 
        f.close()    
        
    def save(self,win=False): 
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
        with open("tmp.py", "a") as f:
            f.write("s = SaveWindowAttributes() \n")
            f.write("s.fileName = '"+self.fichier+self.name+str(self.time)+"_' \n")
            f.write("s.format = s.PNG \n")
            f.write("s.progressive = 1 \n")
            f.write("s.fileName = '"+self.fichier+self.name+str(self.time)+"_' \n") 
            f.write("SetSaveWindowAttributes(s) \n")
            f.write("SaveWindow()")
        f.close()
        if win : 
            process = subprocess.Popen("visit -cli -s  tmp.py", shell=True) # Faire sa independante de cette methode 
            process.wait() 
        else   :
            os.system("visit -nowin -cli -s  tmp.py") # Faire sa independante de cette methode
            
    def coordonee(self):
        """

        Return plot coordenates.
        
        Parameters
        --------- 
        
        Returns
        -------
        coordonne : array int
            Coordonné.
        
        """ 
        if(self.nX==1)&(self.nY==1):
            #raise ValueError("Use plot and not plot2!!!")
            return(0) 
        elif((self.nX==1)|(self.nY==1)): 
            return(max(self.xIndice,self.yIndice))
        else: 
            return(self.xIndice,self.yIndice)
        
    def insert(self):
        """

        Add a photo to the Multiple plot.
        
        Parameters
        ---------  

        Returns
        -------
        
        
        """ 
        img = mpimg.imread(""+self.fichier+self.name+str(self.time)+"_0000.png") 
        
        self.subplot.imshow(img)
        self.subplot.axis('off') 
        
    def add(self,fichier,field,name,xIndice=0,yIndice=0,time=-1,dim=2,rotx=0,roty=0,rotz=0,mesh="dom",max=None,min=None):
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
        dim : int
            Dimension of the plot.  
        rotx : int
            Rotation along the x axis.  
        roty : int
            Rotation along the y axis.  
        rotz : int
            Rotation along the z axis.   
        time : int
            Time of the plot. 
        empty : bool
            If True returns a empty plot.  
        size : int
            Size of the image.  
        max : float
            Maximun value ploted.  
        min : float
            Minimum value ploted.  

        Returns
        -------
        
        """ 
        
        self.xIndice = xIndice
        self.yIndice = yIndice 
        self.fichier = "build/" + fichier 
        self.field   = field
        self.name    = name
        self.time    = time
        self.dim     = dim
        self.rotx    = rotx
        self.roty    = roty
        self.rotz    = rotz
        self.mesh    = mesh
        self.max    = max
        self.min    = min
        
        
        self.addPlot(self.coordonee())
        
        
    def plot(self): 
        """

        Visualize the graph.
        
        Parameters
        ---------  

        Returns
        -------
        
        """ 
        if (self.key==0): 
            self.setFrame(self.time)
            self.save()        
            self.insert()
        if(self.empty==True):  
            self.save()        
            self.insert()
        plt.show()        
        
    def addMesh(self,mesh):
        """

        Add another mesh to the Visit Plot. 

        Parameters
        ---------
        mesh: str
            name of the mesh

        Returns
        -------
        
        """
        with open("tmp.py", "a") as f:
            f.write("AddPlot('Mesh', '"+mesh+"')\n")
            f.write("DrawPlots() \n")
        f.close()


    def pas(self,numero=0):
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
        self.time=str(numero)
        with open("tmp.py", "a") as f:
            f.write("SetTimeSliderState("+ str(numero) +")\n")
        f.close()

    def lineout(self,a,b):
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
        with open("tmp.py", "a") as f:
            f.write("Lineout(("+str(a[0])+","+str(a[1])+"),("+str(b[0])+","+str(b[1])+"),(\"default\")) \n")#ajouter Z
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
        self.time=str(int(self.time)+1)
        with open("tmp.py", "a") as f:
            f.write("TimeSliderNextState()\n")
        f.close()

    def point(self,x,y,z=0):
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
        self.time=str(int(self.time)+1)
        with open("tmp.py", "a") as f:
            f.write("QueryOverTime(\"Pick\")\n")
        f.close()
        
    def setFrame(self,numero=-1):
        """

        For setting frame in visit

        Parameters
        ---------
        numero: int
            If numero = -1, the last frame chosen, else it select the frame in visit

        Returns
        -------
        
        """
        self.time=str(numero)
        with open("tmp.py", "a") as f:
            if numero == -1: # last frame
                f.write("SetTimeSliderState(TimeSliderGetNStates()-1)\n")
            else:
                f.write("SetTimeSliderState("+ str(numero) +")\n")
        f.close()
         

    def meshColor(self):
        """

        Change the Color of the mesh in red.

        Parameters
        --------- 

        Returns
        -------
        
        """
        with open("tmp.py", "a") as f:
            f.write("MeshAtts = MeshAttributes() \n")
            f.write("MeshAtts.meshColorSource = MeshAtts.Foreground \n")
            f.write("MeshAtts.opaqueColorSource = MeshAtts.OpaqueCustom \n")
            f.write("MeshAtts.opaqueColor = (255, 0, 0, 255) \n")
            f.write("SetPlotOptions(MeshAtts) \n")
        f.close()

    def meshTrans(self):
        """

        Turn transperent the mesh.

        Parameters
        ---------
        field : str
            The field we want to plot.  
        name : str
            The name of the field.  


        Returns
        -------
        
        """
        with open("tmp.py", "a") as f:
            f.write("MeshAtts = MeshAttributes() \n")
            f.write("MeshAtts.opaqueColorSource = MeshAtts.Background  # Background, OpaqueCustom \n")
            f.write("MeshAtts.opacity = 0.254902 \n")
            f.write("SetPlotOptions(MeshAtts) \n")
        f.close()

    def change(self,nom,atribut):
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
        flag=0
        if nom=="field" :
            self.field=atribut
            flag=1
        if nom=="name" :
            self.name=atribut
            flag=1
        if nom=="field" :
            self.mesh=atribut
            flag=1
        if nom=="mesh" :
            self.time=atribut
            flag=1
        if nom=="fichier" :
            self.fichier=atribut
            flag=1
        if nom=="dim" :
            self.dim=atribut
            flag=1
        if nom=="rotx" :
            self.rotx=atribut
            flag=1
        if nom=="roty" :
            self.roty=atribut
            flag=1
        if nom=="rotz" :
            self.rotz=atribut
            flag=1
        if nom=="nom" :
            self.nom=atribut
            flag=1
        if flag :
            print("ERREUR: Attribut Inexistant")

class export_lata_base:  
    """
    
    Class to export profile or fields from lata files.
     
    """
    def __init__(self,fichier,field,name,saveFile,frame = -1):
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
        self.fichier=fichier
        self.field=field
        self.name=name
        self.saveFile = saveFile
        self.frame = frame
        self._reset()

    def _reset(self):
        """

        Reset the class
        
        """

        # Assure qu'il ne y a pas deja un fichier, si oui, il l'efface
        file = "tmp.py"
        if os.path.exists(file):
            os.remove(file)
        with open("tmp.py", "a") as f:
            f.write("dbs = ('"+self.fichier+"') \n")
            f.write("ActivateDatabase(dbs) \n")
        self.setFrame(self.frame)
        self.addPlot(self.field,self.name)
        f.close()

    def getFrames(self):
        """

        CMethod for getting the Frames.
        
        """
        with open("tmp.py", "a") as f:
            f.write("with open(\"frames.txt\",\"w\") as f:\n")
            f.write("   f.write(str(TimeSliderGetNStates()-1))\n")
            f.write("f.close()\n")
            f.write("sys.exit()\n")
        f.close()
        self.run()
        with open("frames.txt","r") as f:
            N = f.read()
        f.close()
        os.remove("frames.txt")
        return(int(N))    
        
    def getDimensions(self):
        r"""

        Get dimensions of mesh.
        Warning, it has to launch visit and write the result in a file.  

        """
        with open("tmp.py","a") as f:
            f.write("SetQueryOutputToString() \n")
            f.write("B = Query(\"Grid Information\") \n")
            f.write("with open(\"dim.txt\",\"w\") as f:\n")
            f.write("   f.write(B) \n")
            f.write("f.close()\n")
            f.write("sys.exit()\n")
        f.close()
        self.run()
        with open("dim.txt","r") as f:
            N = f.read()
            res = re.findall(r'[\d]*[.][\d]+|[\d]+', N)[1:] 
        f.close()
        os.remove("dim.txt")
        res = [int(r)-1 for r in res]
        return(res)

    def addPlot(self,field,name):
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
        with open("tmp.py", "a") as f:
            f.write("AddPlot('"+field+"','"+name+"')\n")
            f.write("DrawPlots() \n")
        f.close()

    def setFrame(self,numero=-1):
        """

        To set frame in visit

        Parameters
        ---------
        numero: int
            If numero = -1, the last frame chosen, else it select the frame in visit

        Returns
        -------
        
        """

        with open("tmp.py", "a") as f:
            if numero == -1: # last frame
                f.write("SetTimeSliderState(TimeSliderGetNStates()-1)\n")
            else:
                f.write("SetTimeSliderState("+ str(numero) +")\n")
        f.close()

    def lineout(self,a,b):
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
        with open("tmp.py", "a") as f:
            if dim == 2:
                f.write("Lineout(("+str(a[0])+","+str(a[1])+"),("+str(b[0])+","+str(b[1])+"),(\"default\")) \n")
            if dim == 3:
                f.write("Lineout(("+str(a[0])+","+str(a[1])+","+str(a[2])+"),("+str(b[0])+","+str(b[1])+","+str(b[2])+"),(\"default\")) \n")
            f.write("SetActiveWindow(2) \n")
        f.close()


    def point(self,x,y,z=0):
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
        with open("tmp.py", "a") as f:
            f.write("QueryOverTime(\"Pick\")\n")
        f.close()

    def query(self,start_point,end_point,num_samples = 10,mode = "Lineout"):
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
            with open("tmp.py","a") as f:
                s = "Query(\"{0}\", end_point={1}, num_samples={2}, start_point={3}, use_sampling=0, vars=( \"{4}\"))\n"
                f.write(s.format(mode, end_point, num_samples, start_point, self.name))
                f.write("SetActiveWindow(2) \n")
            f.close()



    def maximun(self,name="Max",time=-1):
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
        with open("tmp.py","a") as f:
            f.write("Query(\"Max\", use_actual_data=1) \n")  
            f.write("f=open(\""+ name +self.saveFile+"\",\"w\") \n") 
            f.write("f.write(str(GetQueryOutputValue())) \n") 
            f.write("f.close() \n") 
            f.write("print(GetQueryOutputValue())\n") 
            
            f.write("""sys.exit()""")
        self.run()
        
    def minimun(self,name="Min",time=-1):
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
        with open("tmp.py","a") as f:
            f.write("Query(\"Min\", use_actual_data=1) \n")  
            f.write("f=open(\""+ name +self.saveFile+"\",\"w\") \n") 
            f.write("f.write(str(GetQueryOutputValue())) \n") 
            f.write("f.close() \n") 
            f.write("print(GetQueryOutputValue())\n") 
            
            f.write("""sys.exit()""")
        self.run()
                            
    def save(self,type="Curve2D"):
        """

        Export and saves the .curve file
        
        """
        with open("tmp.py", "a") as f:
            f.write("dbAtts = ExportDBAttributes() \n")
            f.write("dbAtts.filename = "+"'"+self.saveFile+"'"+"\n")
            f.write("dbAtts.db_type = \""+type+"\" \n")
            f.write("dbAtts.variables = (\"default\", \"vec\") \n")
            f.write("ExportDatabase(dbAtts) \n")
            f.write("sys.exit()")
        f.close()
        self.run()
    


    def run(self):
        """

        Run the visit command in terminal.

        """
        os.system("visit -nowin -cli -s  tmp.py 1&>tmp.out") 
