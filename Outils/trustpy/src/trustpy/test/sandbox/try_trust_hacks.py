# to append to trust_hacks for example not KISS hacks


def getEdge_1(self):
  return ("%s %s %s" % (self.dir, self.eq, self.pos))

def getEdge_2(self):
  return ("%s %s %s %s %s" % (self.pos2_min, self.inf1, self.dir2, self.inf2, self.pos2_max))

# hack defbord_2 (for example)
if False: #LOG.isChristian():
  defbord_2_Tru._attributesList.extend( [
    ("edge_1", "StrXyz"),
    ("edge_2", "StrXyz") ] )

  defbord_2_Tru.getEdge_1 = getEdge_1
  defbord_2_Tru.getEdge_2 = getEdge_2

  def isHidden(self, nameAttr):
    """to know if attribute is currently displayed in treeView and other dialog widget"""
    if nameAttr in "edge_1 edge_2".split():
      return False  # not hidden
    return True  # hidden

  def checkValues(self, verbose=True):
    """TOO"""
    print("%s.checkValues" % self._className)

  def on_attributesChange(self, verbose=False):
    print("%s.on_attributesChange" % self._className)

  def setattr(self, name, value):
    """There is no GUI check and set from edge to dir etc."""
    newvalue = value
    if name == "edge_1":
      newvalue = "%s %s %s" % (self.dir, self.eq, self.pos)
      print("%s.__setattr__ '%s' = '%s' " % (self.__class__.__name__, name, newvalue))
    if name == "edge_2":
      newvalue = "%s %s %s %s %s" % (self.pos2_min, self.inf1, self.dir2, self.inf2, self.pos2_max)
      print("%s.__setattr__ '%s' = '%s' " % (self.__class__.__name__, name, newvalue))

    super(defbord_2_Tru, self).__setattr__(name, newvalue)

  def setCurrentVariables(self):
    """TOO"""
    print("%s.setCurrentVariables" % self._className)

  # defbord_2_Tru.isHidden = isHidden
  defbord_2_Tru.checkValues = checkValues
  defbord_2_Tru.on_attributesChange = on_attributesChange
  defbord_2_Tru.__setattr__ = setattr