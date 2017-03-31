#
# Author : A. Bruneton
#

from pyqtside import QtGui, QtCore
from pyqtside.QtCore import Slot
from pyqtside.uic import loadUiGen
from trust_wiz.utils import completeResPath
from jig import widgets
from extra_validation import getMCTypeMap

class MainWidget(QtGui.QWidget):
  def __init__(self):
    QtGui.QWidget.__init__(self)
    loadUiGen(completeResPath("MainWizWidget.ui"), self)
    self._dynWidget = None
    self._groupChoosers = {}   # Widgets to choose a sub-set of groups in the MED file

  def addGroupChoosers(self):
    from GroupChooser import GroupChooser
    domains = self._dynWidget.findChildren("domain")
    for d in domains:
      pbType, pbRootPath = d.getPath()[2], d.getPath()[:2]
      l = d.getLayout()
      grpChoo = GroupChooser(pbRootPath) 
      self._groupChoosers[pbType] = grpChoo
      l.addWidget(grpChoo, l.rowCount(), 0, 1, 4)
      grpChoo.changeSelectedGroups.connect(self.onChangeSelectedGroups)
      grpChoo.setVisible(False)

  def setStatus(self, txt):
    self.statusLabel.setText(txt)

  def setUpTurbulenceLink(self):
    turbs = self._dynWidget.findChildren("turb_model")
    for turb in turbs:
      turb.onComboChanged.connect(self.onTurbulenceChange)
      # By default, k-eps hidden:
      self.onTurbulenceChange(turb.getPath(), "Laminar")

  def setUpDomainWidget(self):
    domainW = self._dynWidget.findChildren("domain")
    for d in domainW:
      inputFileW = d.getChild("med_file")
      meshNameW = d.getChild("mesh_name")
      dimW = d.getChild("dimension")
      geoType = d.getChild("geo_types")
      geoType.hideFromGUI()
      dimW.setActive(False)   # dimension is given by the file and is actually not editable directly
      inputFileW.onFilePathChanged.connect(self.onMEDFileChange)
      meshNameW.onComboChanged.connect(self.onMEDMeshChange)

  def setUpBoundConditions(self):
    """ Keys "group_name" and "bc_type" are always hidden """
    bcs = self._dynWidget.findChildren("bound_cond")
    for bc in bcs:
      refItem = bc.getItem()
      # Hide all group names:
      for w in refItem.findChildren("group_name"):
        w.hideFromGUI()
      # Hide all bc_type widgets
      for t in refItem.findChildren("bc_type"):
        t.hideFromGUI()

  def makeKEPSVisibleAndInvalid(self, w, visible):
    w.setVisibility(visible)
    if not isinstance(w, widgets.JigObjectVoid):
      if not visible:
        # Make k-epsilon equation valid
        w.getChild(0).setText(0.0)
        w.getChild(1).setText(0.0)
      else:
        w.getChild(0).setText('')
        w.getChild(1).setText('')

  @Slot(list, str)
  def onTurbulenceChange(self, widgetPath, turb_model):
    visible_keps = (turb_model == "k-epsilon")
    rootPath = widgetPath[:-1]
    # Initial conditions
    ic = self._dynWidget.resolvePath(rootPath + ["init_cond"])
    for w in ic.findChildren("k-epsilon"):
      self.makeKEPSVisibleAndInvalid(w, visible_keps)
    # Boundary conditions
    bc = self._dynWidget.resolvePath(rootPath + ["bound_cond"])
    #    Also change array reference item:
    refItem = bc.getItem()
    for w in refItem.findChildren("k-epsilon") + bc.findChildren("k-epsilon"):
      self.makeKEPSVisibleAndInvalid(w, visible_keps)
        
  @Slot(list, str)        
  def onMEDFileChange(self, widgetPath, filePath):
#    print "file change"
    import os
    import MEDLoader
    try:
      if not os.path.isfile(filePath):
        raise Exception
      mesh_names = MEDLoader.GetMeshNames(str(filePath))
    except:
      mesh_names = []
    widgetPath[-1] = "mesh_name"
    meshNameW = self._dynWidget.resolvePath(widgetPath)
    meshNameW.populate(mesh_names)
    if mesh_names == []:
      self.onMEDMeshChange(widgetPath, "")
    
  @Slot(list, str)
  def onMEDMeshChange(self, widgetPath, mesh_name):
#    print "mesh change"
    from extra_validation import getMCTypeMap
    dim = 1
    groups = []
    geotypes = []
    mesh_name = str(mesh_name)
    if mesh_name != "":
      import MEDLoader
      import MEDLoader as ml
      try:
        medFileW = self._dynWidget.resolvePath(widgetPath[:-1] + ["med_file"])
        filePath = str(medFileW.getJSon())
        mesh = MEDLoader.ReadMeshFromFile(filePath, mesh_name)
        dim = mesh.getCoords().getNumberOfComponents()
        mfm = ml.MEDFileMesh.New(filePath, mesh_name)
        groups = mfm.getGroupsOnSpecifiedLev(-1)
        mc_typ_map = getMCTypeMap()
        geotypes = [ mc_typ_map[t] for t in mesh.getAllGeoTypes() ]
      except:
        pass
    self.changeGroups(widgetPath, groups)
    self.changeDimension(widgetPath, dim)
    self.changeGeoTypes(widgetPath, geotypes)
    
  @Slot(list, list)
  def onChangeSelectedGroups(self, groups, pbPath):
#    pbPath = [u"/", unicode(pb_type)]
    w = self._dynWidget.resolvePath(pbPath)
    bcs = w.findChildren("bound_cond")
    assert(len(bcs) == 1)
    bc = bcs[0]
    bc.setNumberOfItems(len(groups))
    for i, g in enumerate(groups):
      bc.setElementTitle(i, g)
      bc_single = bc.getChild(i) 
      self.setUpSingleGroup(bc_single, i, g)
    
  def changeGeoTypes(self, widgetPath, geotypes):
#    print "changein geo types", geotypes
    rootPath = widgetPath[:-1]
    w = self._dynWidget.resolvePath(rootPath + ["geo_types"])
    w.setNumberOfItems(len(geotypes))
    for i,g in enumerate(geotypes):
      w2 = w.getChild(i)
      w2.setText(g)
    
  def changeDimension(self, widgetPath, dim):
    rootPath = widgetPath[:-1]
    meshDimW = self._dynWidget.resolvePath(rootPath+["dimension"])
    meshDimW.setIndex(dim-1)
    rootPath = rootPath[:2]   # Select sub problem
    subPb = self._dynWidget.resolvePath(rootPath)
    # Also change reference item of the BC array:
    bcs = subPb.findChildren("bound_cond")
    assert(len(bcs) == 1)
    bc = bcs[0]
    refItem = bc.getItem()
    speedWs = refItem.findChildren("velocity") + subPb.findChildren("velocity")+subPb.findChildren("gravity") 
    for w in speedWs:
      if isinstance(w, widgets.JigArray):
        w.setNumberOfItems(dim)
        w._view.fixTabOrder()
        
  def changeGroups(self, widgetPath, groups):
#    print "change groups:", groups
    pbType = widgetPath[2]
    self._groupChoosers[pbType].onNewGroups(groups)
    # Emulate user always adding  all groups: 
    self._groupChoosers[pbType].onAddAll()
               
  def setUpSingleGroup(self, bc_single, idx, grp_name):
    bc_single.setDescription("Choose boundary condition for group '%s'" % grp_name)
#    print bc_single.getPath()
    ws = bc_single.findChildren("group_name")
    for w in ws:
      w.setText(grp_name)
    for w in bc_single.findChildren("wall_type"):
      p = w.getPath()
      w.hideFromGUI()
      parent = self._dynWidget.resolvePath(p[:-1])
      parent.setTitle(w.getTitle())   

