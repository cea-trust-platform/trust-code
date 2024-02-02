#!/usr/bin/env python
# -*- coding: utf-8 -*-

# %% LICENSE_SALOME_CEA_BEGIN
# see trustpy/LICENCE file
# %% LICENSE_END

"""
This is unittest reincarnation of obsolete examples/example_3/try_TRAD_2_v1.5_GUI.py
"""

import os
import os.path as OP
import pprint as PP
import unittest

from trustpy.unittest_custom_methods_trust import UnittestCustomMethodsTrust
import trustpy.baseTru as BTRU
import trustpy.trust_utilities as TRUU
import xyzpy.classFactoryXyz as CLFX

import trustpy.test.trust_all as TALL

verbose = False
verboseHelp = False
timeout = 10

########################################################################################
class TestCase(unittest.TestCase, UnittestCustomMethodsTrust):
  """
  This is unittest reincarnation of obsolete examples/example_3
  """

  _test_dir = os.path.abspath(os.path.dirname(__file__))
  _models = []

  def reinitClasses(self):
    CLFX.resetDefaults()
    BTRU.initXyzClasses()

  def test_000(self):
    """done first to set stdout verbosity tests redirection"""
    if not verbose:
      self.redirect_stdout()
    self.print_begin_test()
    self.reinitClasses()

  def test_999(self):
    """done last to set stdout verbosity tests redirection"""
    self.print_begin_test()
    CLFX.resetDefaults() # reset classFactoryXyz for next tests suites
    self.restore_stdout()
    if verbose: print("END %s" % self.caller_name_lineno())

  def test_010(self):
    """test inheritage"""
    self.print_begin_test()

    a = TALL.TRUSTAll_Tru()
    help = a.getHelp()
    if verboseHelp: a.logHelp()
    self.assertIn("class TRUSTAll_Tru(trustpy.baseTru.ConstrainBase_Tru)", help)
    inheritage = a.getInheritage()

    self.assertIn("trustpy.baseTru.ConstrainBase_Tru", inheritage)
    self.assertIn("builtins.object", inheritage)
    self.assertEqual("TRUSTAll_Tru", inheritage[0])
    TRUU.log_debug("trust_all getInheritage\n%s" % PP.pformat(inheritage))

    inheritage = a.getInheritageTru()
    self.assertIn("trustpy.baseTru.ConstrainBase_Tru", inheritage)
    self.assertNotIn("builtins.object", inheritage)
    self.assertEqual("TRUSTAll_Tru", inheritage[0])
    TRUU.log_debug("trust_all getInheritageTru\n%s" % PP.pformat(inheritage))

  def test_020(self):
    """test trad2_utilities.trad2xyz method"""
    self.print_begin_test()
    # create trust_packagespy_generated_test_500.py
    import trustpy.trad2_code_gen as T2C
    name = "trust_packagespy_generated_test_500.py"
    currentdir = OP.dirname(__file__)
    pname = OP.abspath(currentdir + '/' + name)
    ath = T2C.Trad2Handler(file_in=OP.abspath(currentdir + "/TRAD_2_v1.5_cv"), file_out=pname)
    ok = ath.trad2xyz()
    self.assertTrue(ok)
    TRUU.log_info("generated file is %s" % pname)
    self.assertTrue(OP.exists(pname))

  def test_025(self):
    """create_TRUSTAll import previously generated"""
    self.print_begin_test()
    self.reinitClasses()
    currentdir = OP.dirname(__file__)
    self.append_path(currentdir)
    TRUU.log_info("current unittest file dir %s\n%s" % (currentdir, self.get_import_paths()))
    import trust_packagespy_generated_test_500 as TRUG

    # make classes known by factory allow instanciate them in create_model
    TRUG_classes = TRUG.packagespy_classes
    CLFX.appendAllXyzClasses(TRUG_classes)

    # model is an instanciated trust_all class
    model = TALL.create_model_all(trust_packagespy_generated=TRUG)
    if verboseHelp: model.logHelp()
    self._models.append(model)  # to use it next test
    self.restore_path(currentdir)

  def test_030(self):
    """display_TRUSTAll in MVC etc."""
    # model is an instanciated trust_all class
    self.print_begin_test()
    import xyzpy.guiXyz.model_view_default as MVD
    model = self._models[0]
    controller = MVD.create_controller(model)
    view = MVD.create_treeview(controller, title=self.caller_method_name())
    self.show_launch_timer(view, timeout=timeout)

  def test_040(self):
    """explore inspect.signature() just for fun"""
    self.print_begin_test()
    import inspect
    import trust_packagespy_generated_test_500 as TRUG
    import xyzpy.loggingXyz as LOG
    # res = inspect.signature(TRUG.comment_Tru.__init__)  # <classmethod object at 0x7f4447e93e20> is not a callable object
    # res = inspect.signature(TRUG.ReadFromTokens_dim)  # classmethod object at 0x7f40d1345d00> is not a callable object
    res = inspect.signature(LOG.formatList)  # TRUG.comment_Tru.__init__)  # TRUG.ReadFromTokens_dim)
    self.assertIn("max_items=200", str(res))
    # print("inspect.signature LOG.formatList %s" % res)


if __name__ == '__main__':
  verbose = True  # verbose if in main
  verboseHelp = False
  timeout = 120
  unittest.main()
  pass
