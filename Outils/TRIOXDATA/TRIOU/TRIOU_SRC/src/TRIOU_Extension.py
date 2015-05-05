myDebug = 1

import os, signal, time
from threading import Thread, Lock
import os
import re

from xsalome import corba2python

import SALOME
import SALOMEDS
import SALOME__POA
import TRIOU_CORBA
import TRIOU_CORBA__POA

# ------------------------------------------
import warnings
if not myDebug:
    warnings.filterwarnings("ignore", "", RuntimeWarning)

# ------------------------------------------
__DEFAULT_DELAY__ = 0.1 # default solver delay

# --- CORBA.ORB instance ---
from omniORB import CORBA
myORB = CORBA.ORB_init([''], CORBA.ORB_ID)

# --- Life Cycle CORBA ---
import LifeCycleCORBA
myLCC = LifeCycleCORBA.LifeCycleCORBA(myORB)                                                            

# -----------------------------
# Find or load TRIOU engine
# -----------------------------
def getEngine():
    engine = myLCC.FindOrLoadComponent( "FactoryServerPy", "TRIOU" )
    return engine

# -----------------------------
# Find or load VISU engine
# -----------------------------
def getVISU():
    import VISU
    aVISU = myLCC.FindOrLoadComponent( "FactoryServer", "VISU" )
    return aVISU

# ------------------------------------------

class Extension(TRIOU_CORBA__POA.Extension):
    def __init__(self):
        """
        Constructor
        """
        self._pid = 0
        self._lock = Lock()
        self._SolverLock = Lock()
        self._paused = 0
        self._PauseLock = Lock()
        self._delay = __DEFAULT_DELAY__
        self._workDirCreated = 0
        self._workDir = ""
        self._dataFile = ""
        self._solverPath = ""
        self._study = None
        self._status = TRIOU_CORBA.OK_STATUS
        pass

    def StartSolver(self, theStudy, theInputFile, theSolverPath, theWorkingDir):
        """
        Starts TRIOU solver:
           <theStudy>      - a SALOMEDS Study reference
           <theInputFile>  - input data file (*.data), if it is empty,
                             the input data file is generated from the study contents
                             (in this case <theStudy> should not be None)
           <theSolverPath> - the path to the TRIOU solver, if it is empty, the default one is used.
           <theWorkingDir> - the working directory, if it is empty, a temporary one is created.
        Returns 1 in success and 0 in failure
        """
        
        self._status = TRIOU_CORBA.FAILED_TO_RUN
        
        # check if solver is already running
        if self._pid != 0:
            if myDebug:
                print "*******************************************************"
                print "* TRIOU_Extension.StartSolver: ERROR!"
                print "* Solver is already started!"
                print "*******************************************************"
            return 0

        # study
        self._study = theStudy

        # check solver binary path
        if not theSolverPath:
            theSolverPath = "TRUST_mpich_opt_st"
            if os.environ.has_key( "TRIOU_ROOT_DIR" ):
                aDir = os.getenv( "TRIOU_ROOT_DIR" ) + "/bin/"
                if os.path.exists( aDir ):
                    theSolverPath = aDir + theSolverPath
            if not os.path.exists( theSolverPath ) or not os.path.isfile( theSolverPath ):
                if myDebug:
                    print "*******************************************************"
                    print "* TRIOU_Extension.StartSolver: ERROR!"
                    print "* Cannot start solver: solver is not found!"
                    print "*******************************************************"
                return 0
            if myDebug:
                print "*******************************************************"
                print "* WARNING! No solver executable name is given."
                print "* Using default one:", theSolverPath
                print "*******************************************************"
        self._solverPath = theSolverPath
            
        try:
            # check/create working directory
            if theWorkingDir:
                if not os.path.exists( theWorkingDir ) or not os.path.isdir( theWorkingDir ):
                    if myDebug:
                        print "*******************************************************"
                        print "* TRIOU_Extension.StartSolver: ERROR!"
                        print "* Cannot start solver: wrong working directory!"
                        print "*******************************************************"
                    return 0
            else:
                # create temporary directory
                theWorkingDir = '/tmp'
                theWorkingDir = os.tempnam( theWorkingDir, "TRIOU" )
                os.makedirs( theWorkingDir )
                self._workDirCreated = 1
            self._workDir  = theWorkingDir
            if myDebug: print "TRIOU_Extension.StartSolver: working directory : ", self._workDir
            self._dataFile = self._workDir + os.sep + "Result.data"

            if theInputFile:
                # if <theInputFile> is given, create link to it in the working directory
                if not os.path.exists( theInputFile ):
                    if myDebug:
                        print "*******************************************************"
                        print "* TRIOU_Extension.StartSolver: ERROR!"
                        print "* Cannot start solver: wrong input file!"
                        print "*******************************************************"
                    self._ClearWorkingDir()
                    return 0
                os.symlink( theInputFile, self._dataFile )
            else:
                # if <theInputFile> is not given, try to generate input data from the study
                # first check if study is given
                if self._study is None:
                    if myDebug:
                        print "*******************************************************"
                        print "* TRIOU_Extension.StartSolver: ERROR!"
                        print "* Cannot start solver: no study is given!"
                        print "*******************************************************"
                    self._ClearWorkingDir()
                    return 0
                # then look for the TRIOU component
                aComp = self._study.FindComponent( "TRIOU" )
                if aComp is None:
                    if myDebug:
                        print "*******************************************************"
                        print "* TRIOU_Extension.StartSolver: ERROR!"
                        print "* Cannot start solver: TRIOU component is not found!"
                        print "*******************************************************"
                    self._ClearWorkingDir()
                    return 0

                # collect only first level objects
                anIter = self._study.NewChildIterator( aComp )
                listclass = []
                while anIter.More():
                    aSObj = anIter.Value()
                    anIter.Next()
                    if myDebug: print "TRIOU_Extension.StartSolver: SObject:", aSObj
                    aObject = aSObj.GetObject()
                    if myDebug: print "TRIOU_Extension.StartSolver: CORBA Object:", aObject
                    if aObject:
                        aPObj = corba2python( aObject )
                        if myDebug: print "TRIOU_Extension.StartSolver: Python Object:", aPObj
                        if aPObj:
                            listclass.append( aPObj )
                        pass
                    pass

                # export the study structure to the data file       
                from triou import write_file_data
                if myDebug: print "TRIOU_Extension.StartSolver: Writing input file: ", self._dataFile
                write_file_data( self._dataFile, listclass )

            # starting execution
            self._SolverLock.acquire()
            self._lock.acquire()

            if self._paused:
                self._PauseLock.release()

            self._paused = 0

            Thread( target=self._RunSolver, args=()).start()

            self._lock.acquire()
            self._lock.release()
        except Exception, e:
            if myDebug:
                print "*******************************************************"
                print "* TRIOU_Extension.StartSolver: ERROR!"
                print "* An exception has bee caught!"
                print "*", e
                print "*******************************************************"
            self._ClearWorkingDir()
            return 0
        return 1

    def _RunSolver(self):
        """
        Performs the solver running in the thread loop (internal usage)
        """
        # Starting the solver
        aBaseName = os.path.basename( self._dataFile ).split( '.' )[ 0 ] # remove extension
        aCmd = 'cd ' + self._workDir + '; exec ' +  self._solverPath + ' ' + aBaseName + ' >& ' + self._workDir + os.sep + 'TRIOU.log'
        self._pid = os.spawnlp( os.P_NOWAIT, '/bin/sh', 'sh', '-c', aCmd)
        if myDebug: print 'TRIOU_Extension._RunSolver: PROCESS STARTED -', self._pid
        self._lock.release()
        
        self._status = TRIOU_CORBA.RUNNING
        
        try:
            while 1:
                # Is the solver alive?
                aPid, aStatus = os.waitpid(self._pid, os.WNOHANG)
                
                if self._paused:
                    self._PauseLock.acquire()
                    self._PauseLock.release()
                    pass
                
                if myDebug: print "TRIOU_Extension._RunSolver: sleep for ", self._delay, "seconds"
                time.sleep( self._delay )
                pass
            pass
        except Exception, e:
            if myDebug: print "TRIOU_Extension._RunSolver: Exception - ", e
            pass
        
        self._status = self._PublishData()
        self._ClearWorkingDir()
            
        self._lock.acquire()
        
        # We are not interested in <pid> any longer
        self._pid = 0

        self._lock.release()
        self._SolverLock.release()
        pass

    def _ClearWorkingDir(self):
        """
        Clears/removes working directory (internal usage)
        """
        if self._workDir:
            if self._workDirCreated:
                os.system( 'rm -rf ' + self._workDir )
        pass

    def _PublishData(self):
        """
        Publishes the MED results if there are any in the study (internal usage)
        """
        if not self._workDir:
            return TRIOU_CORBA.FAILED_TO_PUBLISH
        
        # look for MED files in the working directory
        aMedFileRE = re.compile( ".+\.med$" )
        aListDir = os.listdir( self._workDir )
        aListOfFiles = []
        aFile = None
        for aFileName in aListDir:
            if re.match( aMedFileRE, aFileName ):
                aFile = self._workDir + os.sep + aFileName
                aListOfFiles.append( aFile )
                if myDebug: print "TRIOU_Extension._PublishData: found MED file ", aFile
            pass
        
        # export MED results to VISU if there are any
        status = TRIOU_CORBA.OK_STATUS
        errors = 0
        if len( aListOfFiles ) > 0:
            aVisuGen = getVISU()
            if aVisuGen is not None:
                if self._study is not None:
                    aVisuGen.SetCurrentStudy( self._study )
                    for aFile in aListOfFiles:
                        try:
                            aResult = aVisuGen.CopyAndImportFile( aFile )
                        except:
                            errors = errors + 1
                            if myDebug:
                                print "********************************************************"
                                print "* TRIOU_Extension._PublishData: ERROR!"
                                print "* Can't Import MED file", aFile
                                print "********************************************************"
                            pass
                else:
                    errors = errors + 1
            else:
                status = TRIOU_CORBA.OK_NO_VISU
                if myDebug:
                    print "********************************************************"
                    print "* TRIOU_Extension._PublishData: ERROR!"
                    print "* Can't find VISU module"
                    print "********************************************************"
                pass
        else:
            status = TRIOU_CORBA.OK_NO_MED_FILES
            if myDebug:
                print "********************************************************"
                print "* TRIOU_Extension._PublishData: WARNING!"
                print "* No result MED files is produced"
                print "********************************************************"
        if errors > 0:
            status = TRIOU_CORBA.OK_WITH_ERRORS
        return status
    
    def StopSolver(self):
        """
        Stops the solver execution
        """
        if self._pid == 0:
            return

        self._lock.acquire()
        if self._paused:
            self._PauseLock.release()
            pass

        try:
            os.kill(self._pid, signal.SIGKILL)
        except:
            pass
        self._pid = 0
        self._paused = 0
        if myDebug: print 'TRIOU_Extension.StopSolver: PROCESS KILLED!'

        self._lock.release()

    def PauseSolver(self):
        """
        Pauses the solver execution
        """
        if self._pid == 0 or self._paused == 1:
            return

        self._lock.acquire()
        self._PauseLock.acquire()

        try:
            os.kill(self._pid, signal.SIGSTOP)
            self._paused = 1
            if myDebug: print 'TRIOU_Extension.PauseSolver: PROCESS PAUSED!'
        except:
            self._PauseLock.release()

        self._lock.release()

    def ContinueSolver(self):
        """
        Resumes the solver execution
        """
        if self._pid == 0 or self._paused == 0:
            return

        self._lock.acquire()

        try:
            os.kill(self._pid, signal.SIGCONT)        
            self._paused = 0
            if myDebug: print 'TRIOU_Extension.ContinueSolver: PROCESS RESUMED!'

            self._PauseLock.release()
        except:
            pass
            
        self._lock.release()

    def IsSolverRunning(self):
        """
        Returns 1 if solver execution is in process
        """
        return (self._pid != 0)

    def IsSolverPaused(self):
        """
        Returns 1 if solver execution is paused
        """
        return self._paused

    def GetStatus(self):
        """
        Gets the status of the solver execution process
        """
        return self._status
