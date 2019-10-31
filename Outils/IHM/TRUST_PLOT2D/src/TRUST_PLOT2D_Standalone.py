# -*- coding: latin-1 -*-
#****************************************************************************
# Copyright (c) 2015 - 2016, CEA
# All rights reserved.
#
# Redistribution and use in source and binary forms, with or without modification, are permitted provided that the following conditions are met:
# 1. Redistributions of source code must retain the above copyright notice, this list of conditions and the following disclaimer.
# 2. Redistributions in binary form must reproduce the above copyright notice, this list of conditions and the following disclaimer in the documentation and/or other materials provided with the distribution.
# 3. Neither the name of the copyright holder nor the names of its contributors may be used to endorse or promote products derived from this software without specific prior written permission.
#
# THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR A PARTICULAR PURPOSE ARE DISCLAIMED.
# IN NO EVENT SHALL THE COPYRIGHT HOLDER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL, SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE, DATA, OR PROFITS;
# OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE.
#
#*****************************************************************************

#
# Author : A. Bruneton
#
import sys, os
from pyqtside.QtGui import QApplication
from pyqtside.QtCore import SIGNAL, SLOT

from trust_plot2d.DynamicDesktop import DynamicDesktop
import trust_plot2d.SalomePyQt_MockUp as SalomePyQt_MockUp
from pyqtside.QtCore import QTimer, QTranslator, Qt

desktop = None

def activate():
    """This method mimicks SALOME's module activation """
    global desktop
    fv = desktop.showFileView()
    ctv = desktop.showCurveTreeView()
    desktop.tabifyDockWidget(fv, ctv)
    fv4= desktop.showMenuTRUST(fv)
    fv2= desktop.showFileOut(fv)
    #fv3= desktop.showFileErr()

    return True

def activeViewChanged( viewID ):
    from curveplot import PlotController
    PlotController.ActiveViewChanged(viewID)

def main(args) :
    global desktop

    # Allow Ctrl-C
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)

    app = QApplication(args)
    rootDir = os.environ.get("CURVEPLOT_ROOT_DIR", "")
    if rootDir != "":
        ts_files = [rootDir + "/share/salome/resources/curveplot/CURVEPLOT_msg_fr.qm",
                    rootDir + "/share/salome/resources/curveplot/CURVEPLOT_msg_en.qm" ]
        trans = QTranslator()
        for f in ts_files:
            if not trans.load(f):
                print("could not load translation %s!" % f)
        app.installTranslator(trans)
    dw = app.desktop()
    x, y = dw.height()*1.0, dw.height()*0.7

    desktop = DynamicDesktop(None)
    sgPyQt = SalomePyQt_MockUp.SalomePyQt(desktop)
    sgPyQt.currentTabChanged.connect(activeViewChanged)
    desktop._sgPyQt = sgPyQt
    desktop.initialize()
    desktop.resize(x,y)
    desktop.show()
    activate()

    d = desktop._dockFileView
    if len(args) > 1 and os.path.isfile(args[1]):
        d.caseWidget.setText(args[1])
        d.onTimerTrigger()
        if len(args) > 2:
            dfw = d.entryListWidget
            if d._singleCase:
                varName = args[2]
            else:
                varName = d.buildLongName(d._caseName + ".dt_ev", args[2])
            l = dfw.findItems(varName, Qt.MatchExactly)
            if len(l):
                dfw.setCurrentItem(l[0])
                d.onPlotButton(withLegend=True)

    #
    app.connect(app,SIGNAL("lastWindowClosed()"),app,SLOT("quit()"))
    app.exec_()

if __name__ == "__main__" :
    main(sys.argv)
