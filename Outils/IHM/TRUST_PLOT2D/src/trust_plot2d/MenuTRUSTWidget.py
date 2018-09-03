from pyqtside.QtCore import Qt, QTimer, QDir, Slot, SIGNAL
from pyqtside.QtGui import QMainWindow,QMenu, QFileDialog, QDockWidget
from pyqtside.uic import loadUiGen
from utils import completeResPath

from FileManager import FileManager
import os
import curveplot
   
class MenuTRUSTWidget(QDockWidget):
  def __init__(self,fv):
    QDockWidget.__init__( self)
    loadUiGen(completeResPath("MenuTRUSTWidget.ui"), self)
    #    self._timer = QTimer()
    #    self.connect(self._timer, SIGNAL("timeout()"), self.onTimerTrigger)
    #    self._timer.start(self.refreshWidget.value()*1000)
    #    self._fileManager = FileManager()
    self._showFileView = fv

  def _getLongCaseName(self):
    cutn=self._showFileView._caseName.split(".data")[0]
    return os.path.join(self._showFileView._currDir,cutn)
  def execute(self,cmd):
    import subprocess
    print "Starting ",cmd
    cmd=cmd.split()
    subprocess.Popen(cmd)
    
  @Slot()
  def onTerminalButton(self):
    """ Start the run
    """
    case= self._getLongCaseName()
    if case == "":
      return
    exterm=os.environ.get("Xterm")
    if exterm == "":
      exterm="xterm"
    self.execute(exterm)

  def EditFile(self,extension):
    case= self._getLongCaseName()
    if case == "":
      return
    case+=extension
    # cmd="xemacs "+case
    editor=os.environ.get("TRUST_EDITOR")
    if editor == "":
      editor="xemacs"
    cmd=editor+" "+case
    self.execute(cmd)

  @Slot()
  def onEditOutButton(self):
    self.EditFile(".out")
    
  @Slot()
  def onEditErrButton(self):
    self.EditFile(".err")
  @Slot()
  def onEditButton(self):
    """ Start the run
    """
    self.EditFile(".data")
  @Slot()
  def onEditXDATAButton(self):
    """ Start the run
    """
    case= self._getLongCaseName()
    if case == "":
      return
    case+=".data"
    cmd="EditData -batch "+case
    self.execute(cmd)
    
  @Slot()
  def onVisuButton(self):
    """ Start the run
    """
    case= self._getLongCaseName()
    if case == "":
      return
    case+=".lata"
    cmd="visit -o  "+case
    #cmd="paraview  "+case
    self.execute(cmd)
