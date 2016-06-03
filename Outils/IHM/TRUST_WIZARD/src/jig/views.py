#
# Author : A. Bruneton
#

from pyqtside.QtGui import QGridLayout, QVBoxLayout, QTabWidget, QWidget, QLabel, QGroupBox
from pyqtside.QtCore import Qt

def setup4Lay(lay):
  """ Handy function to setup a 4-cols layout """
  lay.setColumnStretch(0, 0)
  lay.setColumnStretch(1, 10)
  lay.setColumnStretch(2, 20)
  lay.setColumnStretch(3, 10)
  
class JigView(object):
  """ A view holds all the logic necessary to the display of the widgets within a Layout
  """
  def __init__(self, jig_widget):
    self._widget = jig_widget
    self._visible = True
    self._active = True
  def placeMeIn(self, gridlay, line, compact=False):
    """  @return the number of rows used    """
    raise NotImplementedError
  def setVisibility(self, visible, update_qt=True):
    """ Set widget visibility. If update_qt is set to False, only the internal state of the widget is changed (this is an advanced usage)
    """
    self._visible = visible
  def setActive(self, enable):
    self._active = enable
  def isActive(self):
    return self._active
  def setHighlight(self, highlight):
    pass
  def aboutToHighlightChild(self, index):  # only for tab views to set the current tab
    pass
  def setDescription(self, desc):
    pass
  def getLayout(self):
    return None

class JigViewVoid(JigView):
  def __init__(self, jig_widget):
    JigView.__init__(self, jig_widget)
    self._active = False
  def placeMeIn(self, gridlay, line, compact=False):
    return 0
  def isActive(self):
    return False

class JigViewSW(JigView):
  """ Single widget view - abstract """
  def __init__(self, jig_widget, title, desc): 
    """ jig_widget is a JigComposed """
    JigView.__init__(self, jig_widget)
    self._mainW = None
    self._title = title
    self._layout = None
    self._leaves = jig_widget._leaves
    self._desc = QLabel(desc)
    self._invisibleWidgets = []    # Some widgets (notably the reference item in an array) must be attached to the parent QWidget but not shown.
  def setActive(self, enable):
    JigView.setActive(self, enable)
    self._mainW.setEnabled(enable)
  def setVisibility(self, visible, update_qt=True):
    JigView.setVisibility(self, visible)
    if not self._mainW is None and update_qt: 
      if visible:  
        self._mainW.show()
        self._desc.show()
      else:        
        self._mainW.hide()
        self._desc.hide()
  def addInvisibleWidget(self, w):
    self._invisibleWidgets.append(w)
  def pushToView(self, w, compact=False):
    subRowNb = self._layout.rowCount()
    return w._view.placeMeIn(self._layout, subRowNb, compact=compact)
  def popFromView(self):
    pass    
  def placeMeIn(self, gridlay, rowNumb, compact=False):
    gridlay.addWidget(self._desc, rowNumb, 1, 1, 4)
    self.placeLeaves(withSpacer=not compact)
    gridlay.addWidget(self._mainW, rowNumb+1, 1, 2, 3)  # Spanning 2 lines and 3 cols
    self.placeInvisible(gridlay, rowNumb+1)
    self.setVisibility(self._visible)
    return 3  
  def placeInvisible(self, gridlay, rowNum):
    for w in self._invisibleWidgets:
      w._view.placeMeIn(gridlay, rowNum, compact=True)
      w.setVisibility(False)
  def placeLeaves(self, withSpacer=True):
    raise NotImplementedError
  def setHighlight(self, highlight):
    if highlight:
      self._desc.setStyleSheet("QLabel {  color : red; }")
    else:
      self._desc.setStyleSheet("QLabel {  color : black; }")
  def setDescription(self, desc):
    self._desc.setText(str(desc))
  def getLayout(self):
    return self._layout
    
class JigViewMW(JigView):
  """ Multiple widget view - typically used for JigLeafWidget """
  def __init__(self, jig_widget):
    """ jig_widget is a JigLeafWidget """ 
    JigView.__init__(self, jig_widget)
    self._widgets = jig_widget._widgets
 
  def setActive(self, enable):
    JigView.setActive(self, enable)
    self._widgets[1].setEnabled(enable)
  
  def setVisibility(self, visible, update_qt=True):
    JigView.setVisibility(self, visible)
    if update_qt:
      if visible:
        self._widgets[0].show()
        self._widgets[1].show()
        if len(self._widgets) > 2:
          self._widgets[2].show()
      else:
        self._widgets[0].hide()
        self._widgets[1].hide()
        if len(self._widgets) > 2:
          self._widgets[2].hide()

  def placeMeIn(self, gridlay, rowNumb, compact=False):
    gridlay.addWidget(self._widgets[0], rowNumb, 1)
    gridlay.addWidget(self._widgets[1], rowNumb, 2)
    if len(self._widgets) > 2:
      gridlay.addWidget(self._widgets[2], rowNumb, 3)
    self.setVisibility(self._visible)
    return 1

  def setHighlight(self, highlight):
    if highlight:
      self._widgets[0].setStyleSheet("QLabel {  color : red; }")
      self._widgets[1].setFocus(Qt.OtherFocusReason) 
    else:
      self._widgets[0].setStyleSheet("QLabel {  color : black; }")

class JigViewBlock(JigViewSW):
  def __init__(self, jig_widget, title, desc):
    JigViewSW.__init__(self, jig_widget, title, desc)
    self._layout = QGridLayout()
    setup4Lay(self._layout)
    self._buildMain()
    self._mainW.setLayout(self._layout)
  
  def placeLeaves(self, withSpacer=True):
    subRowNb = 1 
    for l in self._leaves:
      subRowNb += self.pushToView(l, compact=True)  # sub-objects are always pushed in compact mode
    if withSpacer:
      # Extra spacer at bottom
      self._layout.setRowStretch(subRowNb+1, 10)

  def popFromView(self):  
    row = self._layout.rowCount()-2 # take the last spacer into account  
    for col in range(4): 
      it = self._layout.itemAtPosition(row, col)
      if not it is None:
        w = it.widget()
        if not w is None:
          w.hide()
  
  def fixTabOrder(self):
    """ Ensure correct tab order """
    from pyqtside.QtGui import QLineEdit
    last = None
    nl, nc =  self._layout.rowCount(), self._layout.columnCount()
    for i in range(nl):
      for j in range(nc):
        it = self._layout.itemAtPosition(i, j)
        if not it is None:
          itw = it.widget()
          if isinstance(itw, QLineEdit):
            if last is not None:
              QWidget.setTabOrder(last, itw)
            last = itw

class JigViewGroup(JigViewBlock):
  """ A single widget: a group box (holding itself several child widgets) """
  def __init__(self, jig_widget, title, desc):
    JigViewBlock.__init__(self, jig_widget, title, desc)  
  def _buildMain(self):
    self._mainW = QGroupBox()

class JigViewBlockWidget(JigViewBlock):
  """ A single widget: a QWidget (holding itself several child widgets) """
  def __init__(self, jig_widget, title, desc):
    JigViewBlock.__init__(self, jig_widget, title, desc)  
  def _buildMain(self):
    self._mainW = QWidget()
  def setHighlightArray(self, highlight):
    """ Highlight all labels in the QWidget """
    for c in self._mainW.findChildren(QLabel):
      if highlight:
        c.setStyleSheet("QLabel {  color : red; }")
      else:
        c.setStyleSheet("QLabel {  color : black; }")
  
class JigViewRadio(JigViewBlockWidget):
  def __init__(self, jig_widget, title, desc):
    JigViewBlockWidget.__init__(self, jig_widget, title, desc)
    self._radios = jig_widget._radios
    
  def placeLeaves(self, withSpacer=True):
    # Placing leaves first:
    subRowNb = 1 
    for i, l in enumerate(self._leaves):
      inc = self.pushToView(l, compact=True)
      self._layout.addWidget(self._radios[i], subRowNb, 0, Qt.AlignCenter)
      subRowNb += inc
    if withSpacer:
      # Extra spacer at bottom
      self._layout.setRowStretch(subRowNb+1, 10)

class JigViewTab(JigViewSW):
  """ A single widget: a tab widget (holding itself several child widgets) """
  def __init__(self, jig_widget,title, desc):
    JigViewSW.__init__(self, jig_widget, title, desc)
    self._mainW = QWidget()
    self._tabW = QTabWidget()
    self._layout = QVBoxLayout()
#    self._layout.addWidget(QLabel("<b>%s</b>" % title))
#    self._layout.addWidget(QLabel(desc))
    self._layout.addWidget(self._tabW)
    self._mainW.setLayout(self._layout)
    self._sub_lay = []
    self._sub_widgets = []

  def placeLeaves(self, withSpacer=True):
    for i, l in enumerate(self._leaves):
      self.pushToView(l, compact=False)
      # Extra spacer at bottom - the withSpacer argument is ignored on purpose
      lay = self._sub_lay[i]
      lay.setRowStretch(lay.rowCount(), 10)
  
  def pushToView(self, jig_widget, compact=False):
    w, lay = QWidget(), QGridLayout()
    setup4Lay(lay)
    self._sub_widgets.append(w)  
    self._sub_lay.append(lay)
    w.setLayout(lay)
    self._tabW.addTab(w, jig_widget.getTitle())
    jig_widget._view.placeMeIn(lay, 0)
    
  def popFromView(self):
    """ Remove the last added tab """
    self._tabW.removeTab(self._tabW.count()-1)
  
  def aboutToHighlightChild(self, index):
    self._tabW.setCurrentIndex(index)

class JigViewDynamicCombo(JigViewSW):
  def __init__(self, jig_widget, title, desc):
    JigViewSW.__init__(self, jig_widget, title, desc)
    self._mainW = QWidget()
    self._layout = QGridLayout()
    setup4Lay(self._layout)
    self._layout.addWidget(self._widget._combo, 1,0,1, 3) 
    self._mainW.setLayout(self._layout)

  def placeLeaves(self, withSpacer=True):
    maxNbRow = -1
    for l in self._leaves:
      n = l._view.placeMeIn(self._layout, 2)
      if maxNbRow != -1:
        l.setVisibility(False)
      if n > maxNbRow: maxNbRow = n
    if withSpacer:
      self._layout.setRowStretch(2+maxNbRow, 10)
