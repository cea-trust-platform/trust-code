myDebug = 1

import os
from xqt import *
try:
    from xcontext import getContext
    context = getContext()
    if context == "clt":

        # --- CORBA.ORB instance ---
        from omniORB import CORBA
        myORB = CORBA.ORB_init( [''], CORBA.ORB_ID )

        # --- Naming Service ---
        from SALOME_NamingServicePy import *
        myNamingService = SALOME_NamingServicePy_i( myORB )

        # --- Life Cycle CORBA ---
        import LifeCycleCORBA
        myLCC = LifeCycleCORBA.LifeCycleCORBA(myORB)

        # --- Study manager ---
        import SALOMEDS
        myStudyManager = myNamingService.Resolve( '/myStudyManager' )

        # --- TRIOU engine ---
        import TRIOU_CORBA
        myEngine = myLCC.FindOrLoadComponent( "FactoryServerPy", "TRIOU" )
    else:
        print("On est HORS salome")
        pass
    pass
except:
    raise Exception("pb init")

# -----------------------------
# Message boxes
# -----------------------------
def QInformation(parent, title, message, btn1 = QString.null, btn2 = QString.null, btn3 = QString.null):
    """ Information message box """
    return QMessageBox.information(parent, title, message, btn1, btn2, btn3)

def QWarning(parent, title, message, btn1 = QString.null, btn2 = QString.null, btn3 = QString.null):
    """ Warning message box """
    return QMessageBox.warning(parent, title, message, btn1, btn2, btn3)

def QError(parent, title, message, btn1 = QString.null, btn2 = QString.null, btn3 = QString.null):
    """ Error message box """
    return QMessageBox.critical(parent, title, message, btn1, btn2, btn3)

# -----------------------------
# Get SALOME Swig object
# -----------------------------
def getSalomeSwig():
    import libSALOME_Swig
    aSalomeSwig = libSALOME_Swig.SALOMEGUI_Swig()
    return aSalomeSwig

# -----------------------------
# Get SALOME PyQt object
# -----------------------------
def getSalomePyQt():
    import SalomePyQt
    aSalomePyQt = SalomePyQt.SalomePyQt()
    return aSalomePyQt

# -----------------------------
# Get active study ID
# -----------------------------
def getStudyId():
    aStudyId = getSalomePyQt().getStudyId()
    return aStudyId

# -----------------------------
# Get active study
# -----------------------------
def getStudy():
    aStudy = myStudyManager.GetStudyByID( getStudyId() )
    return aStudy

# -----------------------------
# Get TRIOU Extension
# -----------------------------
def getExtension():
    anExtension = myEngine.newGetExtensionObject().GetExtension()
    return anExtension

# -----------------------------
# SetSetting(): activate module
# -----------------------------
def setSettings():
    if str( getSalomePyQt().getSetting( "TRIOU:SolverFile" ) ) == "":
        aFile = "TRUST_mpich_opt_st"
        if "TRIOU_ROOT_DIR" in os.environ:
            aFile = os.getenv( "TRIOU_ROOT_DIR" ) + "/bin/" + aFile
        getSalomePyQt().addStringSetting( "TRIOU:SolverFile", aFile, 1 )
    pass

# -----------------------------
# action: set solver executable
# -----------------------------
def SetSolverLocation():
    aFile = getSalomePyQt().getSetting( "TRIOU:SolverFile" )
    if aFile.isEmpty() and "TRIOU_ROOT_DIR" in os.environ:
        aFile = os.getenv( "TRIOU_ROOT_DIR" ) + "/bin/TRUST_mpich_opt_st"
        pass
    aFile = getSalomePyQt().getFileName( getSalomePyQt().getDesktop(), aFile, QStringList(), "Select solver executable", 1 )
    if not aFile.isEmpty():
        getSalomePyQt().addStringSetting( "TRIOU:SolverFile", aFile, 1 )
        pass
    pass

# -----------------------------
# action: start solver
# -----------------------------
def StartSolver():
    import TRIOUGUI_SolverDlg
    studyId = getStudyId()
    d = TRIOUGUI_SolverDlg.SolverDlg( getExtension(), getSalomePyQt().getDesktop() )
    d.show()
    pass

# -----------------------------
# action: import data file
# -----------------------------
def ImportDataFile():
    aFilters = QStringList(); aFilters.append( qApp.tr( "Data files (*.data)" ) ); aFilters.append( qApp.tr( "All files (*.*)" ) )
    #aFile = getSalomePyQt().getFileName( getSalomePyQt().getDesktop(), "", aFilters, "Import data file", 1 )
    print("ici")
    from xmainwindow import getMainWindow
    titi=getMainWindow()
    aFile=QFileDialog.getOpenFileName(QString.null,
                                      "(*.data)",
                                      titi,"Import data file","Import data file")
    if not aFile.isEmpty():

        from xcontext import getContext
        context = getContext()
        if context == "clt2":
            ret = myEngine.newExportImportObject().ImportDataFile( getStudy(), str( aFile ) )
            if myDebug: print("TRIOUGUI_Extension:ImportDataFile: ret =", ret)
            getSalomeSwig().updateObjBrowser( 0 )
            pass
        else:
            theWorkingDir = '/tmp'
            theWorkingDir = os.tempnam( theWorkingDir, "TRIOU" )
            os.makedirs( theWorkingDir )
            os.system("ln -sf "+str(aFile)+' '+theWorkingDir)
            import sys
            syspath=sys.path
            print(theWorkingDir)
            s=open(theWorkingDir+"/trad","w")
            syspath2=":".join(syspath)
            s.write("#!/bin/bash\nexport PYTHONPATH="+syspath2+'\n')
            s.write('file1=`basename '+str(aFile)+'`\n')
            s.write('cd '+theWorkingDir+'\n')
            s.write('python -c \"from triou import *;list=read_file_data(\'$file1\');write_file_python(\'jdd.py\',list)\"\n')
            s.close()
            os.system("sh "+theWorkingDir+"/trad")
            print("ici")
            if context == "clt":
                from xdatagui import importPy
                importPy(theWorkingDir+"/jdd.py","salome")
            else:
                getMainWindow().importpy(theWorkingDir+"/jdd.py")
                pass
            print("la")
            #from triou import read_file_data
            #listclass=read_file_data(str(aFile))
            #print listclass
            pass
        pass
    pass

# -----------------------------
# action: export data file
# -----------------------------
def ExportDataFile():
    from xcontext import getContext
    context = getContext()
    if context == "clt":
        aFilters = QStringList(); aFilters.append( qApp.tr( "Data files (*.data)" ) ); aFilters.append( qApp.tr( "All files (*.*)" ) )
        aFile = getSalomePyQt().getFileName( getSalomePyQt().getDesktop(), "", aFilters, "Export data file", 0 )
        if not aFile.isEmpty():
            ret = myEngine.newExportImportObject().ExportDataFile( getStudy(), str( aFile ) )
            if myDebug: print("TRIOUGUI_Extension:ExportDataFile: ret =", ret)
            pass
        pass
    else:
        from xmainwindow import getMainWindow
        titi=getMainWindow()
        aFile=QFileDialog.getSaveFileName(QString.null,
                                          "(*.data)",
                                          titi,"Export data file","Export data file")
        if not aFile.isEmpty():
            theWorkingDir = '/tmp'
            theWorkingDir = os.tempnam( theWorkingDir, "TRIOU" )
            os.makedirs( theWorkingDir )
            #os.system("ln -sf "+str(aFile)+' '+theWorkingDir)
            # sauvegarde en python
            from xtree import getMainXTree
            axtree = getMainXTree()
            axtree.saveAs(theWorkingDir+'/prov.py')
            #
            import sys
            syspath=sys.path
            print(theWorkingDir)
            s=open(theWorkingDir+"/trad","w")
            syspath2=":".join(syspath)
            s.write("#!/bin/bash\nexport PYTHONPATH="+syspath2+'\n')
            s.write('file1=`basename '+str(aFile)+'`\n')
            s.write('cd '+theWorkingDir+'\n')
            s.write('python -c \"from triou import *;from prov import *;write_file_data(\''+str(aFile)+'\',list_instance)\"\n')
            s.close()
            os.system("sh "+theWorkingDir+"/trad")
            pass
        pass
    pass

# --- action map ---
id2extaction = {}
id2extaction[51]  = SetSolverLocation
id2extaction[6001] = StartSolver
id2extaction[91]  = ImportDataFile
id2extaction[92]  = ExportDataFile

# -----------------------------
# OnGUIEvent(): process event
# -----------------------------
def OnGUIEvent( command_id ):
    if myDebug: print("TRIOUGUI_Extension::OnGUIEvent:", command_id)
    if command_id in id2extaction:
        id2extaction[ command_id ]()
    else:
        from xdatasalome import OnGUIEvent
        OnGUIEvent( command_id )
        pass
    from xcontext import getContext
    context = getContext()
    if context == "clt":
        getSalomeSwig().updateObjBrowser( 0 )
    else:
        from xmainwindow import getMainWindow
        getMainWindow().updateObjectBrowser()
        pass
    return
