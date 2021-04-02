from pyqtside.QtCore import Qt, QTimer, QDir, Slot, Signal
from pyqtside.QtWidgets import QMainWindow,QMenu, QFileDialog, QDockWidget
from pyqtside.uic import loadUiGen

import numpy as np
import time

from .utils import completeResPath
from .FileManager import FileManager
from .TrustFiles import SonSEGFile

import curveplot
def run_TRUST(longcaseName):
    import os
    exe=os.getenv("exec")
    if exe is None:
        print("exec not defined")
        return
    import subprocess
    caseName=longcaseName.split(".data")[0]
    fout=caseName+".out"
    ferr=caseName+".err"
    def remove_f(fout):
        if os.path.exists( fout ):
            try:
                os.remove( fout )
            except:
                print("pb when remove "+fout)
    remove_f(fout)
    remove_f(ferr)
    with open(fout, "w") as out:
        with open(ferr, "w") as err:
            dirrun=os.path.dirname(os.path.abspath(longcaseName))
            aProcess = subprocess.Popen([exe,os.path.basename(caseName)] ,stderr=err,stdout=out,cwd=dirrun)
    return aProcess

class DirScanWidget(QDockWidget):

    caseChanged = Signal()
    runStarted  = Signal()
    runStopped  = Signal()
    runTerminated  = Signal()

    def __init__(self):
        QDockWidget.__init__( self)
        loadUiGen(completeResPath("DirScanWidget.ui"), self)
        self._timer = QTimer()
        self._timer.timeout.connect(self.onTimerTrigger)
        self._timer.start(self.refreshWidget.value()*1000)
        self._fileManager = FileManager()
        self._liveCurves = {}    # long name / psID, curve ID
        self._entryMap = {}      # long name / (fileObj, file_entry)
        self._displayMap = {}    # long name / name appearing in the list widget
        self._displayMapRev = {} # as above, reverted!
        self._prevEntries = []
        self._singleCase = True
        self._blockSig = False
        self._caseName = "" # irrelevant if _singleCase=False (i.e
        self._currDir = ""
        self._statusFile = "" # file indicating computation progress
        self._nameChangeSwitch = False
        self._entryList = []  # the full list (without filtering) for the entry list widget

        self._outfiles = False
        self._process = None
#    curveplot.RegisterCallback(self.onCurveSelected)


    @Slot()
    def onDirButton(self):
        import os
        case = QFileDialog.getOpenFileName(self, "Open case",
                                           os.curdir,
                                           "Data file (*.data)")
        if case:
            self.caseWidget.setText(case[0])
            self.onCaseChanged()
            # to update views relatives to input case change (ex: TailFileWidget)
            self.caseChanged.emit()

#  def onCurveSelected(self, crv_id):
#    if self._blockSig:
#      return
#    for txt, (_, crvId) in self._liveCurves.items():
#      if crvId == crv_id:
#        self._blockSig = True
#        self.__selectInListWidget(txt)
#        self._blockSig = False
#        return

    @Slot(str)
    def onFilterChanged(self, txt):
        self.entryListWidget.clear()
        import re
        txt = str(txt).replace("*", ".*")
        txt = txt.replace("?", ".?")
        txt = txt.replace("+", ".+")
        exp = re.compile(txt)
        for t in self._entryList:
            if exp.search(t):
                self.entryListWidget.addItem(t)

    @Slot()
    def onItemSelectionChanged(self):
        if self._blockSig:
            return
        selItems = self.entryListWidget.selectedItems()
        if len(selItems) > 1 or len(selItems) == 0:
            self.plotAllButton.setEnabled(False)
            return
        txt = str(selItems[0].text())
        longName = self._displayMapRev.get(txt, None)
        obj, _ = self._entryMap.get(longName, None)
        if obj is not None and isinstance(obj, SonSEGFile):
            self.plotAllButton.setEnabled(True)
        else:
            self.plotAllButton.setEnabled(False)
#    crvId, _ = self._liveCurves.get(longName, (None, None))
#    if crvId is not None:
#      if curveplot.GetCurrentCurveID() != crvId:
#        self._blockSig = True
#        curveplot.SetCurrentCurve(crvId)
#        self._blockSig = False
#        return
#    if curveplot.GetCurrentCurveID() != -1:
#      curveplot.SetCurrentCurve(-1)

    @Slot()
    def onPlotButton(self,withLegend=None):
        if withLegend is None:
            withLegend = not bool(len(self._liveCurves))
        # Read selected entries:
        activeEntries = self.entryListWidget.selectedItems()
        for a in activeEntries:
            self.__plotJob(False, entry=a, withLegend=withLegend)

    @Slot()
    def onPlotSameButton(self,withLegend=None):
        if withLegend is None:
            withLegend = not bool(len(self._liveCurves))
        activeEntries = self.entryListWidget.selectedItems()
        for a in activeEntries:
            self.__plotJob(True, entry=a, withLegend=withLegend)

    @Slot()
    def onPlotAllButton(self):
        activeEntry = self.entryListWidget.currentItem()
        self.__plotAllJob(activeEntry)

    @Slot()
    def onRunButton(self):
        """ Start the run
        """
        if self._currDir == "":
            return
        import os
        print("*fg* DirScanWidget.onRunButton")
        self._process = run_TRUST(os.path.join(self._currDir,self._caseName))
        self.runStarted.emit()

    @Slot()
    def onProcessFinished(self):
        print("*fg* DirScanWidget.onProcessFinished ----")
        self.runTerminated.emit()
        self._process = None

    @Slot()
    def onStopButton(self):
        """ Stupidly write 1 to the stop file :-)
        """
        print("*fg* DirScanWidget.onStopButton ---")
        import os

        if self._currDir == "":
            return
        stopFile = os.path.join(self._currDir, self._caseName + ".stop")
        try:
            with open(stopFile, "w") as f:
                f.write("1\n")
        except IOError:
            print("Could not write into stop file ...")
        # wait until the trust process has finished
        status = self._process.wait()
        self.onProcessFinished()
        print("*fg* DirScanWidget.onStopButton done")

    @Slot()
    def onCaseChanged(self):
        import os
        activeEntry = self.entryListWidget.currentItem()
        activeText = None
        if activeEntry is not None:
            activeText = str(activeEntry.text())
            oldLongName = self._displayMapRev[activeText]
        self._currDir = ""

        # Update file list for list widget
        casePath = str(self.caseWidget.text())
        # Extract case name and dir:
        self._currDir, self._caseName = "", ""
        direc, case = os.path.split(casePath)
        qdir = QDir(direc)
        if qdir.exists() and str(direc) != "" and os.path.isfile(casePath):
            self._currDir = direc
            self._caseName = case.split(".")[0]
            filters = []
            filters.append("*.son")
            filters.append("*.dt_ev")
            if self._outfiles:
                filters.append("*.out")
            qdir.setNameFilters(filters)
            files = qdir.entryList()
            entries = self.__scanFiles(direc, files)
            # Compare with prev entries (if nothing has changed nothing to do)
            equ = True
            if len(entries) != len(self._prevEntries):
                equ = False
            else:
                for i, e in enumerate(self._prevEntries):
                    if e[0] != entries[i][0]:
                        equ = False
                        break
            if equ:
                return
            self._prevEntries = entries[:]
            self.__clearEntries()
            # Update the entry label if needed
            for (longName, entryName) in entries:
                if self._singleCase:
                    s = entryName
                else:
                    s = longName
                self._entryList.append(s)
                self._displayMap[longName] = s
            self._displayMapRev = {v: k for k, v in list(self._displayMap.items())}
            if activeEntry is not None:
                self.__selectInListWidget(oldLongName)
            # to update views relatives to input case change (ex: TailFileWidget)
            self.caseChanged.emit()
        else:
            self.__clearEntries()
        self.onFilterChanged(str(self.filterEdit.text()))

    @Slot()
    def onOutStateChanged(self,flag):
        self._outfiles=not(flag==0)
        self.onCaseChanged()

    @Slot()
    def onPeriodChanged(self):
        self._timer.setInterval(self.refreshWidget.value()*1000)

    @Slot()
    def onTimerTrigger(self):
        # Refresh case/variables entries:
        self.onCaseChanged()
        # Refresh computation status
        self.__refreshStatus()
        # Refresh curves
        self.__refreshCurves()

    def __refreshCurves(self):
        curveplot.LockRepaint()
        for e, crvPsIds in list(self._liveCurves.items()):
            crvId, psID = crvPsIds
            # Delete from internal map deleted curves
            if curveplot.GetPlotSetID(crvId) == -1:
                self._liveCurves.pop(e)
                continue
            # Get new data for the curve
            if e in self._entryMap:
                fileObj, colName = self._entryMap[e]
                x, y = fileObj.getNewValues(colName)
                hasChanged = (x is None or x.shape[0])
                if x is None:
                    x, y = fileObj.getValues(colName)
                    curveplot.ResetCurve(crvId)
                if x.shape[0]:
                    curveplot.ExtendCurve(crvId, x, y)
                if isinstance(fileObj, SonSEGFile) and hasChanged:
                    curveplot.SetPlotSetTitle("Time=%f (s)" % fileObj.getLastTime(), psID)
            else:
                # If entry/file deleted, curve is kept but reset:
                curveplot.ResetCurve(crvId)
                #continue
            # Re-set label (in case switch between long/short names)
            if self._nameChangeSwitch:
                curveplot.SetCurveLabel(crvId, self._displayMap[e])
        if self._nameChangeSwitch:
            self._nameChangeSwitch = False
        curveplot.UnlockRepaint()

    def __selectInListWidget(self, longName):
        text = self._displayMap.get(longName, "")
        if text != "":
            l = self.entryListWidget.findItems(text, Qt.MatchExactly)
            if len(l):
                self.entryListWidget.setCurrentItem(l[0])

    def __clearEntries(self):
        self.entryListWidget.clear()
        self._entryList = []
        self._displayMap = {}
        self._displayMapRev = {}

    def __scanFiles(self, rootDir, fileList):
        """ Build QListWidget entries according to the file list
        """
        import os
        res = []
        cnt_dt_ev = 0 # count numb of dt_ev files to decide if we use long names or short
        # Clear internal map
        self._entryMap = {}
        # Extract case name and stop file:
        for f in fileList:
            fullName = os.path.join(str(rootDir), str(f))
            if fullName[-6:] == ".dt_ev":
                cnt_dt_ev += 1
        singleCase = (cnt_dt_ev <= 1)
        self._nameChangeSwitch = (singleCase != self._singleCase)
        self._singleCase = singleCase
        refFilePth = os.path.join(str(rootDir), self._caseName + ".stop")
        if not os.path.isfile(refFilePth):
            refFilePth = None
        for f in fileList:
            fullName = os.path.join(str(rootDir), str(f))
            fileObj, shortName = self._fileManager.getFileObject(fullName, refFilePth)
            if fileObj is not None: # file has not been deleted meanwhile:
                entrHidden = []
                if isinstance(fileObj, SonSEGFile):
                    entr = fileObj.getEntriesSeg()
                    entrHidden = fileObj.getEntries()
                else:
                    entr = fileObj.getEntries()
                for e in entr:
                    longName = self.buildLongName(shortName, e)
                    self._entryMap[longName] = (fileObj, e)
                    res.append((longName, "%s" % e))
                for e in entrHidden:
                    longName = self.buildLongName(shortName, e)
                    self._entryMap[longName] = (fileObj, e)
            else:
#         print "File %s has disappeared" % f
                pass
        return res

    def buildLongName(self, shortName, varName):
        return "[%s] - %s" % (shortName, varName)

    def __plotJob(self, append, entry=None, withLegend=False):
        if entry is not None:
            e = self._displayMapRev[str(entry.text())]
            fileObj, varName = self._entryMap[e]
            x, y = fileObj.getValues(varName)
            isLogY = fileObj.isLogY()
            xLabel = fileObj.getXLabel()
            # Create curve
            crvID, psID = curveplot.AddCurve(x, y, curve_label=str(entry.text()), x_label=xLabel , append=append)
            if isinstance(fileObj, SonSEGFile):
                curveplot.SetPlotSetTitle("Time=%f (s)" % fileObj.getLastTime(), psID)
            curveplot.SetYLog(psID, log=isLogY)
            if withLegend:
                curveplot.SetLegendVisible(psID, True)
            curveplot.SetCurveMarker(crvID, "")
            curveplot.SetCurrentCurve(crvID)
            # Register it internally
            self._liveCurves[e] = (crvID, psID)

    def __plotAllJob(self, entry):
        if entry is not None:
            topEntry = self._displayMapRev[str(entry.text())]
            fileObj, varName = self._entryMap[topEntry]
            if not isinstance(fileObj, SonSEGFile):
                raise Exception("Internal error: plotAll: not a SEG file!")
            subEntries = fileObj.getPointEntries(varName)
            curveplot.LockRepaint()
            psID = curveplot.AddPlotSet(varName)
            curveplot.SetCurrentPlotSet(psID)
            curveplot.SetYLog(psID, log=fileObj.isLogY())
            curveplot.SetXLabel("Time", psID)
            curveplot.SetLegendVisible(psID, False)
            # Invert entries:
            revert = dict((v, k) for (k,(_, v)) in list(self._entryMap.items()))
            for se in subEntries:
                x, y = fileObj.getValues(se)
                crvName = str(se)
                crvID, _ = curveplot.AddCurve(x, y, curve_label=crvName, append=True)
                curveplot.SetCurveMarker(crvID, "")
                # Register it internally, and ensure auto refresh will work:
                longName = revert[se]
                self._displayMap[longName] = crvName
                self._liveCurves[longName] = (crvID, psID)
            curveplot.UnlockRepaint()

    def __refreshStatus(self):
        """ Read status file and update progress bar """
        import os
        val = None
        try:
            if self._caseName != "":
                statusFile = os.path.join(self._currDir, self._caseName + ".progress")
                with open(statusFile, "r") as f:
                    val = f.readlines()[-1]
                if len(val):
                    val = min(max(float(val), 0.0), 100.0)
        except IOError:
            val = None

        if not val is None:
            self.progressBar.setEnabled(True)
            self.progressBar.setFormat("%p%")
            self.progressBar.setValue(val)
        else:
            self.progressBar.setEnabled(False)
            self.progressBar.setValue(0.0)
            self.progressBar.setFormat("[Not avail.]")
