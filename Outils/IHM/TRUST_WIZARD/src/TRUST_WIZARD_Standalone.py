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
import sys
from pyqtside.QtGui import QApplication , QWidget
from pyqtside.QtCore import SIGNAL, SLOT, QTimer, QTranslator

from trust_wiz.TrustWizDesktop import TrustWizDesktop
import trust_wiz.SalomePyQt_MockUp as SalomePyQt_MockUp

desktop = None

def activate():
    """This method mimicks SALOME's module activation """
    global desktop

    desktop.showCentralWidget()
    return True

def loadJSon(filePath):
  import json
  with open(filePath) as f:
    data = json.loads(f.read())
  return data
  
def main(args) :
    global desktop
    
    # Allow Ctrl-C
    import signal
    signal.signal(signal.SIGINT, signal.SIG_DFL)
    
    app = QApplication(args)
    
    dw = app.desktop()
    x, y = dw.width()*0.25, dw.height()*0.7
    
    testMode = 0 #1
    desktop = TrustWizDesktop(None, testMode=testMode)
    sgPyQt = SalomePyQt_MockUp.SalomePyQt(desktop)
    desktop._sgPyQt = sgPyQt
    
    if len(args) == 1:
      # GUI interactive mode 
      desktop.initialize()
      desktop.resize(x,y)    
      activate()
      
#      if not testMode:
#        import jig.widgets as widgets
#        dw = desktop._dynWidget
#        w = dw.resolvePath(["/", 2, 'thermo_hydrau_pb', 'domain', 'med_file'])
#        w.setText("/home/ab205030/Maillage_test.med")
#        rootW = dw.resolvePath(["/"])
#        rootW._combo.setCurrentIndex(2)
#        w = dw.resolvePath(["/", 2,  'thermo_hydrau_pb', 'turb_model'])
#        w.setIndex(1)
#        for w in dw.findChildren("velocity") + dw.findChildren("temperature") + dw.findChildren("gravity"):
#          if isinstance(w, widgets.JigArrayFlat):
#            for ci in range(len(w._leaves)):
#              w.getChild(ci).setText(0.0)
#          if isinstance(w, widgets.JigLineEditNumber):
#            w.setText("0.0") 
#        pass
      desktop.show()    
          
      app.connect(app,SIGNAL("lastWindowClosed()"),app,SLOT("quit()"))
      app.exec_()
    else:
      # Simple command line validation
      data = loadJSon(args[1])
      desktop.createWidgetAndValidator()
      ret, pth = desktop._validator.validateInput(data=data, highlight=False)
      if ret != "":
        print ret
        print "Error path", pth
        sys.exit(-1)

if __name__ == "__main__" :
    main(sys.argv)
