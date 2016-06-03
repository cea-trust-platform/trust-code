#
# Author : A. Bruneton
#

from pyqtside.QtGui import QWidget, QLabel, QGroupBox, QComboBox, QSizePolicy, QButtonGroup
from pyqtside.QtCore import QObject, Qt, SIGNAL, Signal, Slot
import views

class JigWidget(QObject):
  def __init__(self, key ,title, desc):
    QObject.__init__(self)
    self._key = key
    self._title = title
    self._desc = desc
    self._path = [key]
    self._view = None # a views.JigView object (or derived)
  def getKey(self):
    return self._key
  def getTitle(self):
    return self._title
  def setTitle(self, title):
    self._title = title
  def setDescription(self, desc):
    self._desc = desc
    self._view.setDescription(desc)
  def getPath(self):
    return self._path
  def getJSon(self):
    raise NotImplementedError
  def setHighlightPath(self, path, highlight):
    """ Highlight (or un-highlight) an object causing an error """
    pass
  def getChild(self, key_or_num):
    raise NotImplementedError
  def findChildren(self, key):
    raise NotImplementedError
  def resolvePath(self, path):
    if not len(path) or path[0] != self._key:
      return None
    w = self
    for p in path[1:]:
      w2 = w.getChild(p)
      if w2 is None:   return None
      else:            w = w2
    return w
  def _completePath(self, otherW):
    self._path = otherW._path + self._path
  def clone(self, newKey=None):
    cls = self.__class__
    key = newKey or self._key
    ret = cls(key, self._title, self._desc)
    # Do not invoke setVisibility() here: we don't want to actually show the widget as soon as it is cloned 
    # (would lead to many blinking stuff on screen everytime an array is extended). So just copy the attribute:
    ret._view._visible = self._view._visible
    ret.setActive(self.isActive())
    return ret
  def setVisibility(self, visible):
#    if not self._visible:
#      print "hiding", self._key, self._path
    return self._view.setVisibility(visible)
  def setActive(self, enable):
#    print "setActive of ", self.getPath()
    return self._view.setActive(enable)
  def isActive(self):
    return self._view.isActive()
  def getLayout(self):
    return self._view.getLayout()
  def hideFromGUI(self):
    self.setVisibility(False)
    self.setActive(False)  # to avoid validation errors on this widget!
  def isConstrained(self):
    """ A 'constrained' widget is a combo box or a set of radio buttons, for which there can
    not be any validation error, since the user can only select in a set of pre-defined valid choices. """ 
    return False

class JigComposed(JigWidget):
  """ Anything that contains more than one child """
  def __init__(self, key, title, desc):
    JigWidget.__init__(self, key, title, desc)
    self._leaves = []

  def _completePath(self, otherW):
    JigWidget._completePath(self, otherW)
    for l in self._leaves:
      l._completePath(otherW)

  def clone(self, newKey=None):
    ret = JigWidget.clone(self, newKey)
    for l in self._leaves:
      c = l.clone()
      c._completePath(ret)
      ret._leaves.append(c)
    return ret    
  
  def getChild(self, key_or_num):
    """ Return a child according to its key (str) or its ordinal position (int) """
    if type(key_or_num) is int:
      if key_or_num < len(self._leaves):
        return self._leaves[key_or_num]
      else:
        return None
    else:
      for l in self._leaves:
        if l.getKey() == key_or_num:
          return l
      return None
  
  def findChildren(self, key):
    ret = []
    for l in self._leaves:
      if l.getKey() == key:
        ret.append(l)
      l2 = l.findChildren(key)
      ret.extend(l2)
    return ret
  
  def setHighlightPath(self, path, highlight):
    if self.resolvePath(path) is None:
      return
    if not highlight:
      self._view.setHighlight(highlight)
    if len(path) >= 2:
      w = self.resolvePath(path[:2]) # direct child
      w.setHighlightPath(path[1:], highlight)
      # Find child index in the _leaves array
      keys = [l._key for l in self._leaves]
      idx = keys.index(w._key)
      if highlight:
        self._view.aboutToHighlightChild(idx)
    else:
      self._view.setHighlight(highlight)        

class JigComposedHeterogen(JigComposed):
  def __init__(self, key, title, desc):
    JigComposed.__init__(self, key, title, desc)
 
  def addJigWidget(self, w, update_qt=True):
    self._leaves.append(w)
    w._completePath(self)

class JigObject(JigComposedHeterogen):
  def __init__(self, key, title, desc):
    JigComposedHeterogen.__init__(self, key, title, desc)
   
  def getChild(self, key):
    """ For an object, child are necessarily strings! """
    if not isinstance(key, basestring):
      return None
    return JigComposedHeterogen.getChild(self, key)
   
  def getJSon(self):
    ret = {}
    for w in self._leaves:
      if not isinstance(w, JigWidget):
        raise ValueError("JigObject has non JigWidget as children!")
      k, j = w.getKey(), w.getJSon()
      ret[k] = j 
    return ret

class JigObjectFlat(JigObject):
  """ Flat representation of an object (all subwidgets inserted in a group box) """
  def __init__(self, key, title, desc):
    JigObject.__init__(self, key, title, desc)
    self._view = views.JigViewBlockWidget(self, title, desc)

class JigObjectVoid(JigObject):
  def __init__(self, key, title, desc):
    JigObject.__init__(self, key, title, desc)
    self._view = views.JigViewVoid(self)

class JigArray(JigComposed):
  """ An array of items, all of the same types. New elements are added by internally cloning 
  a reference item set with "setItem()". Tricky point: this item is ultimately a QWidget and hence
  must be parented in the displayed window (hidden orphan widgets don't behave so well).
  """ 
  def __init__(self, key, title, desc):
    JigComposed.__init__(self, key, title, desc)
    self._item = None
  
  def clone(self, newKey=None):
    ret = JigComposed.clone(self, newKey=newKey)
    ret.setItem(self._item.clone())
    ret._view.placeLeaves()
    return ret
  
  def getChild(self, num):
    """ For an array, child are necessarily integers! """
    if not isinstance(num, int):
      return None
    return JigComposed.getChild(self, num)
  
  def getItem(self):
    return self._item
  
  def setItem(self, w):
    self._item = w
    self._item.setVisibility(False)
    self._view.addInvisibleWidget(w)
    
  def setNumberOfItems(self, n, update_view=True):
    if self._item is None:
      raise ValueError("Call setItem() first!")
    if n == len(self._leaves):
      return
    if n < len(self._leaves):
      for i, w in enumerate(self._leaves[n:]):
        w._view.setVisibility(False)
        if update_view:
          self._view.popFromView()
      self._leaves = self._leaves[:n]
    elif n > len(self._leaves):
      for i in range(len(self._leaves), n):
        w = self._item.clone(newKey=i)
        # Do not show in Qt yet (to avoid widgets appearing all over the place):
        # the final 'pushToView' will do it properly after placement.
        w._view.setVisibility(True,update_qt=False)  
        w._completePath(self)
        w.setTitle("%s (compo %d)" % (self._title, i))
        self._leaves.append(w)
        if update_view:
          self._view.pushToView(w)

  def setElementTitle(self, idx, title):
    self._leaves[idx].setTitle(title)
  
  def getJSon(self):
    ret = []
    for w in self._leaves:
      if not isinstance(w, JigWidget):
        raise ValueError("JigObject has non JigWidget as children!")
      ret.append(w.getJSon()) 
    return ret
    
class JigArrayFlat(JigArray):
  def __init__(self, key, title, desc):
    JigArray.__init__(self, key, title, desc)
    self._view = views.JigViewBlockWidget(self, title, desc)
  
  def setHighlightPath(self, path, highlight):
    """ Override to highlight the whole array when no sub-items could be provided """
    if len(path) == 1:
      self._view.setHighlightArray(highlight)
    return JigArray.setHighlightPath(self, path, highlight)
    
class JigObjectTab(JigObject):
  """ Tabulated representation of an object: one tab per property """
  def __init__(self, key, title, desc):
    JigObject.__init__(self, key, title, desc)
    self._view = views.JigViewTab(self, title, desc)
      
  def clone(self, newKey=None):
    # Some work to do here:
    raise NotImplementedError
    
class JigArrayTab(JigArray):
  def __init__(self, key, title, desc):
    JigArray.__init__(self, key, title, desc)
    self._view = views.JigViewTab(self, title, desc)

  def setElementTitle(self, idx, title):
    JigArray.setElementTitle(self, idx, title)
    self._view._tabW.setTabText(idx, title)

  def clone(self, newKey=None):
    # Some work to do here:
    raise NotImplementedError
  
  def clearArrayLayout(self):
    self._tabW.clear()
    
  def completeArrayLayout(self, widget):
    self.completeTab(widget)
      
class JigOneOf(JigComposedHeterogen):
  def __init__(self, key, title, desc):
    JigComposedHeterogen.__init__(self, key, title, desc)
  
  def getChild(self, num):
    """ For a oneOf, child are necessarily integers! """
    if not isinstance(num, int):
      return None
    return JigComposed.getChild(self, num)

  def getJSon(self):
    cl = self._currentLeaf()
    if not cl is None:
      return cl.getJSon()
    else:
      return None

class JigOneOfRadio(JigOneOf):
  def __init__(self, key, title, desc):
    JigOneOf.__init__(self, key, title, desc)
    self._radios = []  # List of tuple (radio button, qlabel)
    self._but_group = QButtonGroup()
    self._view = views.JigViewRadio(self, title, desc)

  def clone(self, newKey=None):
    """ Also activate the same radio in the clone """
    ret = JigOneOf.clone(self, newKey=newKey)
    for _ in ret._leaves:
      ret.__addRadio()
    return ret

  def __addRadio(self):
    from pyqtside.QtGui import QRadioButton
    r = QRadioButton(self._view._mainW)   # ugly, TODO think about this
    self._radios.append(r)
    self._but_group.addButton(r)
    self.connect(r, SIGNAL("toggled(bool)"), self.onRadio)
#    if len(self._radios) == 1:
#      self._radios[0].setChecked(True)
    self.onRadio()

  def addJigWidget(self, w, update_qt=True):
    JigOneOf.addJigWidget(self, w, update_qt=update_qt)
    self.__addRadio()

  def onRadio(self, togg=True):
    if not togg:
      return
    for i, r in enumerate(self._radios):
      if r.isChecked(): 
        self._leaves[i].setActive(True)
      else:
        self._leaves[i].setActive(False)

  def _currentLeaf(self):
    l = [r.isChecked() for r in self._radios]
    try:
      idx = l.index(True)
    except ValueError:
      return None
    return self._leaves[idx]
    
class JigOneOfCombo(JigOneOf):
  def __init__(self, key, title, desc):
    JigOneOf.__init__(self, key, title, desc)
    self._combo = QComboBox()
    self.connect(self._combo, SIGNAL("currentIndexChanged(int)"), self.onIndexChanged)
    self._currLeaf = None
    self._view = views.JigViewDynamicCombo(self, title, desc)
    self._blockSig = False

  def _currentLeaf(self):
    return self._currLeaf

  def clone(self, newKey=None):
    ret = JigOneOf.clone(self, newKey=newKey)
    # Warning: add item will trigger "onIndexChange"
    ret._blockSig = True
    for w in ret._leaves:
      ret._combo.addItem(w.getTitle())
    ret._blockSig = False
    if len(ret._leaves):
      ret._currLeaf = ret._leaves[0]
      ret._currLeaf._view.setVisibility(True, update_qt=False)
      ret._currLeaf.setActive(True)
      for l in ret._leaves[1:]:
        l.setActive(False)
    return ret

  def onIndexChanged(self, newIdx, update_qt=True):
    if self._blockSig: return
    if not self._currLeaf is None:
      self._currLeaf._view.setVisibility(False)
      self._currLeaf.setActive(False)
    self._currLeaf = self._leaves[newIdx]
    self._currLeaf._view.setVisibility(True, update_qt=update_qt)
    self._currLeaf.setActive(True)

  def addJigWidget(self, w, update_qt=True):
    JigOneOf.addJigWidget(self, w, update_qt=update_qt)
    if not update_qt:
      self._blockSig = True 
    self._combo.addItem(w.getTitle())
    if not update_qt:
      self._blockSig = False
    if len(self._leaves) == 1:
      self.onIndexChanged(0, update_qt=update_qt)
    else:
      w.setActive(False)

class JigLeafWidget(JigWidget):
  def __init__(self, key, title, desc):
    JigWidget.__init__(self, key, title, desc)
    self._lab = QLabel("%s" % title)
#    self._lab.setToolTip(desc)
#    self._lab.setSizePolicy(QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred))
    self._widgets = [self._lab]
    self._view = views.JigViewMW(self)    
  def getChild(self, key_or_num):
    return None
  def findChildren(self, key):
    return []
  def setTitle(self, title):
    JigWidget.setTitle(self, title)
    self._lab.setText(title)
  def setHighlightPath(self, path, highlight):
    if self.resolvePath(path) is None:
      return
    self._view.setHighlight(highlight)

class JigDisplay(JigLeafWidget):
  def __init__(self, key, title, desc):
    JigLeafWidget.__init__(self, key, title, desc)
    self._widgets.append(QLabel(""))
  def getJSon(self):
    return self._key
  
class JigLineEdit(JigLeafWidget):
  def __init__(self, key, title, desc, conversionFunc=None):
    from pyqtside.QtGui import QLineEdit
    JigLeafWidget.__init__(self, key, title, desc)
    self._mainW = QLineEdit()
    self._mainW.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred))
    self._mainW.setToolTip(desc)
    self._widgets.append(self._mainW)
    self._conversionFunc = conversionFunc
  def clone(self, newKey=None):
    ret = JigLeafWidget.clone(self, newKey=newKey)
    ret._mainW.setText(self._mainW.text())
    ret._conversionFunc = self._conversionFunc
    return ret
  def setText(self, txt):
    self._mainW.setText(unicode(txt))
  def getJSon(self):
    return self._conversionFunc(self._mainW.text())

class JigLineEditUnicode(JigLineEdit):
  def __init__(self, key, title, desc):
    JigLineEdit.__init__(self, key, title, desc, conversionFunc=unicode)

class JigLineEditNumber(JigLineEdit):
  def __init__(self, key, title, desc):
    JigLineEdit.__init__(self, key, title, desc, conversionFunc=self.convFunc)
    self._mainW.editingFinished.connect(self.onEditingFinished)
  def convFunc(self, x):
    try: 
      return float(x)
    except:
      return str(x)
  @Slot()
  def onEditingFinished(self):
    try:
      if str(self._mainW.text()) != "":
        float(self._mainW.text())
      self._view.setHighlight(False)
    except:
      self._view.setHighlight(True)
      self._mainW.setFocus()
      self._mainW.selectAll()

class JigFileChooser(JigLineEdit):
  onFilePathChanged = Signal(list, str, name='onFilePathChanged')
  
  def __init__(self, key, title, desc):
    from pyqtside.QtGui import QPushButton
    JigLineEdit.__init__(self, key, title, desc, conversionFunc=str)
    self._filter = "All files (*.*)"
    self._button = QPushButton("Choose ...")
    self._button.setSizePolicy(QSizePolicy(QSizePolicy.Minimum, QSizePolicy.Fixed))
    self.connect(self._button, SIGNAL("clicked()"), self.onChoose)
    self._widgets.append(self._button)
    self._mainW.textChanged.connect(self.onTextChanged)

  @Slot(str)
  def onTextChanged(self, file_pth):
    self.onFilePathChanged.emit(self._path[:], str(file_pth))  # important: a copy of the path!!
  def setFilter(self, file_filt):
    self._filter = file_filt
  def onChoose(self):
    from pyqtside.QtGui import QFileDialog
    import os
    filePth = QFileDialog.getOpenFileName(None, "Open file", os.curdir, self._filter)
    if filePth:    self._mainW.setText(filePth)

class JigSpinBox(JigLeafWidget):
  def __init__(self, key, title, desc):
    from pyqtside.QtGui import QSpinBox
    JigLeafWidget.__init__(self, key, title, desc)
    self._mainW = QSpinBox()
    self._mainW.setSizePolicy(QSizePolicy(QSizePolicy.Preferred, QSizePolicy.Preferred))
    self._mainW.setToolTip(desc)
    self._widgets.append(self._mainW)
  def placeMainWidget(self, gridlay, rowNumb):
    gridlay.addWidget(self._widgets[1], rowNumb, 2, Qt.AlignLeft)
  def getJSon(self):
    return self._mainW.value()

class JigComboBox(JigLeafWidget):
  def __init__(self, key, title, desc):
    JigLeafWidget.__init__(self, key, title, desc)
    self._mainW = QComboBox()
    self._mainW.setSizePolicy(QSizePolicy(QSizePolicy.Expanding, QSizePolicy.Preferred))
    self._mainW.setToolTip(desc)
    self._widgets.append(self._mainW)
  @Slot(int)
  def onComboIndexChanged(self, idx):
    if idx == -1:
      return
    self.onComboChanged.emit(self._path[:], self._mainW.itemData(idx))  # important a copy of the path!!
  def setIndex(self, index):
    self._mainW.setCurrentIndex(index)
  def clone(self, newKey=None):
    ret = JigLeafWidget.clone(self, newKey=newKey)
    ret.populate(self.getValues())
    return ret
  def populate(self, values):
    self._mainW.clear()
    for _, v in enumerate(values):
      self._mainW.addItem(unicode(v), self._convert(v))  
  def getValues(self):
    return [self._mainW.itemData(i) for i in range(self._mainW.count()) ]  
  def getJSon(self):
    idx = self._mainW.currentIndex()
    if idx != -1:
      val = self._mainW.itemData(idx)
    else:
      val = None
    return val

class JigComboBoxUnicode(JigComboBox):
  onComboChanged = Signal(list, str, name='onComboChanged')
  def __init__(self, key, title, desc):
    JigComboBox.__init__(self, key, title, desc)
    self._mainW.currentIndexChanged.connect(self.onComboIndexChanged)
  def _convert(self, v): 
    return str(v)     

class JigComboBoxInt(JigComboBox):
  # only the signal signature changes:
  onComboChanged = Signal(list, int, name='onComboChanged')
  def __init__(self, key, title, desc):
    JigComboBox.__init__(self, key, title, desc)
    self._mainW.currentIndexChanged.connect(self.onComboIndexChanged)
  def _convert(self, v): 
    return int(v)     
