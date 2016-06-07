#
# Author : A. Bruneton
#

import json
import jsonschema as jss
import widgets
#from validator import floatValidator

class Generator(object):
  """ Generates a GUI from a JSon schema. Also generates an enhanced validator object (standard JSON validation + potentially 
  custom validation rules). See registerCustomValidation() and getValidator()
  """
  DEFAULT_DESC = u""
  SCHEMA_ROOT_KEY = u"/"
  
  def __init__(self, loader):
    self._loader = loader
    self._schema = loader.getMasterSchema()
    self._root_layout = None
    self._custom_widget = {}
    self._full_validator = None   # Validator instance, with custom validation rules properly registered
    self._custom_valid_input = {}  
    
  def registerCustomWidget(self, key, cls):
    """ Registers a custom widget for a given key.
    @param key the string of the key for which the custom widget should be called.
    @param cls a class inheriting from JigWidget and implementing the virtuals defined there.
    """
    self._custom_widget[key] = cls
    
  def registerCustomValidation(self, key, func, order=0):
    """ Registers a new custom validation rule. 
    @param key the string of the key against which the validation should be executed
    @param func validation function. Takes as only parameter the sub-schema stored below 'key'. Should return a void string if 
    all OK, an error message otherwise.
    @param order optional int indicating the (relative) order at which this validation rule should be called. Validation
    rules can hence be sequenced.
    """ 
    self._custom_valid_input.setdefault(key, []).append({"func": func, "order": order})
    
  def getValidator(self, widget=None):
    """ @return a complete Validator instance (i.e. with custom validation rules) on the last parsed schema """
    if not widget is None:
      self._full_validator.setWidget(widget)
    return self._full_validator
    
  def generate(self):
    """
    Generate the GUI based on the loaded JSON schema and populate the custom validation rules.
    By default the generated widget is hidden. Invoke setVisibility(True) to display it.
    @return a JigWidget
    @return a QLayout in which the GUI has been set up
    """
    from pyqtside.QtGui import QGridLayout
    from views import setup4Lay
     
    self._full_validator = self._loader.buildJSonValidator()   # only a JSon validator, no custom rules yet
    self.__reset()
    w, _ = self.__generateInternal(self.SCHEMA_ROOT_KEY, [self.SCHEMA_ROOT_KEY], self._schema, 0, "")
    # Finish validator:
    self._full_validator._setSchemaMap(self._schema_map)
    # The layout of the root: 
    self._root_layout = QGridLayout()
    setup4Lay(self._root_layout)
    self._root_layout.setRowStretch(self._root_layout.rowCount(), 10)
    w._view.setVisibility(False)  # To ease testing
    w._view.placeMeIn(self._root_layout, 0)
    self._full_validator.setWidget(w)
    return w, self._root_layout

  def __reset(self):
    self._schema_map = {}
    self._currentRootURI = self._loader.getRootURI()   

  def __prepareCustomRule(self, key, schema_path, func, order):
    self._full_validator._registerCustomValidator(key, func, order)
    simple_path = filter(lambda item: item[0] != "$", schema_path) 
    self._schema_map.setdefault(tuple(simple_path), []).append(schema_path)

  @classmethod
  def _substituteRefs(cls, schema, rootURI):
    if isinstance(schema, dict):
      for k, v in schema.items():
        if k == "$ref":
          arr = v.split("#")
          if len(arr) == 2 and arr[0] == '':
            schema[k] = rootURI + v
        else:
          cls._substituteRefs(v, rootURI)
    elif isinstance(schema, list):
      for l in schema:
        cls._substituteRefs(l, rootURI) 

  def _mergeSchemas(self, d1, d2, rootURI=None):
    """ Merge two schemas: if some keys are sub-schemas (=dict), they are merged themselves. If some keys
    are list, they are fused in the order provided.
    Finally if d1 and d2 have common keys which are neither dict nor list nor '$ref'
    the first value (from d1) is kept.
    TODO: document how references are handled
    """
    ret = {}
    for k1, v1 in d1.items():
      if d2.has_key(k1):
        if isinstance(v1, dict) and isinstance(d2[k1], dict):
          ret[k1] = self._mergeSchemas(d1[k1], d2.pop(k1))
        elif isinstance(v1, list) and isinstance(d2[k1], list):
          ret[k1] = v1 + d2.pop(k1)
        elif k1 == "$ref" and d2.has_key('$ref'):
          # If the two schemas contains a '$ref' the first one is resolved and substituted:
          subSch, rootURI2 = self._loader.getSubSchema(v1, rootURI)
          if rootURI != rootURI2:
            self._substituteRefs(subSch, rootURI2)
          ret = self._mergeSchemas(ret, subSch)   # the recursion won't go further than one level down (no "$ref" yet 
                                              # in ret) so we don't care about rootURI
        else:
          # Keeping first value
          ret[k1] = v1
          d2.pop(k1)
      else:
        ret[k1] = v1
    ret.update(d2) # Remaining stuff from d2:
    return ret

  def __generateInternal(self, key, path, schema, level, ze_title=None, ze_desc=None):
    """ Crossed recursion with __genObject() and __getArray() 
    The accumulated 'path' is of the form ['/', '$1', 'toto', '#'] where items starting with '$' denote
    a 'oneOf' clause, and items '#' denote an array item (position in the array is irrelevant).  
    This is used later by the validator.   
    """
    cvi = self._custom_valid_input 
    if cvi.has_key(key):
      for v in cvi[key]:
        self.__prepareCustomRule(key, path, v["func"], v["order"])
    
    ks = schema.keys()
    title = schema.get("title", ze_title)
    if title is None: title = str(key)
    desc = schema.get("description", ze_desc)
    if desc is None: desc = title
    # Sanity check: top most level must be an "object"
    newLev = level
    if self._custom_widget.has_key(key):
      # Custom widget generation
      widgCls = self._custom_widget[key] 
      w = widgCls(key, title, desc)
    elif "$ref" in ks:
      # References take precedence over all the rest - the current schema is completed with the content of the
      # ref, and we recurse. The current root URI is updated to allow nested $ref:
      oldRootURI = self._currentRootURI
      subSchema, self._currentRootURI = self._loader.getSubSchema(schema["$ref"], oldRootURI)
      schema2 = schema.copy(); schema2.pop("$ref")  # :-) otherwise inf recursion ...
      subSchema = self._mergeSchemas(subSchema, schema2)
      w, newLev = self.__generateInternal(key, path, subSchema, level, ze_title=title)  # same level!
      # Restore prev root URI:
      self._currentRootURI = oldRootURI
      return w, newLev
    elif "oneOf" in ks:
      subSchemas = schema["oneOf"]  # necessarily a list with at min one item (valid schema).
      typ = schema.get("gui_type", None)
      cls_map = {None: None,"oneOf_combo": widgets.JigOneOfCombo, "oneOf_radio":widgets.JigOneOfRadio }
      w, newLev = self.__genOneOf(key, path, title, desc, subSchemas, level, cls=cls_map[typ])
#    elif "anyOf" in ks:
#      # anyOf is exactly similar to oneOf from a GUI perspective (but not from a schema one):
#      subSchemas = schema["anyOf"]  # necessarily a list with at min one item (valid schema).
#      w, newLev = self.__genOneOf(key, title, desc, subSchemas, level)
    elif "allOf" in ks:
      subSchemas = schema["allOf"]  # necessarily a list with at min one item (valid schema).
      # Concatenate all sub-schemas:
      concat = {}
      for sub in subSchemas:
        concat = self._mergeSchemas(concat, sub, self._currentRootURI)
      w, newLev = self.__generateInternal(key, path, concat, level, ze_title=title, ze_desc=desc)  # same level!
      return w, newLev
    elif "enum" in ks:
      values = schema["enum"]       # necessarily a list (valid schema).
      if not len(values):
        raise ValueError("Void enum!")
      default = schema.get("default", values[0])
      typ = type(default)
      w = self.__genCombo(key, title, desc, default, values, typ)
    else:
      typ = schema.get("gui_type", None)   # gui_type takes precedence for GUI generation
      if typ is None: typ = schema.get("type", "object")
      if typ == "integer":
        dflt = schema.get("default", -1)
        w = self.__genInteger(key, title, desc, dflt)
      elif typ == "string":
        dflt = schema.get("default", "")
        w = self.__genString(key, title, desc, dflt)
      elif typ == "number":  
        dflt = schema.get("default", '')   # By default float numbers are left empty
        w = self.__genNumber(key, title, desc, dflt)
      elif typ == "file_path":
        dflt = schema.get("default", "")
        filt = schema.get("gui_file_filter", "All files (*.*)")
        w = self.__genFilePath(key, title, desc, dflt, filt)
      # Recurse here:
      elif typ == "object":
        w, newLev = self.__genObject(key, path, title, desc, schema.get("properties", {}), level)
      elif typ == "object_tab":
        w, newLev = self.__genObject(key, path, title, desc, schema.get("properties", {}), level,cls=widgets.JigObjectTab)
      elif typ == "object_flat":
        w, newLev = self.__genObject(key, path, title, desc, schema.get("properties", {}), level,cls=widgets.JigObjectFlat)
      elif typ == "array":
        minItems = schema.get("minItems", 0)
        w, newLev = self.__genArray(key, path, title, desc, schema["items"], minItems, level)
      elif typ == "array_tab":
        minItems = schema.get("minItems", 0)
        w, newLev = self.__genArray(key, path, title, desc, schema["items"], minItems, level, cls=widgets.JigArrayTab)
      else:
        raise ValueError("No matching widget in generate routine for type  '%s'!" % typ)
    return w, newLev

  def __genObject(self, key, path, title, desc, dicProps, level, cls=None):
    """ Generate a flat object (GroupBox) if the objects below do not go too deep, or if there is just one
    property in the object (happens typically for a root), otherwise 
    a tabulated widget """
    if not len(dicProps) and cls is None:
      return widgets.JigObjectVoid(key, title, desc), level
    inf = 10000000 # Unordered widgets are placed at bottom
    maxLev = -1
    l = []
    for p in dicProps:
      w, lev = self.__generateInternal(p, path + [p], dicProps[p], level+1) 
      pos = dicProps[p].get("gui_order", inf)
      if lev > maxLev: maxLev = lev
      l.append((pos, w))
    if cls is None:
      if maxLev > level+2 and len(l) > 1:    
        wMain = widgets.JigObjectTab(key, title, desc)
      else:                   
        wMain = widgets.JigObjectFlat(key, title, desc)
    else:
      wMain = cls(key, title, desc)  
    l.sort()
    for _, w in l:
      wMain.addJigWidget(w, update_qt=False)
    return wMain, maxLev

  def __genOneOf(self, key, path, title, desc, listSub, level, cls=None):
    """ Generate a set of radio button if the objects below do not go too deep, otherwise 
    a combo box.
    In the accumulated path, a tag of the form '$1' is inserted instead of the usual '1'.
     """
    maxLev, lst = -1, []
    for i, l in enumerate(listSub):
      w, lev = self.__generateInternal(i, path + ["$%d" % i], l, level+1)
      if lev > maxLev: maxLev = lev
      lst.append(w)
    if cls is None:
      if maxLev > level+3:     
        wMain = widgets.JigOneOfCombo(key, title, desc)
      else:                   
        wMain = widgets.JigOneOfRadio(key, title, desc)
    else:
      wMain = cls(key, title, desc)
    for w in lst: 
      wMain.addJigWidget(w, update_qt=False)
    return wMain, maxLev

  def __genArray(self, key, path, title, desc, schema, numItem, level, cls=None):
    w, lev = self.__generateInternal(0, path + ["#"], schema, level+1)
    if cls is None:
      if lev > level+3:
        wMain = widgets.JigArrayTab(key, title, desc)
      else:
        wMain = widgets.JigArrayFlat(key, title, desc)
    else:
      wMain = cls(key, title, desc)
    wMain.setItem(w)
    wMain.setNumberOfItems(numItem, update_view=False)  # view update will be performed by the top level "placeMeIn()"
    return wMain, lev
    
  def __genInteger(self, key, title, desc, defaultValue, required=False):      
    w = widgets.JigSpinBox(key, title, desc)
    w._mainW.setValue(defaultValue)
    return w
  
  def __genNumber(self, key, title, desc, defaultValue, required=False):
    w = widgets.JigLineEditNumber(key, title, desc)
    w.setText(defaultValue)
    return w

  def __genString(self, key, title, desc, defaultValue, required=False):      
    w = widgets.JigLineEditUnicode(key, title, desc)
    w.setText(defaultValue)
    return w

  def __genFilePath(self, key, title, desc, default, file_filt, required=False):      
    w = widgets.JigFileChooser(key, title, desc)
    w.setFilter(file_filt)
    w._mainW.setText(default)
    return w

  def __genCombo(self, key, title, desc, default, values, typ):
    idx = -1
    for i, v in enumerate(values):
      if not type(v) is typ:
        raise ValueError("Key '%s': enum must be of single type" % key)
      if default == v:
        idx = i
    if typ is int:       w = widgets.JigComboBoxInt(key, title, desc)
    elif typ is unicode: w = widgets.JigComboBoxUnicode(key, title, desc)
    w.populate(values)
    if idx != -1:  
      w._mainW.setCurrentIndex(idx)
    else:          
      raise ValueError("Invalid default value") 
    return w

