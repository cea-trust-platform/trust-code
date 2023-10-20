#!/usr/bin/env python
# -*- coding: utf-8 -*-

# %% LICENSE_SALOME_CEA_BEGIN
# see trustpy/LICENCE file
# %% LICENSE_END


import unittest
from trustpy.unittest_custom_methods_trust import UnittestCustomMethodsTrust

from xyzpy.baseXyz import _XyzConstrainBase
import xyzpy.intFloatListXyz as IFLX
import xyzpy.classFactoryXyz as CLFX
import xyzpy.loggingXyz as LOG

verbose = False
timeout = 5  # seconds

def printDebug(value):
  msg = str(value)
  if '\n' not in msg:
    msg = "DEBUG: %s" % msg
  else:
    msg = "DEBUG:\n  %s" % LOG.indent(msg)
  print(LOG.toColor("<yellow>") + msg + LOG.toColor("<reset>"))

def get_tokens(aStr):
  """"because '  a   b c   '.split() -> ['a', 'b', 'c']"""
  tmp = str(aStr)
  return tmp.split()


class defbord_2_test_Tru(_XyzConstrainBase):
  """two edges as 'x = 0.0   0.0 <= y <= 1.0' for example"""
  _verbose = True
  _className = "defbord_2_Tru"

  _attributesList = [
    ("edge_1", "StrEdge_1_Tru"),
    ("edge_2", "StrEdge_2_Tru")
  ]

  def __setattr__(self, name, value):
    super(defbord_2_test_Tru, self).__setattr__(name, value)
    self.on_attributesChange()

  def __init__(self):
    super(defbord_2_test_Tru, self).__init__()
    self.setIsCast(True)
    self._setAllAttributesList()
    # self.check_values()

  def getEdge_1(self):
    return str(self.edge_1)

  def getEdge_2(self):
    return str(self.edge_2)

  def getEdge_1_list(self):
    return str(self.edge_1).split()

  def getEdge_2_list(self):
    return str(self.edge_2).split()

  def checkValues(self, isRaiseException=False):
    try:
      var_1, _, _ = self.getEdge_1_list()
      _, _, var_2, _, _ = self.getEdge_2_list()
    except Exception as e:  #  may be attributes not set
      return False

    if var_1 == var_2:  # verboten
      msg = "%s.check_values variable edge_1 '%s' and variable edge_2 '%s' have to be distinct" % (self._className, var_1, var_2)
      if isRaiseException:
        raise Exception(msg)
      else:
        return False
    return True

  def on_attributesChange(self, verbose=False):
    # print("%s.on_attributesChange" % self._className)
    res = self.checkValues(isRaiseException=True)
    return res


class StrEdge_1_Tru(IFLX.StrXyz):
  """string only as 'x = 0.0' for example"""
  _defaultValue = "x = 0.0"

  def __new__(cls, value=None):
    if value == None:
      tokens = get_tokens(cls._defaultValue)
    else:
      tokens = get_tokens(value.replace('=', ' = '))  # case 'x=0.0'

    strValue = " ".join(tokens)

    if len(tokens) != 3:
      msg = "incorrect string '%s', expected 3 tokens as 'x = 0.0'" % strValue
      raise Exception(msg)

    if tokens[0] not in "x y z".split():
      msg = "incorrect string '%s', expected tokens 1 as x or y or z'" % strValue
      raise Exception(msg)

    if tokens[1] != "=":
      msg = "incorrect string '%s', expected tokens 2 as '='" % strValue
      raise Exception(msg)

    try:
      tmp = float(tokens[2])
    except:
      msg = "incorrect string '%s', expected tokens 3 as a float number" % strValue
      raise Exception(msg)

    obj = str.__new__(cls, strValue)
    return obj


class StrEdge_2_Tru(IFLX.StrXyz):
  """string only as '0.0 <= y <= 1.0' for example"""
  _defaultValue = "0.0 <= y <= 1.0"

  def __new__(cls, value = None):
    if value == None:
      tokens = get_tokens(cls._defaultValue)
    else:
      tokens = get_tokens(value.replace('<=', ' <= '))  # case '0.0<=y<=1.0'

    strValue = " ".join(tokens)

    if len(tokens) != 5:
      msg = "incorrect string '%s', expected 5 tokens as '0.0 <= y <= 1.0'" % strValue
      raise Exception(msg)

    try:
      tmp1 = float(tokens[0])
    except:
      msg = "incorrect string '%s', expected tokens 1 as a float number" % strValue
      raise Exception(msg)

    if tokens[1] != "<=":
      msg = "incorrect string '%s', expected tokens 2 as '<='" % strValue
      raise Exception(msg)

    if tokens[2] not in "x y z".split():
      msg = "incorrect string '%s', expected tokens 3 as x or y or z'" % strValue
      raise Exception(msg)

    if tokens[3] != "<=":
      msg = "incorrect string '%s', expected tokens 4 as '<='" % strValue
      raise Exception(msg)

    try:
      tmp2 = float(tokens[4])
    except:
      msg = "incorrect string '%s', expected tokens 5 as a float number" % strValue
      raise Exception(msg)

    if tmp1 > tmp2:
      msg = "incorrect string '%s', because expression implies %s <= %s" % (strValue, tmp1, tmp2)
      raise Exception(msg)

    obj = str.__new__(cls, strValue)
    return obj

packagespy_classes = [StrEdge_1_Tru, StrEdge_2_Tru, defbord_2_test_Tru]

########################################################################################
class TestCase(unittest.TestCase, UnittestCustomMethodsTrust):
  """
  Testing loading/usage of some new classes into trustpy.
  """

  def test_000(self):
    """Done first to set stdout verbosity tests redirection"""
    if not verbose:
      self.redirect_stdout()
    self.print_begin_test()
    CLFX.resetDefaults()

  def test_999(self):
    """Done last to set stdout verbosity tests redirection"""
    self.print_begin_test()
    CLFX.resetDefaults()  # reset classFactoryXyz for next tests suites
    self.restore_stdout()
    # if verbose: print(self.caller_name_lineno())

  def test_002(self):
    """make test class known by class factory"""
    CLFX.appendAllXyzClasses(packagespy_classes)

  def test_010(self):
    a = defbord_2_test_Tru()
    self.assertEqual(StrEdge_1_Tru._defaultValue, a.getEdge_1())
    self.assertEqual(StrEdge_2_Tru._defaultValue, a.getEdge_2())
    self.assertTrue(a.checkValues())
    # printDebug(LOG.formatList(dir(a)))
    tmp = "0.0 <= x <= 1.0"
    # a.edge_2 = tmp
    self.assertRaises(Exception, a.__setattr__, "edge_2", tmp)
    tmp = "3.0 <= z <= 4.0"
    a.edge_2 = tmp
    self.assertEqual(tmp, a.getEdge_2())
    tmp = "3.0 <= z <= 0.0"
    self.assertRaises(Exception, a.__setattr__, "edge_2", tmp)

    # try GUI
    self.display_treeview(a, title=self.caller_method_name(), timeout=timeout)


if __name__ == '__main__':
  verbose = True
  timeout = 20
  unittest.main()
