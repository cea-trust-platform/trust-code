#
# Author : A. Bruneton
#

from jsonschema import exceptions

class Validator(object):
  """ Encapsulation of the standard JSON validation process as offered by the 'jsonschema' module + application
  of the custom validation rules registered before the generation.
  Rules should be registered in the generator via registerCustomValidation().
  Use the getValidator() method from Generator to retrieve a functional validator.
  
  Main methods are setWidget() and validateInput().
  """
  def __init__(self, jss_validator):
    self._validator = jss_validator
    self._widget = None      # A JIG widget
    self._last_highlight = None
    self._custom_val = {}    # Custom validators
    self._ordered_val = []   # Ordered custom validators
    self._simple_to_schema = {}

  def setWidget(self, jig_widget):
    self._widget = jig_widget

  def _setSchemaMap(self, mp):
    self._simple_to_schema = mp

  def _registerCustomValidator(self, key, func, order=0):
    self._custom_val.setdefault(order, {}).setdefault(key, set()).add(func)

  def _orderCustomValidator(self):
    self._ordered_val = []
    ks = self._custom_val.keys()
    ks.sort()
    for k in ks:
      self._ordered_val.append(self._custom_val[k])

  def _customValidation(self, data):
    """ Apply the custom validation rules.
    TODO: this should be a generator too to allow for the same logic of iteration as for the pure JSON validation.
    """
    from collections import deque
    from copy import copy
    from generator import Generator
    self._orderCustomValidator()
    for dic in self._ordered_val:
      ret, path = self.__customValidationRec(dic, data, [Generator.SCHEMA_ROOT_KEY])
      if ret != "":
        # Convert a real data-set path containing array positions ( e.g. '/', 'toto', 4)
        # into a generic one ( '/', 'toto', '#' ) and save array positions (4 here).
        generic_path, array_pos = [], deque([])
        for e in path:
          if isinstance(e, int):
            array_pos.append(e)
            generic_path.append('#')
          else:
            generic_path.append(e)
        # Lookup corresponding schema paths (with 'oneOf' references):
        schema_paths = self._simple_to_schema.get(tuple(generic_path), [])
        # Put the proper array back in all schema paths and convert 'oneOf' references back to numbers
        schema_paths2 = []
        for p in schema_paths:
          new_schem_pth, arr_pos = [], copy(array_pos)
          for e in p:
#            try:
              if e == '#':       new_schem_pth.append(arr_pos.popleft())
              elif e[0] == "$":  new_schem_pth.append(int(e[1:]))
              else:              new_schem_pth.append(e)
#            except IndexError:
#              pass
          schema_paths2.append(new_schem_pth)
        return ret, schema_paths2
    return "", []
      
  def __customValidationRec(self, validation_dic, data, curr_path):
    # Recursion terminaison:
    key = curr_path[-1]
    v_set = validation_dic.get(key, None)
    if not v_set is None:
      for v in v_set:
        ret = v(data)
        if ret is None:
          raise Exception("Custom validation function %s registered for key '%s' returned 'None'!" % (v,key))
        if ret != "":
          return ret, curr_path
    # Recursion:
    if isinstance(data, dict):
      for k, v in data.items():
        ret, path = self.__customValidationRec(validation_dic, v, curr_path + [k])
        if ret != "": return ret, path
    elif isinstance(data, list):
      for i, l in enumerate(data):
        ret, path = self.__customValidationRec(validation_dic, l, curr_path + [i])
        if ret != "": return ret, path
    return "", []

  def validateInput(self, data=None, highlight=True):
    """ Validate the dataset. If an error is spotted the corresponding widget is highlighted in the GUI.
    Note that the highlight param might change the reported error. Indeed when the GUI is used we know
    which choice inside a "oneOf" has been retained, and we can then discard some irelevant errors.
    This is impossible to do in pure script mode.
    @param data the JSon data to validate. If not provided, the JSon extracted from the GUI will be used.
    @param highlight if True the corresponding faulty widget is highlighted in the GUI.
    @return error message if any error, void string otherwise.  
    @return the path of the faulty entry. Void list otherwise.
    """
    
    if highlight and not self._last_highlight is None:
      self._widget.setHighlightPath(self._last_highlight, False)
      self._last_highlight = None
    if data is None:
      data = self._widget.getJSon() 
    ##
    ## Custom validation first:
    ##
    ret, schema_paths = self._customValidation(data)
    schema_path = []
    if ret != "":
      schema_path = schema_paths[0]
      if highlight:
        # In GUI mode, we can see if the widget is active (useful in "oneOf" constructs where user has explicitly
        # chosen an option).
        for p in schema_paths:
          schema_path = p
          w = self._widget.resolvePath(schema_path)
          if not w is None and w.isActive():
            # Check that all parents are active too!
            allActive = True
            for i in range(1, len(schema_path)):
              parentW = self._widget.resolvePath(schema_path[:i])
              if not parentW.isActive():
                allActive = False
                break
            if not allActive: continue
            self._last_highlight = w.getPath()
            self._widget.setHighlightPath(self._last_highlight, True)
            break
    ##
    ## JSon validation:
    ##    
    if ret == "":
      if not self._validator.is_valid(data):
        # Iterate by relevance until we find an enabled widget: this is esp. useful for "oneOf" configurations
        # where the best_match is sometimes a sub-schema which hasn't been clicked by the user.
        errors = self._validator.iter_errors(data)
        for err in self.__best_match_iterator(errors):
          schema_path, ret = list(err.absolute_schema_path), err.message
#          print "@@@@"
#          print err.absolute_schema_path
#          print err.absolute_path 
#          print "@@@@"
          if not highlight:
            # Stop at first (best match) error in pure script mode.
            break
          # Same as before, see if the widget is active
          w = self._resolvePathInWidget(schema_path, err.absolute_path)
          if not w is None and w.isActive():
            self._last_highlight = w.getPath()
            self._widget.setHighlightPath(self._last_highlight, True)
            break
          schema_path = []
    return ret, schema_path

  def __best_match_iterator(self, errors, key=exceptions.relevance):
    """ Freely inspired from the initial exceptions.best_match() function, but allowing to iterate on all  
      the (sorted) errors, instead of just retrieving one.
    """
    # ** Remember the impl. of best_match(): **
    #
    #    def best_match(errors, key=relevance):
    #      errors = iter(errors)
    #      best = next(errors, None)
    #      if best is None:
    #          return
    #      best = max(itertools.chain([best], errors), key=key)
    #  
    #      while best.context:
    #          best = min(best.context, key=key)
    #      return best      
    #
    errors = sorted(errors, key=key, reverse=True)  # from max to min, mimick the initial "best = max..."
    return self.__context_traversal_generator(errors, key)    

  def __context_traversal_generator(self, sorted_err, key):
    for e in sorted_err:
      if e.context is None or not len(e.context):
        yield e
        continue
      sorted_context = sorted(e.context, key=key)  # from min to max
      for c in self.__context_traversal_generator(sorted_context, key):
        yield c
      yield e
    raise StopIteration

  def _resolvePathInWidget(self, schema_path, data_path):
    """ Retrieve child widget from an error path/data path. This is not the same as the resolvePath() in JigWidget
    since here we parse a path returned by the 'jsonschema' module.
    Strong choice: any part of the path not understood is ... discarded! Indeed the schema_path is very
    precise and indicates for example which element of the meta-schema was violated ("minItems", etc ...)  
    @return None if nothing is understood, the sub-widget otherwise
    """
    final_path = []
    from collections import deque
    # Scan data_path to store all array indices (those are the only int in the path):
    array_pos = deque([])
    for p in data_path:
      if isinstance(p, int):
        array_pos.append(p)
        
    w, once = self._widget, False
    for p in schema_path:
      if p == "items":
        p = array_pos.popleft()
      w2 = w.getChild(p)
      if w2 is None:
        pass
      else:
        final_path.append(p)
        once = True
        w = w2
#    print "FINAL", final_path, w, w.getPath()
    if once:    return w
    else:       return None
