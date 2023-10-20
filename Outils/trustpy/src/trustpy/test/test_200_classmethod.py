#!/usr/bin/env python
# -*- coding: utf-8 -*-

# %% LICENSE_SALOME_CEA_BEGIN
# see trustpy/LICENCE file
# %% LICENSE_END


import unittest
import pprint as PP
import inspect as INSP

verbose = False

def printcheck(a):
  if verbose:
    print("\na.__dict__\n%s" % PP.pformat(a.__dict__))
    print("\nTest_A.__dict__\n%s" % PP.pformat(Test_A.__dict__))
    print("\ndir(a)\n%s" % PP.pformat(dir(a)))
    print("\nvars(a)\n%s" % PP.pformat(vars(a)))
    # PP.pprint("help(a) %s" % help(Test_A))

def method_2(self):
  """docstring for method_2"""
  return "from method_2"

# a comment before Test_A
class Test_A():
  """docstring for class Test_A"""

  class_attribute_1 = "class_attr_1"

  def __init__(self):
    self.attribute_1 = 1

  def method_1(self):
    """docstring for method_1"""
    return "from method_1"


########################################################################################
class TestCase(unittest.TestCase):

  def test_010(self):
    a = Test_A()
    self.assertEqual("from method_1", a.method_1())
    if verbose:
        PP.pprint(dir(a))
    self.assertIn("method_1", dir(a))

  def test_015(self):
    Test_A.method_2 = method_2
    a = Test_A()
    self.assertEqual("from method_2", a.method_2())
    self.assertIn("method_1", dir(a))
    self.assertIn("method_2", dir(a))
    self.assertEqual("docstring for method_1", INSP.getdoc(a.method_1))
    self.assertEqual("docstring for method_2", INSP.getdoc(a.method_2))
    self.assertEqual("docstring for method_1", INSP.getdoc(Test_A.method_1))
    self.assertEqual("docstring for method_2", INSP.getdoc(Test_A.method_2))

  def test_020(self):
    a = Test_A()   # 'Test_A.method_2 = method_2' mandatory done in test_015
    self.assertIn("test_200_classmethod.py", str(INSP.getmodule(a)))
    self.assertIn("a comment before Test_A", str(INSP.getcomments(Test_A)))

    res = INSP.getmembers(Test_A, INSP.isfunction)
    names = [i[0] for i in res]
    if verbose: print("getmembers(Test_A, isfunction)\n%s" % PP.pformat(res))
    self.assertIn("__init__", names)
    self.assertIn("method_1", names)
    self.assertIn("method_2", names)
    printcheck(a)
    self.assertIn("attribute_1", a.__dict__)
    self.assertNotIn("class_attribute_1", a.__dict__)
    self.assertIn("class_attribute_1", Test_A.__dict__)



if __name__ == '__main__':
  verbose = True
  unittest.main()
  pass
