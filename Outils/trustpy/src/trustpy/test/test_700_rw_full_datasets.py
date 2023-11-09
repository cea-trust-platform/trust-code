#!/usr/bin/env python
# -*- coding: utf-8 -*-

# %% LICENSE_SALOME_CEA_BEGIN
# see trustpy/LICENCE file
# %% LICENSE_END

"""
Testing loading of various dataset examples into the datamodel generated by trustpy.
"""

import os
import glob
import unittest

from trustpy.unittest_custom_methods_trust import UnittestCustomMethodsTrust
from reference_data import *
import trustpy.baseTru as BTRU
import trustpy.trust_utilities as TRUU
import xyzpy.classFactoryXyz as CLFX
import xyzpy.loggingXyz as LOG

from trustpy.trust_parser import TRUSTParser, TRUSTStream

timeout = 100
verbose = False

def printAdrien(value):
  msg = str(value)
  if '\n' not in msg:
    msg = "ADRIEN: %s" % msg
  else:
    msg = "ADRIEN:\n  %s" % LOG.indent(msg)
  # print(LOG.toColor("<yellow>") + msg + LOG.toColor("<reset>"))
  print(msg)

########################################################################################
class TestCase(unittest.TestCase, UnittestCustomMethodsTrust):
  """
  Testing loading of various complete dataset examples into the datamodel generated by trustpy.
  """

  _test_dir = os.path.abspath(os.path.dirname(__file__))
  _models = []

  def generic_test_700(self, data_ex, fnam="??"):
    """ Generic test method taking a (piece of) dataset and testing it. """
    # Generate if needed - the full thing here!!
    self.generate_python_and_import_full()
    # Parse the TRUST data set provided in arg
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(parser=tp, file_nam=fnam)
    tds_cls = CLFX.getXyzClassFromName("DataSet" + BTRU._TRIOU_SUFFIX)
    # import cProfile
    # cProfile.runctx("tds_cls.ReadFromTokens(stream)", globals=globals(), locals=locals())
    ds = tds_cls.ReadFromTokens(stream)
    return ds

  def load_file(self, fName):
    """ Open a test file located in subfolder 'datasets' and return its content as a string. """
    dr = self._test_dir + "/datasets"
    with open(dr + "/" + fName) as f:
      # rstrip() to get rid of any end of line, blanks, etc ... after the 'end' keyword
      return f.read().rstrip()

  def reinitClasses(self):
    CLFX.resetDefaults()
    # BTRU.initXyzClasses()

  #########################################################
  def test_000(self):
    """ Done first to set stdout verbosity tests redirection"""
    if not verbose:
      self.redirect_stdout()
    self.print_begin_test()
    self.reinitClasses()

  def test_999(self):
    """ Done last to set stdout verbosity tests redirection"""
    self.print_begin_test()
    CLFX.resetDefaults()  # reset classFactoryXyz for next tests suites
    self.restore_stdout()

  def test_100(self):
    """ Almost the same as test_500 in test_504_read_write_data, but this time:
      - with the full TRAD_2 file
      - with the domain attribute set, which was not there in the simpler case
    """
    # self.__class__._do_not_regenerate = True
    self.print_begin_test()

    data_ex = """
    # Some stupid test #
    champ_uniforme gravite 
    nom coucou
    read gravite 2 28 32   # Keyword with no brace #
    read coucou toto
    lire_MED
      {
         domain dom
         mesh ze_mesh_name
         file a/complicated/path/to.med
         exclude_groups 2 toto titi
         convertAllToPoly
      }"""
    res = self.generic_test_700(data_ex)
    self.assertEqual(len(res), 5)
    # Build expected value
    exp = buildMinimalExpec(self._TRUG[1])
    exp[4].domain = "dom"
    # Test
    for i in range(5):
      self.assertTrue(exp[i].equal(res[i]))
    # Test write out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

  def test_150(self):
    """ Probes ... what a pain ... deserve their own unit test. """
    self.print_begin_test()
    self.generate_python_and_import_full()
    tp = TRUSTParser()
    # Force dimension (normally comes from keyword 'dimension' in dataset)
    self._TRUG[1].AbstractSizeIsDim_Tru._DIMENSION = 2
    # One probe (type 'sonde' in TRAD_2):
    data_ex = "sonde_pression pression periode 0.005 points 2 0.13 0.105 0.13 0.115"
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    S = self._TRUG[1].sonde_Tru
    res = S.ReadFromTokens(stream)
    exp = buildSondesExpec(self._TRUG[1])
    self.assertTrue(exp.equal(res))
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)
    # Try changing value:
    res.type.points[0].pos[1] = 0.123
    news = "sonde_pression pression periode 0.005 points 2 0.13 0.123 0.13 0.115"
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, news).ok)
    # Reset dimension
    self._TRUG[1].AbstractSizeIsDim_Tru._DIMENSION = -1

  def test_151(self):
    """ Bords (in 'mailler') are also tricky """
    self.print_begin_test()
    # self.__class__._do_not_regenerate = True
    data_ex = "Bord periox X = 0.  0. <= Y <= 2.0"
    self.generate_python_and_import_full()
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    # Force dimension (normally comes from keyword 'dimension' in dataset)
    self._TRUG[1].AbstractSizeIsDim_Tru._DIMENSION = 2
    S = self._TRUG[1].bord_Tru
    # Replicate hack performed in 'trust_hacks.py' when dimension is read in the dataset:
    S._attributesList[1] = (S._attributesList[1][0], 'defbord_2_Tru')
    res = S.ReadFromTokens(stream)
    exp = buildBordExpec(self._TRUG[1])
    self.assertTrue(exp.equal(res))
    check = exp.check_equality(res)
    # printAdrien("exp.check_equality(res) %s" % check.toStr())
    self.assertTrue(check.ok)
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)
    # Try changing value:
    res.nom = "bip"
    res.defbord.edge_1 = "X = 24.2"
    news = """Bord bip X = 24.2  0. <= Y <= 2.0"""
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, news).ok)
    # Reset dimension
    self._TRUG[1].AbstractSizeIsDim_Tru._DIMENSION = -1

  def test_152(self):
    """ Testing bloc_lecture via 'solveur_pression' """
    self.print_begin_test()
    # self.__class__._do_not_regenerate = True
    data_ex = """{
       solveur_pression petsc cholesky {   nImp c est un blOc leCtuRe }
       convection { negligeable }
    }"""
    self.generate_python_and_import_full()
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    S = self._TRUG[1].navier_stokes_standard_Tru
    res = S.ReadFromTokens(stream)
    exp = buildSolveurPressionExpec(self._TRUG[1])
    self.assertTrue(exp.equal(res.solveur_pression))
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)
    # Try changing value:
    res.solveur_pression.option_solveur.bloc_lecture = "{  taDa tudu }"
    news = """ petsc cholesky {  taDa tudu }"""
    s = ''.join(res.solveur_pression.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, news).ok)

  def test_153(self):
    """ Testing complex lists """
    self.print_begin_test()
    # self.__class__._do_not_regenerate = True
    data_ex = """
    dom_solide
    {
        Pave Cavite1 { } { } ,
        Pave Cavite2 { } { }
    }"""
    self.generate_python_and_import_full()
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    S = self._TRUG[1].mailler_Tru
    res = S.ReadFromTokens(stream)
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)
    # Try changing value (delete last item in list, and change name)
    res.bloc.pop()
    res.bloc[0].name = "toto"
    news = """
    dom_solide
    {
        Pave toto { } { }
    }"""
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, news).ok)

  def test_154(self):
    """ Testing 'list_nom_virgule' """
    self.print_begin_test()
    # self.__class__._do_not_regenerate = True
    data_ex = """{ vx , vy }"""
    self.generate_python_and_import_full()
    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    S = self._TRUG[1].list_nom_virgule_Tru
    NA = self._TRUG[1].nom_anonyme_Tru
    res = S.ReadFromTokens(stream)
    # Test writing out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

############################################################
## From here on, full datasets from subfolder 'datasets'
##   We don't build full expected value in Python anymore
##   (too much work ...) and simply test equality when writing out.
############################################################
  def test_200(self):
    """ Dataset: upwind_simplified.data """
    # self.__class__._do_not_regenerate = True
    self.print_begin_test()
    data_ex = self.load_file("upwind_simplified.data")
    res = self.generic_test_700(data_ex)
    # self.display_treeview(res, title=self.caller_method_name(), timeout=timeout)
    # Test write out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)
    # # Test changing one bit:
    # tmp = res.get("pb").post_processing.sondes.pop()
    # tmp._parentAsAttribute = None
    # res.get("pb").post_processing.sondes.insert(0, tmp)
    # one_prob.type.points[0].pos[1] = 0.123
    # s = ''.join(res.toDatasetTokens())
    # print(s)

  def test_210(self):
    """ Dataset: Canal_perio_VEF_2D.data """
    self.print_begin_test()
    data_ex = self.load_file("Canal_perio_VEF_2D.data")
    res = self.generic_test_700(data_ex)

    # Nice GUI
    if LOG.isChristian():
      self.display_treeview(res, title=self.caller_method_name(), timeout=timeout)

    # Test write out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

  def test_220(self):
    """ Dataset: diffusion_implicite_jdd6.data """
    # self.__class__._do_not_regenerate = True
    self.print_begin_test()
    data_ex = self.load_file("diffusion_implicite_jdd6.data")
    res = self.generic_test_700(data_ex)
    # Test write out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

  def test_230(self):
    """ Dataset:  distance_paroi_jdd1.data
    which used various <inherited> things (like 'ice' solver) and 'bloc_lecture'
    """
    self.print_begin_test()
    data_ex = self.load_file("distance_paroi_jdd1.data")
    res = self.generic_test_700(data_ex)
    # Test write out:
    s = ''.join(res.toDatasetTokens())
    self.assertTrue(self.check_str_equality(s, data_ex).ok)

  def xx_test_799(self):
    """ tmp single dataset """
    # self.__class__._do_not_regenerate = True
    self.print_begin_test()
    e = "/export/home/adrien/Projets/TRUST/TRUST_LOCAL/tests/Reference/Kernel/Kernel.data"
    e = "/export/home/adrien/Projets/TRUST/TRUST_LOCAL/tests/EF/EF_Echange_global_impose/EF_Echange_global_impose.data"
    # e= "/export/home/adrien/Projets/TRUST/TRUST_192/tests/Reference/POISEUILLE_ALIGNED_power_law/POISEUILLE_ALIGNED_power_law.data"
    e = "/export/home/adrien/Projets/TRUST/TRUST_PKGPY/tests/Reference/Quasi_Comp_Obst_GP_VDF_FM/Quasi_Comp_Obst_GP_VDF_FM.data"
    e = "/export/home/adrien/Projets/TRUST/TRUST_LOCAL/tests/Verification/champ_fonc_med_table_jdd2/champ_fonc_med_table_jdd2.data"
    e = "/export/home/adrien/Projets/TRUST/TRUST_LOCAL/tests/Reference/Poreux_VEF_2D/Poreux_VEF_2D.data"
    e = "/export/home/adrien/Projets/TRUST/TRUST_LOCAL/tests/Reference/Poreps/Poreps.data"
    e = "/export/home/adrien/Projets/TRUST/TRUST_LOCAL/tests/Turbulence_Null/DYN_plans_paralleles_Turb_Null/DYN_plans_paralleles_Turb_Null.data"
    e = "/export/home/adrien/Projets/TRUST/TRUST_LOCAL/tests/Turbulence_Nul/Couplage_champ_cl_VDF_Turb_Nul/Couplage_champ_cl_VDF_Turb_Nul.data"
    d = os.path.dirname(e)
    bas = os.path.split(d)[-1]
    fNam = os.path.join(d, bas + ".data")
    with open(fNam) as f:
      data_ex = f.read()
    # printAdrien("'" + data_ex + "'")
    res = self.generic_test_700(data_ex, fnam=e)
    # Test write out:
    s = ''.join(res.toDatasetTokens())
    data_ex_p = self.prune_after_end(data_ex)
    self.assertTrue(self.check_str_equality(s, data_ex_p).ok)

  def xx_test_800(self):
    """ Test **all** TRUST datasets """
    trust_root = os.environ.get("TRUST_ROOT", None)
    #if trust_root is None:
    #  raise Exception("TRUST_ROOT not defined! Have you sourced TRUST?")
    tst_dir = os.path.join(trust_root, "tests")
    # Find all datasets:
    pattern = os.path.abspath(os.path.join(tst_dir, "**/*.lml.gz"))
    g = glob.glob(pattern, recursive=True)
    printAdrien("Number of test cases found: %d" % len(g))
    with open("/home/catA/ab205030/failed_trustpy.txt") as fl:
      short_lst = [s.strip() for s in fl.readlines()]
    ko = []
    for i, e in enumerate(g[:]):
      print("%d/%d" % (i, len(g)), end="\r")
      # Extract dir name, and build dataset file name
      d = os.path.dirname(e)
      bas = os.path.split(d)[-1]
      fNam = os.path.join(d, bas + ".data")
      if fNam in short_lst:
        continue
      with open(fNam) as f:
        data_ex = f.read()
      try:
        res = self.generic_test_700(data_ex)
        # Test write out:
        s = ''.join(res.toDatasetTokens())
        data_ex_p = self.prune_after_end(data_ex)
        # self.check_str_equality(s, data_ex_p).ok
        self.assertTrue(self.check_str_equality(s, data_ex_p).ok)
        # printAdrien("    -> OK!")
        # ok.append(fNam)
        # break
      except Exception as e:
        ko.append(fNam)
        printAdrien(f"   Dataset KO: {fNam}")
        printAdrien("    -> KO :-( %s" % e)
    print("\n".join(ko))
    print(len(ko))

if __name__ == '__main__':
  verbose = True  # verbose if in main
  TRUU._log_level = 3
  timeout = 300
  # import cProfile
  # cProfile.run("unittest.main()")
  unittest.main()