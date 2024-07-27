#!/usr/bin/env python
# -*- coding: UTF-8 -*-

""" Main module for the set of classes representing the TRUST data model.
This module is a bit long, but its logic can be well understood by reading first
  - class Base_common
  - and then its most important child Dataset, representing a full TRUST dataset
  
Authors: A Bruneton, C Van Wambeke, G Sutra
"""

import pprint as PP

from xyzpy.baseXyz import *
from xyzpy.intFloatListXyz import StrXyz, StrVerbatimXyz, StrNoBlankXyz, StrInListXyz, \
                                  IntXyz, IntRangeXyz, IntInListXyz, \
                                  FloatXyz, FloatRangeXyz, \
                                  TodoXyz, BoolXyz
import xyzpy.classFactoryXyz as CLFX
import trustpy.trust_utilities as TRUU
from trustpy.trust_parser import TRUSTTokens

########################################################
# Utilities TRUST methods
########################################################

def check_append_list(classname, list1, list2, suppressed):
  """
  list1 append _attributesList list2 with no duplicates and error message
  """
  res, done = [], []
  for att_name, att_type in list1:
    done.append(att_name)
    res.append((att_name, att_type))
  for att_name, att_type in list2:  # list2 from parent class
    if att_name in suppressed:
      continue # skipped
    if att_name not in done:
      done.append(att_name)
      res.append((att_name, att_type))
    else:
      # duplicate att_name - simply ignore
      pass
  return res

def check_append_dict(classname, dict1, dict2):
  """
  dict1 append _attributesSynonyms dict2 with no duplicates and error message
  """
  res = {}
  suppress_param = []
  for att_name, v in dict1.items():
    res[att_name] = v
  for att_name, v in dict2.items():   # dict2 from class mere
    if att_name not in res:
      res[att_name] = v
    else:
      # duplicate att_name - simply ignore
      pass
  return res

########################################################
# Customized classes that will sit at the top of the inheritance
# tree for all the other generated classes.
########################################################

class Base_common(object):
  """ Abstract base class containing all the methods relevant to TRUST data model
  logic.
  By far the two most important methods are:
    ReadFromTokens() - which instanciate a class from a stream of tokens (i.e. from the .data file)
    toDatasetTokens() - which does the inverse operation and serializes the content of the instance as a stream of tokens
  """
  _braces = 1         # whether we expect braces when reading the class - see doc_TRAD2 - by default, expect braces.
  _read_type = False  # whether to read the actual type before instanciating the class (typically for *_base or *_deriv classes) - By default we do *not* read the type
                      # In the Python code generation this attribute is overloaded when hitting _base or _deriv.
  _synonyms = []   # a list of synonyms for the name of implemented class
  _infoMain = []   # a tuple giving the source file and the line # where the type was defined
  _infoAttr = {}   # same thing for class attributes (dictionnary indexed by main attr name)
  _attributesSynonyms = {}   # synonyms for the current class (read_med -> lire_med)
  _optionals = set([])  # set of optional attibutes for a keyword
  _plainType = False    # whether the class is a basic type (like an int, etc ...) -> used for
                        # correctly displaying debug information of the parent attribute

  def __init__(self):
    self._tokens = {}     # a dictionnary giving for the current instance the tokens corresponding to each bit - see override
                          # in ConstrainBase for a more comprehensive explanation.

  def toTrustData(self):
    """ Output current data (contained in self)"""
    return ''.join(self.toDatasetTokens(self))

  @classmethod
  def GenErr(cls, stream, msg, attr=None):
    """ Useful method to generate nice error message when an exception is raised. 
    It provides the file name and line number of:
      - the faulty dataset (.data file)
      - the point in the source code (or in the TRAD2.org) where the concerned grammar element
      was defined.
    """
    err = "\n" + TRUU.RED + msg + TRUU.END + "\n"
    ctx = TRUU.YELLOW
    ctx +=  "=> This error was triggered in the following context:\n"
    ctx += "   Dataset: line %d  in file  '%s'\n" % (stream.currentLine(), stream.fileName())

    s = ""
    if len(cls._infoMain) > 0:  # automatic classes (like base types int, float...) do not have debug info
      s =  "   Model:   line %d  in file  '%s'\n" % (cls._infoMain[1], cls._infoMain[0])
    if not attr is None:
      fnam, lineno = cls._infoAttr[attr]
      s = f"   Model:   line {lineno}  in file  '{fnam}'\n"
    ctx += s + TRUU.END
    return err + ctx

  def _checkToken(self, key, expec, typ=str):
    """ Used in toDatasetTokens() method: consider the following:
      - a dataset has been loaded in Python: while doing so the original tokens are stored in self.tokens
      - a value is modified in the data (for ex: "ds.get("sch").tmax = 23") -> at this point the underyling value in self.token
      still contains what was initially read!
     So this method checks whether the value stored in the token (as a string) is consistent with the actual value of the attribute.
    """
    lst_low = self._tokens.get(key, TRUSTTokens()).low()
    s = ''.join(lst_low).strip()
    try:  # conversion function 'typ' might fail, if for example trying to convert 'qdfs' into int:
      ret = typ(s) == typ(expec.strip())
    except:
      ret = False
    return ret

  def _getClsTokens(self):
    """ Return output tokens for the class name (i.e. the TRUST keyword itself)
    """
    if self._read_type:
      cls = self.__class__
      expec = [cls.__name__] + cls._synonyms
      for e in expec:
        v = no_low(e)
        if self._checkToken("cls_nam", v):
          return self._tokens["cls_nam"].orig()
      return [" " + no_tru_low(cls.__name__)]
    return []

  def _getBraceTokens(self, brace):
    """ Get output tokens for opening/closing brace """
    if self._checkToken(brace, brace):
      return self._tokens[brace].orig()
    else:
      if brace == "{":    return ["{\n"]
      elif brace == "}":  return ["\n}\n"]
      raise Exception(f"Internal error -  unexpected parameter: '{brace}'")


  def toDatasetTokens(self):
    """ Produce a list of tokens representing the object in the format expected in a TRUST dataset.
    Note: this tries as much as possible to reproduce what was initially loaded, if the 
    initial parsing (via ReadFromTokens()) was successful.
    In any case, it is the responsibility of this method to check that output data 
    is coherent with the real value stored in the object.
    See also _checkToken() in the same class.
    """
    raise NotImplementedError

  @classmethod
  def IsOptional(cls, attr_nam):
    """ Returns True if an attribute is optional """
    return attr_nam in cls._optionals

  @classmethod
  def ConsumeBrace(cls, stream, brace):
    """ Consume the tokens in stream until a opening or closing brace is found. 'brace' is either '{' or '}' """
    nams = cls.GetAllTrustNames()
    brac_nam = {'{':"opening", '}': "closing"}[brace]
    tok = stream.probeNextLow()
    if tok != brace:
      err = cls.GenErr(stream, f"Keyword '{nams}' expected a {brac_nam} brace '{brace}'")
      raise ValueError(err)
    stream.validateNext()

  @classmethod
  def ReadFromTokens(cls, stream):
    """
    Consumes a given number of tokens in 'stream' (a trust_parser.TRUSTStream object) and fills the various class members from it.
    Should be overriden in derived class. At the end of the read, the current position of the stream is advanced up to the last
    reading point.
    @return a new instance of the same class correctly initialized with the values parsed
    """
    inh = cls().getInheritage()
    msg = "Internal error - ReadFromTokens() not implemented in class '%s'" % cls.__name__
    msg += "\n  (=> Inheritage of this class is " + str(inh) + ")"
    raise Exception(msg)

  @classmethod
  def WithBraces(cls):
    """
    Get brace style for the keyword when reading it from the data file (with braces or without). What a mess... :
    -1: like the parent class
    0: no braces, no attribute names
    1: with braces, attributes explicitly named
    -2: like 0, but should come after 'discretize' (not checked here)
    -3: like 1, but should come after 'discretize' (not checked here)
    """
    mp = {0: False, 1: True, -2: False, -3: True}
    if cls._braces in [0, 1, -2, -3]:
      return mp[cls._braces]
    # Need to look at the parent
    for c in cls.__bases__:
      if c.__name__.endswith(_TRIOU_SUFFIX):
        return c.WithBraces()
    raise Exception("Internal error")

  @classmethod
  def _Dbg(cls, msg):
    """ Handy debug printing """
    # The 'inspect' module is very costly - so skip if not in highest logging level:
    if TRUU._log_level <= 3:
      return
    import inspect
    curframe = inspect.currentframe()
    calframe = inspect.getouterframes(curframe, 2)
    cal_nam = calframe[1][3]
    def pad(s):
      return s + " "*(max(55-len(s), 0))
    s = f"[{cls.__name__}.{cal_nam}] --"
    m = msg.replace("@FUNC@", s)
    TRUU.log_debug(m)

###########################
class ConstrainBase(Base_common):
  """
  Class representing any type/keyword for which the attribute types are to be checked.
  """
  _rootNamesForSyno = {} # See method _ReadClassName and trust_hacks.py

  def __init__(self):
    super(ConstrainBase, self).__init__()
    opbr  = TRUSTTokens(low=["{"], orig=[" {\n"])
    clobr = TRUSTTokens(low=["}"], orig=[" \n}\n"])
    clsnam = TRUSTTokens(low=[], orig=[" " + self.__class__.__name__[:-_LEN_TRIOU_SUFFIX]])
    self._tokens = {"{": opbr,           # For a ConstrainBase we might need opening and closing brace. By default those
                    "}": clobr,          # are simple '{' and '}' followed by a line return. If ReadFromTokens() was
                    "cls_nam": clsnam }  # invoked to build the object this will respect the initial input (with potentially more spaces)
    self._attrInOrder = []               # Just to save the order in which the attributes were read ...

  def __setattr__(self, name, value):
    """ 'lambda' is a reserved Python keyword ... and is everywhere a medium attribute """
    if name == "lambda":
      name = "Lambda"
    _XyzConstrainBase.__setattr__(self, name, value)

  def isHidden(self, nameAttr):
    """
    to know if attribute is currently displayed in treeView and other dialog widget
    avoid display aco '{' and acof '}' in GUI as two immutables useless leaves
    """
    if nameAttr in ["aco", "acof"]:
      return True  # hidden
    else:
      return False

  @classmethod
  def _GetBaseClassName(cls, cls_nam):
    """ Strip '<inherited>' from class name """
    # TODO to put in baseXyz??
    tag = '<inherited>'
    if cls_nam.startswith(tag):
      return cls_nam[len(tag):]
    return cls_nam

  @classmethod
  def _InvertSyno(cls):
    """ 
    Invert all attributes synonyms - returns a dict giving for each synonym (incl the attribute itself)
    the real underlying attribute name and the corresponding class name
    """
    invert_syno = {}
    for nam, attr_cls_name in cls._attributesList:
      invert_syno[nam] = (nam, attr_cls_name)
      # 'Lambda' is the only attribute which won't be full lower case (because 'lambda' is not
      # allowed as a Python attribute in a class). So register "lambda" as a syno for "Lambda".
      if nam == "Lambda":
        invert_syno["lambda"] = (nam, attr_cls_name)
      if nam in cls._attributesSynonyms:
        for syno in cls._attributesSynonyms[nam]:
          if syno.strip() != "":
            invert_syno[syno] = (nam, attr_cls_name)
    return invert_syno

  @classmethod
  def _ReadFromTokens_braces(cls, stream):
    """ Read from a stream of tokens using the key/value syntax of TRUST keyword, i.e. keywords
    using opening and closing braces.
    """
    ret = cls()
    nams = cls.GetAllTrustNames()
    invert_syno = cls._InvertSyno()

    # Parse opening brace
    cls.ConsumeBrace(stream, "{")
    ret._tokens["{"] = stream.lastReadTokens()

    # Identify mandatory attributes:
    attr_ok = {}
    for attr_nam, _ in cls._attributesList:
      if not cls.IsOptional(attr_nam):
        attr_ok[attr_nam] = False  # False="was not read yet"
    tok = stream.probeNextLow()
    while tok != "}":
      stream.validateNext()
      if tok in invert_syno:
        attr_nam, attr_cls_nam = invert_syno[tok]
        ret._tokens[attr_nam] = stream.lastReadTokens()
        ret._attrInOrder.append(attr_nam)
        cls._Dbg(f"@FUNC@ attr_nam '{attr_nam}' attr_cls_nam '{attr_cls_nam}'")
        # Retrieve attribute's class (as given by the data file), and instanciate it
        nam = cls._GetBaseClassName(attr_cls_nam)
        attr_cls = CLFX.getXyzClassFromName(nam)
        assert not attr_cls is None  # Should never happen...
        # Temporarily load debug instruction from current class on the child attribute type.
        # This is useful for base types which are actually not defined anywhere in the
        # TRAD2 (like ints, floats, strings, etc...) and thus we can for example display an error message
        # saying that '3.5' is not an int, and provide the actual TRAD2 entry for it.
        if attr_cls._plainType: attr_cls._infoMain = cls._infoAttr[attr_nam]
        # Parse child attribute:
        val = attr_cls.ReadFromTokens(stream)
        # Potentially reset debug info:
        if attr_cls._plainType: attr_cls._infoMain = []
        cls._Dbg(f"@FUNC@ setting attribute '{attr_nam}' with val '%s'" % str(val))
        ret.__setattr__(attr_nam, val)
        if attr_nam in attr_ok:
          attr_ok[attr_nam] = True
      else:
        err = cls.GenErr(stream, f"Unexpected attribute '{tok}' in keyword '{nams}'")
        raise ValueError(err) from None
      tok = stream.probeNextLow()
    # Have we parsed all mandatory attributes?
    for k, v in attr_ok.items():
      if not v:
        err = cls.GenErr(stream, f"Attribute '{k}' is mandatory for keyword '{nams}' and was not read", attr=k)
        raise ValueError(err) from None

    # Parse closing brace
    cls.ConsumeBrace(stream, "}")
    ret._tokens["}"] = stream.lastReadTokens()

    return ret  # parsing was successful

  @classmethod
  def _ReadFromTokens_no_braces(cls, stream):
    """ Read from a stream of tokens, in the order dictated by cls._attributeList 
    Used for TRUST keywords which are **not** expecting opening/closing braces. This is **painful**.
    """
    ret = cls()
    invert_syno, ca, nams = cls._InvertSyno(), cls._attributesList, cls.GetAllTrustNames()
    attr_idx = 0
    while attr_idx < len(ca):
      # On a given token, try as much as possible to match it against the attribute list:
      #   - try all consecutive optional attr first
      #   - otherwise, if mandatory attribute, should match directly
      # If this fails, the token stream is ill formed, so raise.
      opt_match = False
      while attr_idx < len(ca) and cls.IsOptional(ca[attr_idx][0]):
        cur_attr, cur_cls_nam = ca[attr_idx]
        cur_cls_nam = cls._GetBaseClassName(cur_cls_nam)
        attr_cls = CLFX.getXyzClassFromName(cur_cls_nam)
        cls._Dbg(f"@FUNC@ cur_attr '{cur_attr}' cur_cls_nam '{cur_cls_nam}'")
        stream.save("BEFORE_OPT")
        try:
          val = attr_cls.ReadFromTokens(stream)
          # OK, optional match! Set the optional attribute:
          opt_match = True
          cls._Dbg(f"@FUNC@ setting optional attr {cur_attr}")
          ret.__setattr__(cur_attr, val)
          attr_idx += 1
          break
        except Exception as e:
          # Could not set optional attribute - no problem, it was ... optional :-) Just reset stream.
          cls._Dbg(f"@FUNC@ optional attr '{cur_attr}' failed")
          stream.restore("BEFORE_OPT")
        attr_idx += 1
      if attr_idx < len(ca) and not opt_match:
        # No optional match - must match current mandatory attr
        cur_attr, cur_cls_nam = ca[attr_idx]
        cls._Dbg(f"@FUNC@ mandatory attr -> cur_attr '{cur_attr}' cur_cls_nam '{cur_cls_nam}'")
        attr_cls = CLFX.getXyzClassFromName(cur_cls_nam)
        # See explanation in ReadFromTokens_withBrace for this:
        if attr_cls._plainType: attr_cls._infoMain = cls._infoAttr[cur_attr]
        # Parse child attribute:
        val = attr_cls.ReadFromTokens(stream)
        # Potentially reset debug info:
        if attr_cls._plainType: attr_cls._infoMain = []
        cls._Dbg(f"@FUNC@ setting *mandatory* attr '{cur_attr}' with val '{val}'")
        ret.__setattr__(cur_attr, val)  # Mandatory attr OK, set it.
        attr_idx += 1

    # Have we parsed everything?
    if attr_idx < len(ca):
      # Only optional attr should remain:
      for att_nam, _ in ca[attr_idx:]:
        if not cls.IsOptional(att_nam):
          err = cls.GenErr(stream, f"Keyword '{nams}' - mandatory attribute '{att_nam}' missing or ill-formed", attr=att_nam)
          raise ValueError(err)
    # At the end return real_j since j might have been advanced in vain trying to match optional attributes:
    return ret

  @classmethod
  def _ReadClassName(cls, stream):
    """ Read the class name. 
    This method is invoked by all the keywords where the actual class name should be read (_read_type = True). For example all keyword accepting as an
    attribute a field_base, might recieve champ_uniforme, or a champ_fonc_med, etc.. So type must actually be read.
    Overriden in trust_hacks.py for convection/diffusion 'negligeable' for example. 
    """
    # Parse one token to identify real class being read:
    kw = BaseChaine.ReadFromTokens(stream)
    cls_nam = kw + _TRIOU_SUFFIX
    if not CLFX.isValidClassName(cls_nam):
      err = cls.GenErr(stream, f"Invalid TRUST keyword '{kw}'")
      raise ValueError(err)
    ###
    ###   HACK HACK
    ###
    # From here, we might have a lot of fun :-) Some synonyms are duplicated.
    # For example 'moyenne' might be a 'stat_post_moyenne' or a 'champ_post_statistiques_base' ... we need context ...
    # So give a preference to the class inheriting the current 'cls' (which is given by the TRAD2)
    root_names = cls._rootNamesForSyno.get(cls_nam, [])
    for r in root_names + [cls_nam]:
      a_cls = CLFX.getXyzClassFromName(r)
      if a_cls is None:
        raise Exception(f"internal error - getXyzClassFromName class name '%s' not found" % r)

      if issubclass(a_cls, cls):
        return a_cls.__name__
    cls._Dbg("@FUNC@ <== ERROR ")
    # If we arrive here we are in fact most likely to hit a point where a previous (non-brace) keyword
    # was missing an attribute, and we treat the next token (for example '}') as a class to be read ...
    err = cls.GenErr(stream, f"Invalid TRUST keyword: '{kw}' (or maybe, did you forget an attribute on the previous line?)")
    raise ValueError(err)

  @classmethod
  def ReadFromTokens(cls, stream):
    """ Override. See Base_common.  """
    if cls._read_type:
      cls_nam = cls._ReadClassName(stream)
      cls._Dbg(f"@FUNC@ parsed type: '{cls_nam}'")
      tok = stream.lastReadTokens()
      ze_cls = CLFX.getXyzClassFromName(cls_nam)
      assert ze_cls is not None   # already checked in _ReadClassName()
    else:
      ze_cls = cls
      cls._Dbg(f"@FUNC@ NOT parsing type (forced type: '{cls.__name__}')")
    # The two reading modes are really different:
    if ze_cls.WithBraces():
      # Here key/value pairs, order doesn't matter
      cls._Dbg(f"@FUNC@ reading with braces")
      val = ze_cls._ReadFromTokens_braces(stream)
    else:
      # Here order matters a lot
      cls._Dbg(f"@FUNC@ reading WITHOUT braces (class '{ze_cls.__name__}')")
      val = ze_cls._ReadFromTokens_no_braces(stream)
    if cls._read_type:
      val._tokens["cls_nam"] = tok
    if 'cls_nam' in val._tokens:
      cls._Dbg(f"@FUNC@ tokens are: {val._tokens['cls_nam'].orig()}")
    return val

  def toDatasetTokens(self):
    """ Override. """
    if self.WithBraces():
      return self.toDatasetTokens_braces()
    else:
      return self.toDatasetTokens_no_braces()

  def toDatasetTokens_braces(self):
    s = []
    # class name
    s.extend(self._getClsTokens())
    # Opening brace
    s.extend(self._getBraceTokens('{'))

    # Check whether some other (new) attributes were added - if so put, them at the end:
    extra_attr = []
    for attr_nam, _ in self._attributesList:
      try:
        self.__getattribute__(attr_nam)
        if attr_nam not in self._attrInOrder:
          extra_attr.append(attr_nam)
      except:
        pass

    for attr_nam in self._attrInOrder + extra_attr:
      a = None
      try:
        a = self.__getattribute__(attr_nam)
      except:
        # Attribute was deleted in this instance:
        continue
      # When braces are there, we need the attribute name explicitly.
      all_n = [attr_nam] + self._attributesSynonyms.get(attr_nam,[])
      # Check whether there is a registered token for the actual attribute name
      #  (we don't need to check the validity of the attribute token since this can't be changed - only value can be changed)
      # Otherwise build a new string for this (newly added) attribute:
      if attr_nam in self._tokens:  tok = self._tokens[attr_nam].orig()
      else:                         tok = ["\n" + attr_nam + " "]
      # WARNING: special case - flags need to be completely removed if False (we don't even want the attribute name .. which is the flag)
      if a.__class__.__name__ != "rien" or bool(a):
        s.extend(tok)
        s.extend(a.toDatasetTokens())  # append attribute value itself
    # Closing brace
    br = self._getBraceTokens('}')
    s.extend(br)
    return s

  def toDatasetTokens_no_braces(self):
    s = []
    # class name
    s.extend(self._getClsTokens())
    for attr_nam, _ in self._attributesList:
      try:
        a = self.__getattribute__(attr_nam)  # might raise if attribute not set
        s.extend(a.toDatasetTokens())  # append attribute value itself - check will be done there
      except: # attribute is simply not set - skip
        pass
    return s

  def logHelp(self, value=None):
    """print help() with logger info"""
    res = self.helpToStr(value=value)
    TRUU.log_info(res)

######################################################
## List-like types
######################################################
class ListOfBase(ListOfBaseXyz, Base_common):
  _plainType = True
  _allowedClasses = []
  _comma = 1   #  1: with comma to separate items, 0: without, -1: like parent

  def __init__(self, values=[]):
    ListOfBaseXyz.__init__(self)
    Base_common.__init__(self)
    self._tokens = {}  # Expected keys: "nb_items"
    self._ze_cls = None  # Single authorized class (only mono-type lists are supported)
    cls = self.__class__
    if len(cls._allowedClasses) == 1:
      # Try auto-cast - this helps for example to initialize ListOfFloat without having to write
      # l = ListOfFloat([BaseFloattant(35), BaseFloattant(48.2)])
      # One can do: l = ListOfFloat([35, 48.2])
      self._ze_cls = CLFX.getXyzClassFromName(cls._allowedClasses[0])
      newv = list(map(self._ze_cls, values))
    else:
      raise Exception("Implementation error: multi-type list can not be parsed yet ...")
    for val in newv:
      self.append(val)

  def __setitem__(self, idx, val):
    """ Override. Helps doing things like:
        a_list = ListOfFloat()
        ... append ... append ..
        a_list[32] = 26.5  # avoids a_list[32] = BaseFloattant(26.5)
    """
    if not self._ze_cls is None:
      # Try auto-cast - this helps assigning values
      newv = self._ze_cls(val)
    else:
      newv = val
    ListOfBaseXyz.__setitem__(self, idx, newv)

  @classmethod
  def MustReadSize(cls):
    """ Whether we should read the list size.
    At present, the rule is simple: when there are no braces, we must read a size.
    """
    return not cls.WithBraces()

  @classmethod
  def WithComma(cls):
    """
    Tell whether list items are separated by a comma.
    -1: like the parent class
    0: no comma to separate items
    1: with comma
    """
    mp = {0: False, 1: True}
    if cls._comma in [0, 1]:
      return mp[cls._comma]
    # Need to look at the parent
    for c in cls.__bases__:
      if c.__name__.endswith(_TRIOU_SUFFIX):
        return c.WithComma()
    raise Exception("Internal error")

  @classmethod
  def ConsumeComma(cls, stream):
    """ Consume a single comma ',' for list. """
    nams = cls.GetAllTrustNames()
    tok = stream.probeNextLow()
    if tok != ',':
      err = cls.GenErr(stream, f"Keyword '{nams}' expected a comma (',') to separate list items")
      raise ValueError(err)
    stream.validateNext()

  @classmethod
  def _ReadListSize(cls, stream):
    """ Return size of the list - overriden in ListOfFloatDim """
    t = stream.probeNextLow()
    try:
      n = int(t)
    except:
      err = cls.GenErr(stream, f"Invalid number of elements in list: '{t}', expected an integer")
      raise ValueError(err)
    stream.validateNext()
    if n < 0:
      err = cls.GenErr(stream, f"Invalid number of elements in list: '{t}', expected a positive integer")
      raise ValueError(err)
    return n

  @classmethod
  def ReadFromTokens(cls, stream):
    """
    Override. Read N objects from the input. N (an integer) must be the first token found.
    This override only works for single-type list.
    """
    if len(cls._allowedClasses) > 1:
      raise Exception("Implementation error: multi-type list can not be parsed yet ...")

    ret = cls()

    if cls.MustReadSize():
      cls._Dbg(f"@FUNC@ reading list size ...")
      n = cls._ReadListSize(stream)
      ret._tokens["len"] = stream.lastReadTokens()
    elif not cls.WithBraces():
      err = cls.GenErr(stream, f"Internal error: Can not read list with no brace and no size info.")
      raise Exception(err)

    # If allowed class is a base type with no debug information, take the one from current
    # (list) class (which itself might have been set in ReadFromTokens_braces / no_braces)
    if ret._ze_cls._plainType:    ret._ze_cls._infoMain = cls._infoMain

    if cls.WithBraces():
      cls._Dbg("@FUNC@ reading list with braces ...")
      cls.ConsumeBrace(stream, "{")
      ret._tokens["{"] = stream.lastReadTokens()
      tok = stream.probeNextLow()
      i = 0
      while tok != '}':
        val = ret._ze_cls.ReadFromTokens(stream)
        ret.append(val)
        tok = stream.probeNextLow()
        if cls.WithComma() and tok != '}':
          cls.ConsumeComma(stream)
          # yes, the ',' token might be different after each element (comments etc...):
          ret._tokens[",%d" % i] = stream.lastReadTokens()
          tok = stream.probeNextLow()
        i += 1
      cls.ConsumeBrace(stream, "}")
      ret._tokens["}"] = stream.lastReadTokens()
    else:  # no braces but we should know somehow how many objects to be read:
      assert n >= 0
      cls._Dbg(f"@FUNC@ reading list **without** braces (exp size is {n}) ...")
      for cnt in range(n):
        val = ret._ze_cls.ReadFromTokens(stream)
        ret.append(val)

    # Reset debug info:
    if ret._ze_cls._plainType: ret._ze_cls._infoMain = []

    return ret

  def _extendWithSize(self, ret):
    """ Append the size of the list to the list of output tokens. 
    Overriden in ListOfXXXXDim classes where the size is fixed.
    """
    len_as_str = str(len(self))
    if self._checkToken("len", len_as_str):
      ln = self._tokens["len"].orig()
    else:
      ln = [" " + len_as_str]
    ret.extend(ln)

  def toDatasetTokens(self):
    """ Override. See doc in mother class. """
    ret = []
    dft_com = TRUSTTokens(orig=[" ,\n"])
    if self.MustReadSize():  # if size must be read, it must be written out too ...
      self._extendWithSize(ret)
    if self.WithBraces():
      br = self._getBraceTokens("{")
      ret.extend(br)
    for i, v in enumerate(self):
      ret.extend(v.toDatasetTokens())
      if self.WithComma() and i != len(self)-1:
        # yes, the ',' token might be different after each element (comments etc...):
        com = self._tokens.get(",%d" % i, dft_com)
        ret.extend(com.orig())
    if self.WithBraces():
      br = self._getBraceTokens("}")
      ret.extend(br)
    return ret

class AbstractSizeIsDim(object):
  """ Shared methods for all ListOfXXXDim classes - implements the necessary stuff to force
  the size of the list to be the dimension read at the top of the dataset.
  """
  _DIMENSION = -1  # Set when the keyword 'dimension' is encountered

  @classmethod
  def _ReadListSize(cls, stream):
    """ Size is fixed and does not need to be provided """
    if cls._DIMENSION not in [2, 3]:
      raise ValueError("Invalid dataset: It seems the dimension has not been defined")
    return cls._DIMENSION

  def _extendWithSize(self, ret):
    """ Override. Do nothing. Here the size is the dimension and does not need to be output """
    pass

class ListOfChaine(ListOfBase):
  """
  List of strings (with no blanks), in the form 'N val1 val2 ...'
  """
  _allowedClasses = ["BaseChaine"]
  _braces = 0  # No braces for this simple list
  _comma = 0

  def __init__(self, values=[]):
    ListOfBase.__init__(self, values)

class ListOfChaineDim(ListOfChaine, AbstractSizeIsDim):
  """
  List of strings, in the form 'val1 val2 ...'
  The number of expected strings is given by the dimension of the problem.
  """
  def __init__(self, values=[]):
    ListOfChaine.__init__(self, values)

  _ReadListSize = AbstractSizeIsDim._ReadListSize
  _extendWithSize = AbstractSizeIsDim._extendWithSize

class ListOfFloat(ListOfBase):
  """
  List of floats, in the form 'N val1 val2 ...'
  """
  _allowedClasses = ["BaseFloattant"]
  _braces = 0  # No braces for this simple list
  _comma = 0

  def __init__(self, values=[]):
    ListOfBase.__init__(self, values)

class ListOfFloatDim(ListOfFloat, AbstractSizeIsDim):
  """
  List of floats, in the form 'val1 val2 ...'
  The number of expected floats is given by the dimension of the problem.
  """
  def __init__(self, values=[]):
    ListOfFloat.__init__(self, values)

  _ReadListSize = AbstractSizeIsDim._ReadListSize
  _extendWithSize = AbstractSizeIsDim._extendWithSize

class ListOfInt(ListOfBase):
  """
  List of ints, in the form 'N val1 val2 ...'
  """
  _allowedClasses = ["BaseEntier"]
  _braces = 0  # No braces for this simple list
  _comma = 0

  def __init__(self, values=[]):
    ListOfBase.__init__(self, values)

class ListOfIntDim(ListOfInt, AbstractSizeIsDim):
  """
  List of ints, in the form 'N val1 val2 ...'
  The number of expected ints is given by the dimension of the problem.
  """
  def __init__(self, values=[]):
    ListOfInt.__init__(self, values)

  _ReadListSize = AbstractSizeIsDim._ReadListSize
  _extendWithSize = AbstractSizeIsDim._extendWithSize


######################################################
# Class interprete
######################################################
class interprete(ConstrainBase):
  """ Class 'interprete' has nothing special in itself, except that it needs
  to be known early, so that the Dataset class can test whether a given class
  is a child of 'interprete'
  So force its definition here, and hence avoid its automatic generation from the
  TRAD2 file.
  """
  _braces = 0   # By default, 'interprete' does not need braces


######################################################
# Main class representing the dataset and forward declarations
######################################################
class Declaration(ConstrainBase):
  """ Class representing a simple forward declaration in the dataset like
  'Pb_conduction pb'.
  Note: this class does not need to override ReadFromTokens() - the parsing is so simple
  (two strings) that everything is done directly in DataSet.ReadFromTokens()
  """
  _attributesList = [ ('cls_nam',    'BaseChaine'),
                      ('identifier', 'BaseChaine')
                    ]
  _read_type = True     # Always since the type ... is the main information here!

  def __init__(self, cls_nam="", identifier=""):
    super(Declaration, self).__init__()
    self.cls_nam = cls_nam
    self.identifier = identifier
    self._cls_obj = None
    self._tokens = {}  # expected keys: 'cls_nam' and 'identifier'

  @classmethod
  def ReadFromTokens(cls, stream):
    ret = cls()
    kw = BaseChaine.ReadFromTokens(stream)
    ret.cls_nam = kw + _TRIOU_SUFFIX
    # Check whether the class is valid:
    ze_cls = CLFX.getXyzClassFromName(ret.cls_nam)
    if ze_cls is None:
      err = cls.GenErr(stream, f"Keyword '{kw}' is not a valid TRUST keyword")
      raise ValueError(err)
    ret._cls_obj = ze_cls
    ret._tokens["cls_nam"] = stream.lastReadTokens()
    ident = BaseChaine.ReadFromTokens(stream)
    # Basic checks on the identifier:
    if ident.startswith("{") or ident.startswith("}") or ident[0].isnumeric():
      err = cls.GenErr(stream, f"Invalid identifier '{ident}' in forward declaration of type '{kw}'")
      raise ValueError(err)
    ret.identifier = ident
    ret._tokens["identifier"] = stream.lastReadTokens()
    return ret

  def toDatasetTokens(self):
    """ Override - see Base_common """
    s = []
    if self._checkToken("cls_nam", self.cls_nam[:-_LEN_TRIOU_SUFFIX]):
      cn = self._tokens["cls_nam"].orig()
    else:
      cn = [" " + self.cls_nam]
    s.extend(cn)
    if self._checkToken("identifier", self.identifier):
      tok_id = self._tokens["identifier"].orig()
    else:
      tok_id = [" " + self.identifier]
    s.extend(tok_id)
    return s

######################################################
class DataSet(ListOfBase):
  """ Class representing a complete TRUST dataset.
  A TRUST dataset is a list of Base_common, with allowed classes
    - Declaration, representing forward declarations like 'Pb_conduction pb'
    - or any child of ConstrainBase, representing all TRUST interprets like 'lire_med { file ... }'
  """
  _allowedClasses = ["<inherited>ConstrainBase"]

  def __init__(self):
    ListOfBase.__init__(self)
    self._declarations = {}   # Map of declarations like "Pb_conduction pb". Key is identifier ('pb')
                              # Value is the (class_name, idx) where idx is the position in the dataset (which is essentially a list, remember)

  def get(self, identifier):
    """ User method - Returns the object associated with a name in the data set """
    if not identifier in self._declarations:
      raise ValueError(f"Invalid identifer '{identifier}'")
    it_num = self._declarations[identifier][1]
    if it_num < 0:
      raise ValueError(f"Identifer '{identifier}' has been declared, but has not been read in the dataset (no 'read {identifier} ...' instruction)")
    return self[it_num]

  @classmethod
  def _Handle_Read_instruction(cls, stream, currPos, ret):
    """ Private. The 'read' keyword is tricky ... We parse it as if it was a plain instanciation, i.e. 
        read pb { ... }
    is turned into
        <blank> pb_conduction { ...}
    """
    save_i_rd = stream.pos()-1
    identif = stream.nextLow()
    save_i_id = stream.pos()-1
    # Is this a valid identifier?
    if not identif in ret._declarations:
      err = cls.GenErr(stream, f"Referencing object '{identif}' (with a 'read|lire' instruction) which was not declared before")
      raise ValueError(err)
    # Retrieve real underlying class from initial forward declaration and
    # strip the '' that will be added again by ConstrainBase.ReadFromTokens:
    decl, _ = ret._declarations[identif]
    ze_cls = decl._cls_obj
    cls_nam = ze_cls.__name__[:-_LEN_TRIOU_SUFFIX]  # Use the native root class name
    # Make a fake stream where we replace the identifier token by the class name (if it expects to read it)
    # and then proceed as usual:
    s2 = stream.clone()
    if ze_cls._read_type:
      s2.tokLow[save_i_rd], s2.tokLow[save_i_id] = "", cls_nam  # 'read pb {' ... becomes '<blank> pb_conduction { ...'
      # Set second stream at the correct position (before 'read' keyword which has now been blanked)
      s2.setPos(currPos)
      # IMPORTANT: in the below, the upper case tokens of 's2' are the same as in 'stream'
      # Only the low ones have been modfied. So the restitution (toDatasetTokens()) will be OK ... lucky for us
      inst = ze_cls.ReadFromTokens(s2)
      # Resync main stream
      stream.setPos(s2.pos())
    else:
      # If type does not need to be read, we can directly proceed with the same stream
      inst = ze_cls.ReadFromTokens(stream)
      tk_lst = stream.tok[currPos:save_i_id+1]
      inst._tokens["cls_nam"] = TRUSTTokens(low=[], orig=tk_lst)
    # Append object in dataset and register its position for the get() method:
    ret.append(inst)
    # Set the final position of the object in the dataset:
    ret._declarations[identif][1] = len(ret)-1

  @classmethod
  def _Handle_std_instanciation(cls, tok, stream, ze_cls, ret):
    """ Private. """
    # In a dataset the type must **always** be read - so it is just a matter to know if the keyword
    # itself reads it, or if it is the Dataset that reads it:
    if ze_cls._read_type:
      cls._Dbg(f"@FUNC@ '{tok}' standard instanciation with type reading")
      inst = ze_cls.ReadFromTokens(stream)
    else:
      cls._Dbg(f"@FUNC@ '{tok}' standard instanciation WITHOUT type reading")
      stream.validateNext() # consume type keyword
      cls_tok = stream.lastReadTokens()
      inst = ze_cls.ReadFromTokens(stream)
      inst._tokens["cls_nam"] = cls_tok
    ret.append(inst)

  @classmethod
  def _Handle_fwd_decl(cls, tok, stream, ze_cls, ret):
    """ Private. """
    decl = Declaration.ReadFromTokens(stream)
    identif = decl.identifier
    cls._Dbg(f"@FUNC@ fwd decl - '{identif}' with class '{decl.cls_nam}'")
    ret._declarations[identif] = [decl, -1]  # -1 because for now position of the real instanciation in the dataset is not known
    ret.append(decl)
    return True

  @classmethod
  def _RegisterAfterPotentialInst(cls, ret, potential_inst):
    """ Private. """
    cls_nam = potential_inst.__class__.__name__
    cls._Dbg(f"@FUNC@ Registering definitly std instanciation of type (class) '{cls_nam}'")
    ret.insert(-1, potential_inst)

  @classmethod
  def ReadFromTokens(cls, stream):
    """ Override. See Base_common. 
    Read a complete TRUST dataset
    """
    ret = cls()

    while not stream.eof():
      currPos = stream.pos()
      tok = stream.probeNextLow()
      cls._Dbg(f"@FUNC@ probing tok '{tok}'")

      # 'Read' instruction
      if tok in ["read", "lire"]:
        cls._Dbg(f"@FUNC@ 'read' KW")
        stream.validateNext()
        cls._Handle_Read_instruction(stream, currPos, ret)
        continue

      # Valid class? Note this will be checked again when in ReadTokens
      cls_nam = tok + _TRIOU_SUFFIX
      if not CLFX.isValidClassName(cls_nam):
        err = cls.GenErr(stream, f"Keyword '{tok}' is not a valid TRUST keyword")
        raise ValueError(err)
      ze_cls = CLFX.getXyzClassFromName(cls_nam)

      # 'interprete' are the highest level keywords in TRUST - they perform an action. They are only allowed at top level
      # in the dataset.
      if issubclass(ze_cls, interprete):
        # Standard keyword instanciation
        cls._Handle_std_instanciation(tok, stream, ze_cls, ret)
        # Should we stop here?
        if ze_cls is fin:
          break
      else:
        # Forward declaration
        cls._Handle_fwd_decl(tok, stream, ze_cls, ret)

    return ret

  def toDatasetTokens(self):
    """ Override. """
    s = []
    for el in self:  # 'self' inherits from list
      if not el._read_type:
        s.extend(el._tokens["cls_nam"].orig())
      s.extend(el.toDatasetTokens())
    return s

######################################################
# Simple leaf types (string, int, floats, ...)
######################################################
class AbstractChaine(Base_common):
  """ Base class for all (constrained or not) strings """
  _plainType = True

  def __init__(self):
    Base_common.__init__(self)

  @classmethod
  def _ValidateValue(cls, value, stream):
    """ To be overriden in derived class. Should raise if value is not allowed. """
    pass

  @classmethod
  def ReadFromTokens(cls, stream):
    """ Override. See Base_common. 
    In this override simply return the next token which is a valid (non blank) string. 
    """
    s = stream.nextLow()
    cls._ValidateValue(s, stream)
    ret = cls(s)  # Build the actual string
    ret._tokens["val"] = stream.lastReadTokens()
    return ret

  def toDatasetTokens(self):
    """ Override. """
    if self._checkToken("val", str(self)):
      return self._tokens["val"].orig()
    else:
      ret = [" " + str(self)]
      ## [ABN] - below is useless since Str are immutable ??
      # Try as much as possible to keep comments etc ... around string value:
      # if "val" in self._tokens:
      #   low, up = self._tokens["val"]
      #   f = list(filter(lambda s: s == "", low))
      #   if len(f) == 1:
      #     idx = low.index(f[0])
      #     # Now inside the token itself, replace string (preserving spaces, line returns around it)
      #     l = up[idx].lower()
      #     j = l.find(low[idx])
      #     low[idx] = low[:idx] + str(self).lower() + low[idx:]
      #     up[idx] = up[:idx] + str(self) + up[idx:]
      #     ret = up
      return ret

##############################################
class chaine(StrVerbatimXyz, AbstractChaine):
  """A simple 'chaine' (string) ... but this is tricky. It might be made of several tokens if braces are found
  See ReadFromTokens below ..
  """
  _defaultValue = "??"

  def __init__(self, value=None):
    StrVerbatimXyz.__init__(self, value)
    Base_common.__init__(self)
    self._withBraces = False     # Whether this chain is a full bloc with '{ }' - not to be confused with self._braces!!

  @classmethod
  def ReadFromTokens(cls, stream):
    """ Read a 'chaine' : this can be:
    - either a single token 'toto' (in which case, just take one token)
    - or a full block, like '{ toto { tata } }' (in which case take all tokens until closed brace)
    """
    tok = stream.probeNextLow()
    withBr = (tok == '{')
    if not withBr:
      # Easy just take one token - take its **lower-case** version
      stream.validateNext()
      lst_rd = stream.lastReadTokens()
      s = ''.join(lst_rd.low())
      if s.strip().startswith("{") or s.strip().startswith("}"):
        err = cls.GenErr(stream, f"Misformatted string or block within braces -> '{s}'")
        raise ValueError(err)
      ret = cls(s)
      ret._withBraces, ret._tokens["val"] = False, lst_rd
      return ret
    # With braces:
    l = []
    cls.ConsumeBrace(stream, "{")
    l.append(stream.lastReadTokens())
    cnt_brace = 1
    while cnt_brace != 0:
      tok = stream.probeNextLow()
      if tok == '{':
        cnt_brace += 1
      if tok == '}':
        cnt_brace -= 1
        if cnt_brace == 0: break  # without validating token so that it is consumed by ConsumeBrace()
      stream.validateNext()
      # When reading a full block, keep the upper case version
      l.append(stream.lastReadTokens())
    cls.ConsumeBrace(stream, "}")
    l.append(stream.lastReadTokens())
    l2 = TRUSTTokens.Join(l)
    s = ''.join(l2.orig())
    ret = cls(s)
    ret._withBraces, ret._tokens["val"] = True, l2
    return ret

  def toDatasetTokens(self):
    if self._withBraces:
      return [str(self)]
    else:
      return AbstractChaine.toDatasetTokens(self)

class BaseChaine(StrNoBlankXyz, AbstractChaine):
  """A string with no blank in it."""
  _defaultValue = "??"

  def __init__(self, value=None):
    StrNoBlankXyz.__init__(self, value)
    AbstractChaine.__init__(self)

class BaseChaineInList(StrInListXyz, AbstractChaine):
  """ Same as BaseChaine, but with constrained string values """
  _allowedList = ["??"]

  def __init__(self, value=None):
    StrInListXyz.__init__(self, value)
    AbstractChaine.__init__(self)

  @classmethod
  def _ValidateValue(cls, val, stream):
    if val not in cls._allowedList:
      err = cls.GenErr(stream, f"Invalid value: '{val}', not in allowed list: '%s'" % str(cls._allowedList))
      raise ValueError(err)

class fin(interprete):
  """ The 'end' keyword at the end of the dataset. It is an 'interprete'.
  It has its own class because the main parsing loop in DataSet needs to spot it specifically :-)
  """
  _braces = 0
  _read_type = False
  _synonyms = ["end"]

class AbstractEntier(Base_common):
  """ Base class for all (constrained or not) integers """
  _plainType = True

  def __init__(self):
    Base_common.__init__(self)

  @classmethod
  def _ValidateValue(cls, val, stream):
    """ To be overriden in derived class. Should raise if value is not allowed. """
    pass

  @classmethod
  def ReadFromTokens(cls, stream):
    """ Parse a single int """
    s = stream.nextLow()
    cls._ValidateValue(s, stream)
    try:
      ret = cls(s)  # Build the actual int
    except:
      err = cls.GenErr(stream, f"Invalid int value: '{s}'")
      raise ValueError(err) from None
    ret._tokens["val"] = stream.lastReadTokens()
    return ret

  def toDatasetTokens(self):
    """ Override. 
    See commented code in AbstractChaine if one day you want to perform a very fine replacement keeping comments
    for example.
    """
    self_as_str = str(int(self))
    if self._checkToken("val", self_as_str, int):
      ret = self._tokens["val"].orig()
    else:
      ret = [" " + self_as_str]
    return ret

class BaseEntier(IntXyz, AbstractEntier):
  def __init__(self, value=None):
    IntXyz.__init__(self, value)
    AbstractEntier.__init__(self)

class BaseEntierInList(IntInListXyz, AbstractEntier):
  """ Integer in a constrained list """
  _allowedList = [0]

  def __init__(self, value=None):
    IntInListXyz.__init__(self, value)
    AbstractEntier.__init__(self)

  @classmethod
  def _ValidateValue(cls, val, stream):
    try:
      i = int(val)
    except:
      err = cls.GenErr(stream, f"Invalid value: '{val}', could not be interpreted as an integer")
      raise ValueError(err)
    if i not in cls._allowedList:
      err = cls.GenErr(stream, f"Invalid value: '{val}', not in allowed list: '%s'" % str(cls._allowedList))
      raise ValueError(err)

class BaseEntierInRange(IntRangeXyz, AbstractEntier):
  """ integer in a constrained range """
  _allowedRange = [0, 0]

  def __init__(self, value=None):
    IntRangeXyz.__init__(self, value)
    AbstractEntier.__init__(self)

  @classmethod
  def _ValidateValue(cls, val, stream):
    ar = cls._allowedRange
    try:       i = int(val)
    except:
      err = cls.GenErr(stream, f"Invalid value: '{val}', could not be interpreted as an integer")
      raise ValueError(err)
    if i < ar[0] or i > ar[1]:
      err = cls.GenErr(stream, f"Invalid value: '{val}', not in allowed range [%d, %d]" % (ar[0], ar[1]))
      raise ValueError(err)

class BaseFloattant(FloatXyz, Base_common):
  _plainType = True

  def __init__(self, value=None):
    FloatXyz.__init__(self, value)
    Base_common.__init__(self)

  @classmethod
  def ReadFromTokens(cls, stream):
    """ Parse a single float """
    s = stream.nextLow()
    try:
      ret = cls(s)  # Build the actual float
    except:
      err = cls.GenErr(stream, f"Invalid float value: '{s}'")
      raise ValueError(err) from None
    ret._tokens["val"] = stream.lastReadTokens()
    return ret

  def toDatasetTokens(self):
    """ Override. 
    See commented code in AbstractChaine if one day you want to perform a very fine replacement keeping comments
    for example.
    """
    self_as_str = str(float(self))
    if self._checkToken("val", self_as_str, float):
      ret = self._tokens["val"].orig()
    else:
      ret = [" " + self_as_str]
    return ret

class Rien(BoolXyz, Base_common):
  """ Boolean flags are of type 'rien' in the TRAD_2 file."""
  _plainType = True

  @classmethod
  def ReadFromTokens(cls, stream):
    """Override. If we call this method it means the flag has been read, so it is always successful, not token read."""
    return True

  def toDatasetTokens(self):
    """ Override. """
    return ['']

class Todo(TodoXyz, Base_common):
  _defaultValue = "TODO (class not implemented yet from TRUST)"
  pass
