# -*- coding: utf-8 -*-
#
# Author : A. Bruneton
#

from pyqtside.QtCore import Slot, Qt, QTimer, QDir
from pyqtside.QtWidgets import QMainWindow,QMenu, QFileDialog, QDockWidget
import numpy as np

from .DirScanWidget import DirScanWidget

import curveplot

class DynamicDesktop(QMainWindow):
    """
    """
    def __init__(self, sgPyQt):
        QMainWindow.__init__(self)
        self._sgPyQt = sgPyQt
        self._dockCurveBrowserView = None
        self._dockFileView = None
        self._dockFileErr = None
        self._dockFileOut = None
        self._dockMenuTRUST = None

    def initialize(self):
        """ Initialize is called later than __init__() so that the Desktop and the SgPyQt
        objects can be properly initialized.
        """
        self._currID = 1234

        self._sgDesktop = self._sgPyQt.getDesktop()
        try:
            self._plotController = curveplot.PlotController.GetInstance(self._sgPyQt)
        except:
            self._plotController = curveplot.GetInstance(self._sgPyQt)
        self.createIDs()
        self.createActions()

        context_actions = [self.itemDelAction]
        menu = QMenu(self)
        for a in context_actions:
            menu.addAction(a)
        self._plotController.setBrowserContextualMenu(menu)

        self.createToolbars()
        self.createMenus()
        self.createView()

        self.itemDelAction.triggered.connect(self.itemDel)
        self.cpsAction.triggered.connect(self.clearPlotSet)
        self.addPSAction.triggered.connect(self.addPS)

    def isRunningInSALOME(self):
        try:
            return self._sgPyQt.isRunningInSALOME()
        except AttributeError:
            return True

    def generateID(self):
        self._currID += 1
        return self._currID

    def createIDs(self):
        # Actions
        self.itemDelActionID = self.generateID()
        self.cpsActionID = self.generateID()
        self.addPSActionID = self.generateID()

        # Menus
        self.etudeMenuID = self.generateID()

    def createActions(self):
        ca = self._sgPyQt.createAction
        self.itemDelAction = ca(self.itemDelActionID, "Delete selected", "Delete selected", "", "")
        self.cpsAction = ca(self.cpsActionID, "Clear plot set", "Clear plot set", "", "")
        self.addPSAction = ca(self.addPSActionID, "Add plot set", "Add plot set", "", "")

    def createToolbars(self):
        pass
#         self.Toolbar = self._sgPyQt.createTool(self.tr("Toolbar"))
#         self._sgPyQt.createTool(self.fileNewAction, self.Toolbar)
#         self._sgPyQt.createTool(self.filePrintAction, self.Toolbar)
#         sep = self._sgPyQt.createSeparator()
#         self._sgPyQt.createTool(sep, self.Toolbar)
#         self._sgPyQt.createTool(self.editUndoAction, self.Toolbar)
#         self._sgPyQt.createTool(self.editRedoAction, self.Toolbar)

    def createMenus(self):
        curveMenu = self._sgPyQt.createMenu( "Plot management", -1, self.etudeMenuID, self._sgPyQt.defaultMenuGroup() )
        self._sgPyQt.createMenu(self.itemDelAction, curveMenu)
        self._sgPyQt.createMenu(self.cpsAction, curveMenu)
        self._sgPyQt.createMenu(self.addPSAction, curveMenu)


    def createView(self):
        pass

    def showCurveTreeView(self) :
        if self._dockCurveBrowserView is None:
            self._dockCurveBrowserView = self._plotController._curveBrowserView
            self._dockCurveBrowserView.show()
            self._sgDesktop.addDockWidget(Qt.LeftDockWidgetArea, self._dockCurveBrowserView)
        return self._dockCurveBrowserView

    def showFileOut(self,fv):
        if self._dockFileOut is None:
            from .TailFileWidget import TailFileWidget
            self._dockFileOut = TailFileWidget(fv)
            self._dockFileOut.show()
            self._sgDesktop.addDockWidget(Qt.RightDockWidgetArea, self._dockFileOut)
        return self._dockFileOut

    def showFileErr(self):
        if self._dockFileErr is None:
            self._dockFileErr = DirScanWidget()
            self._dockFileErr.show()
            self._sgDesktop.addDockWidget(Qt.RightDockWidgetArea, self._dockFileErr)
        return self._dockFileErr

    def showMenuTRUST(self,fv):
        if self._dockMenuTRUST is None:
            from .MenuTRUSTWidget import MenuTRUSTWidget
            self._dockMenuTRUST = MenuTRUSTWidget(fv)
            self._dockMenuTRUST.show()
            self._sgDesktop.addDockWidget(Qt.RightDockWidgetArea, self._dockMenuTRUST)
        return self._dockMenuTRUST

    def showFileView(self):
        if self._dockFileView is None:
            self._dockFileView = DirScanWidget()
            self._dockFileView.show()
            self._sgDesktop.addDockWidget(Qt.LeftDockWidgetArea, self._dockFileView)
        return self._dockFileView

    def hideFileView(self):
        self._dockFileView.setVisible(False)

    def hideCurveTreeView(self):
        self._dockCurveBrowserView.setVisible(False)

    @Slot()
    def itemDel(self):
        curveplot.DeleteCurrentItem()

    @Slot()
    def clearPlotSet(self):
        curveplot.ClearPlotSet()

    @Slot()
    def addPS(self):
        # Also a test for unicode!
        curveplot.AddPlotSet()
