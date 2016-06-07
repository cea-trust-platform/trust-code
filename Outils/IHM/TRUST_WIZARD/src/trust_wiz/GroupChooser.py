#
# Author : A. Bruneton
#

from pyqtside import QtGui, QtCore
from pyqtside.uic import loadUiGen
from trust_wiz.utils import completeResPath
from pyqtside.QtCore import Slot,Signal

class GroupChooser(QtGui.QWidget):
  changeSelectedGroups = Signal(list, list, name='changeSelectedGroups')
  
  def __init__(self, pb_root_path):
    QtGui.QWidget.__init__(self)
    loadUiGen(completeResPath("GroupChooser.ui"), self)
    self._pb_root_path = pb_root_path
    self._groups = []
    self._sel_groups = []

  def getSelectedGroups(self):
    self._sel_groups.sort()
    return self._sel_groups

  def onNewGroups(self, grps):
    self._groups = list(grps)
    self._sel_groups = []
    self._clearAll()
    for g in grps: 
      self.listAllGroups.addItem(g)
    self.changeSelectedGroups.emit([], self._pb_root_path)

  def _clearAll(self):
    self.listAllGroups.clear()
    self.listSelGroups.clear()

  @Slot()
  def onClearAllSelected(self):
    self.listSelGroups.clear()
    self.listAllGroups.clear()
    self.listAllGroups.addItems(self._groups)
    self._sel_groups = []
    self.changeSelectedGroups.emit([], self._pb_root_path)
  
  @Slot()  
  def onClearOne(self):
    for i in self.listSelGroups.selectedItems():
      self.listSelGroups.takeItem(self.listSelGroups.row(i))
      grp = str(i.text())
      self.listAllGroups.addItem(grp)
      self._sel_groups.remove(grp)
    self.changeSelectedGroups.emit(self._sel_groups, self._pb_root_path)
  
  @Slot()
  def onAddAll(self):
    self._clearAll()
    self.listSelGroups.addItems(self._groups)
    self._sel_groups = self._groups[:]
    self.changeSelectedGroups.emit(self._sel_groups, self._pb_root_path)
  
  @Slot()
  def onAddOne(self):
    lag = self.listAllGroups
    for i in lag.selectedItems():
      lag.takeItem(lag.row(i))
      grp = str(i.text())
      self.listSelGroups.addItem(grp)
      self._sel_groups.append(grp)
    self.changeSelectedGroups.emit(self._sel_groups, self._pb_root_path)
