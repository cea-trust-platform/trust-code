import os

from pyqtside.QtCore import Qt, QTimer, QDir,QObject,  QFileSystemWatcher, QFile, QIODevice, Slot, SIGNAL
from pyqtside.QtGui import QMainWindow,QMenu, QFileDialog, QDockWidget
from pyqtside.uic import loadUiGen
from utils import completeResPath

from FileManager import FileManager
from TrustFileWatcher import TrustFileWatcher
import curveplot
   
class TailFileWidget(QDockWidget):
  def __init__(self,fv):
    QDockWidget.__init__( self)
    loadUiGen(completeResPath("TailFileWidget.ui"), self)
#    self._timer = QTimer()
#    self.connect(self._timer, SIGNAL("timeout()"), self.onTimerTrigger)
#    self._timer.start(self.refreshWidget.value()*1000)
#    self._fileManager = FileManager()
    self._longcaseName = "" # irrelevant if _singleCase=False (i.e
#   self._currDir = "" 
    #
    # Output file 
    self.__stdoutTrustFileWatcher = TrustFileWatcher(self, self.outTextEdit, fv, ".out")
    self.__stderrTrustFileWatcher = TrustFileWatcher(self, self.errTextEdit, fv, ".err")
    self._showFileView = fv
    # connections
    self._showFileView.caseChanged.connect(self.onCaseChanged)
    self._showFileView.runStarted.connect(self.__stdoutTrustFileWatcher.restart)
    self._showFileView.runStopped.connect(self.__stdoutTrustFileWatcher.stop)
    self._showFileView.runTerminated.connect(self.__stdoutTrustFileWatcher.terminate)
    self._showFileView.runStarted.connect(self.__stderrTrustFileWatcher.restart)
    self._showFileView.runStopped.connect(self.__stderrTrustFileWatcher.stop)
    self._showFileView.runTerminated.connect(self.__stderrTrustFileWatcher.terminate)

  def _getLongCaseName(self):
    cutn=self._showFileView._caseName.split(".data")[0]
    return os.path.join(self._showFileView._currDir,cutn)
      
  @Slot()
  def onRunButton(self):
    """ Start the run
    """
    print "*fg* TailFileWidget.onRunButton" 
    if self._longcaseName == "":
      return
    run_TRUST(self._longcaseName)
  @Slot()
  def onStopButton(self):
    """ Stupidly write 1 to the stop file :-)
    """
    import os
    if self._currDir == "":
      return
    stopFile = os.path.join(self._currDir, self._caseName + ".stop")
    try: 
      with open(stopFile, "w") as f:
        f.write("1\n")
    except IOError:
      print "Could not write into stop file ..."  
      
  @Slot()
  def onCaseChanged(self):
    print "*fg* TailFileWidget onCaseChanged"
    self.tryLoadFile(self.outTextEdit, ".out")
    self.tryLoadFile(self.errTextEdit, ".err")
  
  def tryLoadFile(self, textEdit, extension):
    aCaseOutputFile = self._getLongCaseName()+extension
    textEdit.clear()
    if ( os.path.exists(aCaseOutputFile) and os.path.isfile(aCaseOutputFile) ):
        fd = open(aCaseOutputFile, 'r')
	content=fd.readlines()
        # on bloque a 10 000 lignes 
        content2=' '.join(content[-10000:])
        textEdit.append(content2)
        fd.close()
        return
        content = fd.read(1024)
        while ( content ):
            textEdit.append(content)
            content = fd.read(1024)
        fd.close()
    return

   

