"""
Victor Banon Garcia
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

We provide here a python package that can be used to create interactive plots in jupyterlab.

"""
from ..visitutils import tools_for_visit as visit
from ipywidgets import interact, interactive, fixed, interact_manual
import ipywidgets as widgets

def plotFunction(fichier,field,name,rotx,roty,rotz,time,plotmesh,cm,addfield,fichier_add, field_add, name_add,visitcomand):
    """
    Function used to plot like VisIt.

    Parameters
    ---------
    fichier : str
        The .lata file we want to plot its mesh with visit.  
    field : str
        The field we want to plot.  
    fichier : str
        The fichier of the field.   
    rotx : int
        Rotation along the x axis.  
    roty : int
        Rotation along the y axis.  
    rotz : int
        Rotation along the z axis.  
    time : str 
        time of the plot.  
    plotmesh : str 
        plot the mesh.   
    cm : str 
        color of the mesh.   
    addfield : str 
        if true plot of the addfield.   
    fichier_add : str 
        The .lata file we want to plot its mesh with visit of the addfield.   
    field_add : str
        The field we want to plot of the addfield.   
    fichier_add : str
        The fichier of the field of the addfield.      
    visitcomand : str 
        visitcomand.   

    Returns
    -------
        
    None
        
    """
    plot=visit.Show(fichier, field, name, plotmesh=plotmesh,time=time)
    print(f'Command: plot=visit.Show(fichier="{fichier}", field="{field}", name="{name}", plotmesh={plotmesh},time={time})')
    if cm:
        plot.meshColor()
        print(f'Command: plot.meshColor()')
    if addfield:
        plot.addField(fichier_add, field_add, name_add)
        print(f'Command: plot.addField("{fichier_add}", "{field_add}", "{name_add}")')
    if visitcomand!="":
        plot.visitCommand(visitcomand)
        print(f'Command: plot.visitCommand({visitcomand})')
    if rotx and roty and rotz:
        plot.rotation3D([rotx,roty,rotz])
        print(f'Command: plot.rotation3D([{rotx},{roty},{rotz}])')
    print(f'Command: plot.plot()')
    plot.plot()

def interface(fichier,field,name,rotx=45,roty=45,rotz=45,time=-1,plotmesh=True,cm=False,addfield=False,fichier_add="", field_add="", name_add="",visitcomand=""):
    """
    Create a unpolish interface for interactive ploting.

    Parameters
    ---------
    fichier : str
        The .lata file we want to plot its mesh with visit.  
    field : str
        The field we want to plot.  
    fichier : str
        The fichier of the field.   
    rotx : int
        Rotation along the x axis.  
    roty : int
        Rotation along the y axis.  
    rotz : int
        Rotation along the z axis.  
    time : str 
        time of the plot.  
    plotmesh : str 
        plot the mesh.   
    cm : str 
        color of the mesh.   
    addfield : str 
        if true plot of the addfield.   
    fichier_add : str 
        The .lata file we want to plot its mesh with visit of the addfield.   
    field_add : str
        The field we want to plot of the addfield.   
    fichier_add : str
        The fichier of the field of the addfield.      
    visitcomand : str 
        visitcomand.   
    Returns
    -------
        
    None
        
    """
    interact(plotFunction, fichier=fichier, field=field, name=name, rotx=rotx, roty=roty, rotz=rotz, time=time, plotmesh=plotmesh,cm=cm,addfield=addfield,fichier_add=fichier_add, field_add=field_add, name_add=name_add,visitcomand=visitcomand);

class Inter(object):

    r""" 
    Class Show, which allow to use visit command in a python enviroment
    """
    def __init__(self,fichier,field,name,rotx=45,roty=45,rotz=45,time=-1,plotmesh=True,casnum=1,cm=False,addfield=False,fichier_add="", field_add="", name_add="",visitcomand=""):
        """
        Initialise the class

        Parameters
        ---------
        fichier : str
            The .lata file we want to plot its mesh with visit.  
        field : str
            The field we want to plot.  
        fichier : str
            The fichier of the field.   
        rotx : int
            Rotation along the x axis.  
        roty : int
            Rotation along the y axis.  
        rotz : int
            Rotation along the z axis.  
        time : str 
            time of the plot.  
        plotmesh : str 
            plot the mesh.   
        cm : str 
            color of the mesh.   
        addfield : str 
            if true plot of the addfield.   
        fichier_add : str 
            The .lata file we want to plot its mesh with visit of the addfield.   
        field_add : str
            The field we want to plot of the addfield.   
        fichier_add : str
            The fichier of the field of the addfield.      
        visitcomand : str 
            visitcomand.   

        Returns
        -------
        
        None
        
        """
        self.fichier=fichier
        self.field=field
        self.name=name
        self.rotx=rotx
        self.roty=roty
        self.rotz=rotz
        self.time=time
        self.plotmesh=plotmesh
        self.cm=cm
        self.addfield=addfield
        self.fichier_add=fichier_add
        self.field_add=field_add
        self.name_add=name_add
        self.visitcomand=visitcomand
        self._reset()
        self.dump()

    def _reset(self):
        """
        Reset the class(empty method).

        Parameters
        ---------
        

        Returns
        -------
        
        """
        pass

    def dump(self):
        """
        Method to interact with the widgets. 

        Parameters
        --------- 
        None

        Returns
        -------
        
        None
        
        """
        # Fichier
        #wname=widgets.FileUpload(multiple=False)
        wname=widgets.Text(value=self.fichier,disabled=False,description='adresse')
        wplotype=widgets.Text(value=self.field,disabled=False ,description='Type du plot')
        wplotname=widgets.Text(value=self.name,disabled=False ,description='Nom du plot')
        #Addfield
        waddfield         =widgets.Checkbox(value=self.plotmesh,description='Addfield')
        wname_Addfield    =widgets.Text(value=self.fichier,disabled=False,description='adresse')
        wplotype_Addfield =widgets.Text(value=self.field,disabled=False ,description='Type du plot')
        wplotname_Addfield=widgets.Text(value=self.name,disabled=False ,description='Nom du plot')
        # Visitcomand
        wVisitcomand=widgets.Text(value=self.visitcomand,disabled=False ,description='Visitcomand')
        # Rotation
        wrotx=widgets.FloatSlider(value=self.rotx,min=-360,max=360.0,description='Rotation selon x')
        wroty=widgets.FloatSlider(value=self.roty,min=-360,max=360.0,description='Rotation selon y')
        wrotz=widgets.FloatSlider(value=self.rotz,min=-360,max=360.0,description='Rotation selon z')
        # Temps
        wtime=widgets.IntSlider(value=self.time,min=-1,description='Temps')
        # Options
        wplotmesh=widgets.Checkbox(value=self.plotmesh,description='Maillage')
        wcm=widgets.Checkbox(value=self.cm,description='Maillage Rouge')


        f_file          = interactive(self.f_file,fichier=wname,field=wplotype,name=wplotname )
        f_addfield      = interactive(self.f_addfield,addfield=waddfield,fichier=wname_Addfield,field=wplotype_Addfield ,name=wplotname_Addfield )
        f_rotation      = interactive(self.f_rotation,rotx=wrotx,roty=wroty,rotz=wrotz)
        f_time          = interactive(self.f_time,time=wtime )
        f_mesh          = interactive(self.f_mesh,plotmesh=wplotmesh ,cm=wcm )
        f_visitcommmand = interactive(self.f_visitcommmand,visitcomand=wVisitcomand)

        list_widgets  = [
        widgets.VBox(children = f_file.children),
        widgets.VBox(children = f_addfield.children),
        widgets.VBox(children = f_rotation.children),
        widgets.VBox(children = f_time.children),
        widgets.VBox(children = f_mesh.children) ,
        widgets.VBox(children = f_visitcommmand.children)
        ]

        accordion = widgets.Accordion(children=list_widgets)
        accordion.set_title(0, 'Fichier')
        accordion.set_title(1, 'Addfield')
        accordion.set_title(2, 'Rotation')
        accordion.set_title(3, 'Temps')
        accordion.set_title(4, 'Options')
        accordion.set_title(5, 'Visit Command')

        tab_nest = widgets.Tab()
        tab_nest.children = [accordion]
        tab_nest.set_title(0, 'Cas1')

        display(tab_nest)

    def f_file(self,fichier,field,name):
        """
        Function to change the file.

        Parameters
        ---------
        fichier : str
            The .lata file we want to plot its mesh with visit.  
        field : str
            The field we want to plot.  
        fichier : str
            The fichier of the field.   

        Returns
        -------
        
        None
        
        """
        plotFunction(fichier=fichier,field=field,name=name,rotx=self.rotx,roty=self.roty,rotz=self.rotz,time=self.time,plotmesh=self.plotmesh,cm=self.cm,addfield=self.addfield,fichier_add=self.fichier_add, field_add=self.field_add, name_add=self.name_add,visitcomand=self.visitcomand)

        self.fichier=fichier
        self.field=field
        self.name=name

    def f_addfield(self,addfield,fichier,field,name):
        """
        Function to change the file.

        Parameters
        --------- 
        addfield : str 
            if true plot of the addfield.   
        fichier_add : str 
            The .lata file we want to plot its mesh with visit of the addfield.   
        field_add : str
            The field we want to plot of the addfield.   
        fichier_add : str
            The fichier of the field of the addfield.    

        Returns
        -------
        
        None
        
        """
        print(fichier)
        plotFunction(fichier=self.fichier,field=self.field,name=self.name,rotx=self.rotx,roty=self.roty,rotz=self.rotz,time=self.time,plotmesh=self.plotmesh,cm=self.cm,addfield=addfield,fichier_add=fichier, field_add=field, name_add=name,visitcomand=self.visitcomand)

        self.fichier_add=fichier
        self.field_add=field
        self.name_add=name
        self.addfield=addfield

    def f_rotation(self,rotx,roty,rotz):
        """
        Function to manage the rotation.

        Parameters
        ---------   
        rotx : int
            Rotation along the x axis.  
        roty : int
            Rotation along the y axis.  
        rotz : int
            Rotation along the z axis.  

        Returns
        -------
        
        None
        
        """
        plotFunction(fichier=self.fichier,field=self.field,name=self.name,rotx=rotx,roty=roty,rotz=rotz,time=self.time,plotmesh=self.plotmesh,cm=self.cm,addfield=self.addfield,fichier_add=self.fichier_add, field_add=self.field_add, name_add=self.name_add,visitcomand=self.visitcomand)
        self.rotx=rotx
        self.roty=roty
        self.rotz=rotz

    def f_time(self,time):
        """
        Function to manage the time.

        Parameters
        --------- 
        time : str 
            time of the plot.  

        Returns
        -------
        
        None
        
        """
        plotFunction(fichier=self.fichier,field=self.field,name=self.name,rotx=self.rotx,roty=self.roty,rotz=self.rotz,time=time,plotmesh=self.plotmesh,cm=self.cm,addfield=self.addfield,fichier_add=self.fichier_add, field_add=self.field_add, name_add=self.name_add,visitcomand=self.visitcomand)
        self.time=time

    def f_mesh(self,plotmesh,cm):
        """
        Function to manage the the mesh&&.

        Parameters
        --------- 
        plotmesh : str 
            plot the mesh.   
        cm : str 
            color of the mesh.   

        Returns
        -------
        
        None
        
        """
        plotFunction(fichier=self.fichier,field=self.field,name=self.name,rotx=self.rotx,roty=self.roty,rotz=self.rotz,time=self.time,plotmesh=plotmesh,cm=cm,addfield=self.addfield,fichier_add=self.fichier_add, field_add=self.field_add, name_add=self.name_add,visitcomand=self.visitcomand)
        self.plotmesh=plotmesh
        self.cm=cm

    def f_visitcommmand(self,visitcomand):
        """
        Function to manage the the mesh&&.

        Parameters
        --------- 
        visitcomand : str 
            visitcomand.    

        Returns
        -------
        
        None
        
        """
        plotFunction(fichier=self.fichier,field=self.field,name=self.name,rotx=self.rotx,roty=self.roty,rotz=self.rotz,time=self.time,plotmesh=self.plotmesh,cm=self.cm,addfield=self.addfield,fichier_add=self.fichier_add, field_add=self.field_add, name_add=self.name_add,visitcomand=visitcomand)
        self.visitcomand=visitcomand

