"""
Victor Banon Garcia
CEA Saclay - DM2S/STMF/LGLS
Mars 2021 - Stage 6 mois

We provide here a python package that can be used to create interactive plots in jupyterlab.

"""
from trustutils.visitutils import tools_for_visit as visit
from ipywidgets import interact, interactive, fixed, interact_manual
import ipywidgets as widgets

def plotFunction(name,plotype,plotname,rotx,roty,rotz,tim,plotmesh,cm,addfield,name_add, plotype_add, plotname_add,visitcomand):
    """
    Function used to plot like VisIt.

    Parameters
    ---------
    fichier : str
        The .lata file we want to plot its mesh with visit.  
    field : str
        The field we want to plot.  
    name : str
        The name of the field.   
    rotx : int
        Rotation along the x axis.  
    roty : int
        Rotation along the y axis.  
    rotz : int
        Rotation along the z axis.  
    tim : str 
        time of the plot.  
    plotmesh : str 
        plot the mesh.   
    cm : str 
        color of the mesh.   
    addfield : str 
        if true plot of the addfield.   
    name_add : str 
        The .lata file we want to plot its mesh with visit of the addfield.   
    field_add : str
        The field we want to plot of the addfield.   
    name_add : str
        The name of the field of the addfield.      
    visitcomand : str 
        visitcomand.   

    Returns
    -------
        
    None
        
    """
    dim=3
    plot=visit.Show(name, plotype, plotname, dim=dim,rotx=rotx,roty=roty,rotz=rotz,plotmesh=plotmesh,time=tim)   
    print(f'Command: plot=visit.Show(name="{name}", plotype="{plotype}", plotname="{plotname}", dim={dim},rotx={rotx},roty={roty},rotz={rotz},plotmesh={plotmesh},tim={tim})')
    if cm: 
        plot.meshColor()
        print(f'Command: plot.meshColor()')
    if addfield: 
        plot.addField(name_add, plotype_add, plotname_add)
        print(f'Command: plot.addField("{name_add}", "{plotype_add}", "{plotname_add}")')
    if visitcomand!="":
        plot.visitCommand(visitcomand)
        print(f'Command: plot.visitCommand({visitcomand})')
    print(f'Command: plot.plot()')
    plot.plot()
    
def interface(name,plotype,plotname,rotx=45,roty=45,rotz=45,tim=-1,plotmesh=True,cm=False,addfield=False,name_add="", plotype_add="", plotname_add="",visitcomand=""):
    """
    Create a unpolish interface for interactive ploting.

    Parameters
    ---------
    fichier : str
        The .lata file we want to plot its mesh with visit.  
    field : str
        The field we want to plot.  
    name : str
        The name of the field.   
    rotx : int
        Rotation along the x axis.  
    roty : int
        Rotation along the y axis.  
    rotz : int
        Rotation along the z axis.  
    tim : str 
        time of the plot.  
    plotmesh : str 
        plot the mesh.   
    cm : str 
        color of the mesh.   
    addfield : str 
        if true plot of the addfield.   
    name_add : str 
        The .lata file we want to plot its mesh with visit of the addfield.   
    field_add : str
        The field we want to plot of the addfield.   
    name_add : str
        The name of the field of the addfield.      
    visitcomand : str 
        visitcomand.   
    Returns
    -------
        
    None
        
    """
    interact(plotFunction, name=name, plotype=plotype, plotname=plotname, rotx=rotx, roty=roty, rotz=rotz, tim=tim, plotmesh=plotmesh,cm=cm,addfield=addfield,name_add=name_add, plotype_add=plotype_add, plotname_add=plotname_add,visitcomand=visitcomand);
        
class Inter:
        
    r""" 
    Class Show, which allow to use visit command in a python enviroment
    """
    def __init__(self,name,plotype,plotname,rotx=45,roty=45,rotz=45,tim=-1,plotmesh=True,casnum=1,cm=False,addfield=False,name_add="", plotype_add="", plotname_add="",visitcomand=""):
        """
        Initialise the class

        Parameters
        ---------
        fichier : str
            The .lata file we want to plot its mesh with visit.  
        field : str
            The field we want to plot.  
        name : str
            The name of the field.   
        rotx : int
            Rotation along the x axis.  
        roty : int
            Rotation along the y axis.  
        rotz : int
            Rotation along the z axis.  
        tim : str 
            time of the plot.  
        plotmesh : str 
            plot the mesh.   
        cm : str 
            color of the mesh.   
        addfield : str 
            if true plot of the addfield.   
        name_add : str 
            The .lata file we want to plot its mesh with visit of the addfield.   
        field_add : str
            The field we want to plot of the addfield.   
        name_add : str
            The name of the field of the addfield.      
        visitcomand : str 
            visitcomand.   

        Returns
        -------
        
        None
        
        """
        self.name=name
        self.plotype=plotype
        self.plotname=plotname
        self.rotx=rotx
        self.roty=roty
        self.rotz=rotz
        self.tim=tim
        self.plotmesh=plotmesh 
        self.cm=cm
        self.addfield=addfield
        self.name_add=name_add
        self.plotype_add=plotype_add
        self.plotname_add=plotname_add
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
        wname=widgets.Text(value=self.name,disabled=False,description='adresse') 
        wplotype=widgets.Text(value=self.plotype,disabled=False ,description='Type du plot') 
        wplotname=widgets.Text(value=self.plotname,disabled=False ,description='Nom du plot') 
        #Addfield
        waddfield         =widgets.Checkbox(value=self.plotmesh,description='Addfield')
        wname_Addfield    =widgets.Text(value=self.name,disabled=False,description='adresse') 
        wplotype_Addfield =widgets.Text(value=self.plotype,disabled=False ,description='Type du plot') 
        wplotname_Addfield=widgets.Text(value=self.plotname,disabled=False ,description='Nom du plot') 
        # Visitcomand
        wVisitcomand=widgets.Text(value=self.visitcomand,disabled=False ,description='Visitcomand') 
        # Rotation
        wrotx=widgets.FloatSlider(value=self.rotx,min=-360,max=360.0,description='Rotation selon x')
        wroty=widgets.FloatSlider(value=self.roty,min=-360,max=360.0,description='Rotation selon y')
        wrotz=widgets.FloatSlider(value=self.rotz,min=-360,max=360.0,description='Rotation selon z')
        # Temps
        wtim=widgets.IntSlider(value=self.tim,min=-1,description='Temps')
        # Options
        wplotmesh=widgets.Checkbox(value=self.plotmesh,description='Maillage') 
        wcm=widgets.Checkbox(value=self.cm,description='Maillage Rouge') 
    
        
        f_file          = interactive(self.f_file,name=wname,plotype=wplotype,plotname=wplotname )
        f_addfield      = interactive(self.f_addfield,addfield=waddfield,name=wname_Addfield,plotype=wplotype_Addfield ,plotname=wplotname_Addfield )
        f_rotation      = interactive(self.f_rotation,rotx=wrotx,roty=wroty,rotz=wrotz)
        f_time          = interactive(self.f_time,tim=wtim ) 
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

    def f_file(self,name,plotype,plotname):  
        """
        Function to change the file.

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
        plotFunction(name=name,plotype=plotype,plotname=plotname,rotx=self.rotx,roty=self.roty,rotz=self.rotz,tim=self.tim,plotmesh=self.plotmesh,cm=self.cm,addfield=self.addfield,name_add=self.name_add, plotype_add=self.plotype_add, plotname_add=self.plotname_add,visitcomand=self.visitcomand)
        
        self.name=name
        self.plotype=plotype
        self.plotname=plotname
        
    def f_addfield(self,addfield,name,plotype,plotname):  
        """
        Function to change the file.

        Parameters
        --------- 
        addfield : str 
            if true plot of the addfield.   
        name_add : str 
            The .lata file we want to plot its mesh with visit of the addfield.   
        field_add : str
            The field we want to plot of the addfield.   
        name_add : str
            The name of the field of the addfield.    

        Returns
        -------
        
        None
        
        """
        print(name)
        plotFunction(name=self.name,plotype=self.plotype,plotname=self.plotname,rotx=self.rotx,roty=self.roty,rotz=self.rotz,tim=self.tim,plotmesh=self.plotmesh,cm=self.cm,addfield=addfield,name_add=name, plotype_add=plotype, plotname_add=plotname,visitcomand=self.visitcomand)

        self.name_add=name
        self.plotype_add=plotype
        self.plotname_add=plotname
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
        plotFunction(name=self.name,plotype=self.plotype,plotname=self.plotname,rotx=rotx,roty=roty,rotz=rotz,tim=self.tim,plotmesh=self.plotmesh,cm=self.cm,addfield=self.addfield,name_add=self.name_add, plotype_add=self.plotype_add, plotname_add=self.plotname_add,visitcomand=self.visitcomand)
        self.rotx=rotx
        self.roty=roty
        self.rotz=rotz
    def f_time(self,tim): 
        """
        Function to manage the time.

        Parameters
        --------- 
        tim : str 
            time of the plot.  

        Returns
        -------
        
        None
        
        """ 
        plotFunction(name=self.name,plotype=self.plotype,plotname=self.plotname,rotx=self.rotx,roty=self.roty,rotz=self.rotz,tim=tim,plotmesh=self.plotmesh,cm=self.cm,addfield=self.addfield,name_add=self.name_add, plotype_add=self.plotype_add, plotname_add=self.plotname_add,visitcomand=self.visitcomand)
        self.tim=tim
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
        plotFunction(name=self.name,plotype=self.plotype,plotname=self.plotname,rotx=self.rotx,roty=self.roty,rotz=self.rotz,tim=self.tim,plotmesh=plotmesh,cm=cm,addfield=self.addfield,name_add=self.name_add, plotype_add=self.plotype_add, plotname_add=self.plotname_add,visitcomand=self.visitcomand)
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
        plotFunction(name=self.name,plotype=self.plotype,plotname=self.plotname,rotx=self.rotx,roty=self.roty,rotz=self.rotz,tim=self.tim,plotmesh=self.plotmesh,cm=self.cm,addfield=self.addfield,name_add=self.name_add, plotype_add=self.plotype_add, plotname_add=self.plotname_add,visitcomand=visitcomand)
        self.visitcomand=visitcomand
       
