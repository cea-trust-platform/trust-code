# -*- coding: utf-8 -*-
#
# Author : A. Bruneton
#

from pyqtside.QtCore import SIGNAL, SLOT, Slot, Qt, QTimer
from pyqtside.QtGui import QMainWindow,QMenu
from extra_validation import TrustExtraValidation

class TrustWizDesktop(QMainWindow):
    VIEW_TYPE = "TrustWiz"
    
    def __init__(self, sgPyQt, testMode=False):
      QMainWindow.__init__(self)
      self._sgPyQt = sgPyQt
      self._dynWidget = None
      self._mainWidget = None
      self._layout = None
      self._viewID = -1
      
      self._loader = None
      self._validator = None
      self._generator = None
      
      self._medFileW = []
      self._meshNameW = []
      self._meshdimW = []
      self._testMode = testMode
      self._extraValidation = TrustExtraValidation()
        
    def initialize(self):
      """ Initialize is called later than __init__() so that the Desktop and the SgPyQt
      objects can be properly initialized.
      """
      self._currID = 1234
      self._sgDesktop = self
      self.createIDs()
      self.createActions()        
      self.createToolbars()
      self.createMenus()
      
#      self.connect(self.validateAction,SIGNAL("activated()"),self.validate)
#      self.connect(self.resetAction,SIGNAL("activated()"),self.reset)
#      self.connect(self.saveAction,SIGNAL("activated()"),self.save)
#      self.connect(self.startAction,SIGNAL("activated()"),self.start)

    def generateID(self):
      self._currID += 1
      return self._currID
    
    def createIDs(self):
      # Actions
      pass
#      self.validateActionID = self.generateID()
#      self.resetActionID = self.generateID()
#      self.startActionID = self.generateID()

    def createActions(self):
      pass
#      ca = self._sgPyQt.createAction
#      self.validateAction = ca(self.validateActionID, "Validate", "Validate", "", "")
#      self.resetAction = ca(self.resetActionID, "Reset", "Reset", "", "")
#      self.startAction = ca(self.startActionID, "Start", "Start", "", "")

    def createToolbars(self):
      pass

    def createMenus(self):
      pass
#        curveMenu = self._sgPyQt.createMenu( "Curve test", -1, self.etudeMenuID, self._sgPyQt.defaultMenuGroup() )
#        self._sgPyQt.createMenu(self.curveSameFigAction, curveMenu)

    def createView(self):
      self.createWidgetAndValidator()
            
      self._mainWidget.validateButton.clicked.connect(self.validate)
      self._mainWidget.saveButton.clicked.connect(self.save)
#      self._mainWidget.resetButton.clicked.connect(self.reset)
      self._mainWidget.startButton.clicked.connect(self.start)
      
      self._dynWidget.setVisibility(True)
      self._mainWidget.dynHolder.setLayout(self._layout)
      vid = self._sgPyQt.createView(self.VIEW_TYPE, self._mainWidget)
      return vid
     
    def registerAllValidationRules(self):
      generator, extra_valid = self._generator, self._extraValidation
      generator.registerCustomValidation("/", extra_valid.validateNonVoidObj, 0)
      generator.registerCustomValidation("medium", extra_valid.validateNonNone, 0)
      
      generator.registerCustomValidation("med_file", extra_valid.validateFile, 10)  
      generator.registerCustomValidation("mesh_name", extra_valid.validateMesh, 20)
      generator.registerCustomValidation("dimension", extra_valid.validateDim, 30)
      generator.registerCustomValidation("bound_cond", extra_valid.validateBC, 30)
      generator.registerCustomValidation("velocity", extra_valid.validateVelocity, 30)
      generator.registerCustomValidation("geo_types", extra_valid.validateGeoTypes, 30)
      generator.registerCustomValidation("turb_model", extra_valid.validateNonNone, 30)
      # Temperature in Wall BC shall not be None:
      generator.registerCustomValidation("temperature", extra_valid.validateNonNone, 30)
     
    def createWidgetAndValidator(self, path=None):
      """ Minimum required for the validation of a dataset """
      import os
      from MainWizWidget import MainWidget
      from jig.loader import Loader
      from jig import widgets

      self._mainWidget = MainWidget()
      if path is None:
        rootDir = os.environ.get("TRUST_WIZARD_ROOT_DIR")
        path = os.path.join(rootDir, "share", "salome", "resources", "trust_wizard")
      if not self._testMode:
        self._loader = Loader(path, "trust_schema.json", "trust_schemas:")
      else:
        self._loader = Loader(path, "schema_composed.json", "http://localhost/")
      self._loader.loadAndValidateSchemas()
      self._generator = self._loader.buildGenerator()
      # Custom widgets:
      self._generator.registerCustomWidget("mesh_name", widgets.JigComboBoxUnicode)
      # Custom validations:
      self.registerAllValidationRules()
      # Generate dynamic widget from JSon:
      self._dynWidget, self._layout = self._generator.generate()
      self._mainWidget._dynWidget = self._dynWidget
      # Add custom widget for group choosing:
      self._mainWidget.addGroupChoosers()
      self._validator = self._generator.getValidator(self._dynWidget)
      # Set up dynamic aspects:
      if not self._testMode:
        self._mainWidget.setUpBoundConditions()
        self._mainWidget.setUpTurbulenceLink()
        self._mainWidget.setUpDomainWidget()

    def showCentralWidget(self):
      if self._viewID == -1:
        self._viewID = self.createView()
      else:
        self._sgPyQt.setViewVisible(self._viewID, True)
        self._sgPyQt.activateView(self._viewID)
        
    def hideCentralWidget(self):
      if self._viewID != -1:
        self._sgPyQt.setViewVisible(self._viewID, False)
        
    @Slot()
    def validate(self):
      import pprint as pp
      self._lastData = self._dynWidget.getJSon()
      pp.pprint(self._lastData)
      ret, path = self._validator.validateInput()
      if ret == "":
        self._mainWidget.setStatus("All ok!")
      else:
        print path
        self._mainWidget.setStatus(ret[:100])
      return ret
    
    @Slot()
    def save(self):
      import json
      from pyqtside.QtGui import QMessageBox, QFileDialog
      ret = self.validate()
      if ret != "":
        QMessageBox.warning(self, "Invalid data", "Invalid data! Please correct before saving.")
        return 
      import os
      fName = QFileDialog.getSaveFileName(self, "Save wizzard data",
                                       os.curdir,
                                       "JSon file (*.json)")
      if fName != "":
        with open(fName, "w") as f:
          json.dump(self._lastData, f, indent=4)
          self._mainWidget.setStatus("File saved!")
      
    @Slot()
    def start(self):
      import json
      from pyqtside.QtGui import QMessageBox, QFileDialog
      ret = self.validate()
      if ret != "":
        QMessageBox.warning(self, "Invalid data", "Invalid data! Please correct before saving.2")
        return 
      import os
      fName = QFileDialog.getSaveFileName(self, "Save data",
                                       os.curdir,
                                       "data file (*.data)")
      if fName != "":
          #with open(fName, "w") as f:
          #json.dump(self._lastData, f, indent=4)
          import First
          reload(First)
          #from First import convert_json_to_jdd
          First.convert_json_to_jdd(self._lastData,fName)
          self._mainWidget.setStatus("Data file saved!")

          
          from trust_plot2d.DirScanWidget import run_TRUST
          run_TRUST(str(fName))
          cmd="trust -evol "+str(fName)
          cmd=cmd.split()
          print "cmd",cmd
          import subprocess
          subprocess.call(cmd)
          print "Ok"
    @Slot()
    def reset(self):
      print "RESET - not impl. yet"
