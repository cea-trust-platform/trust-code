#!/usr/bin/env python
# -*- coding: UTF-8 -*-

"""
Contains also KISS colorized logger to get minimum log
instead packagespy logger (using import more complex logging)
"""

import pprint as PP
import inspect  # this_function_name = inspect.currentframe().f_code.co_name

_sep = " > "  # distinct of ':' of packagespy logger

BLACK = "\033[0;30m"
RED = "\033[0;31m"
GREEN = "\033[0;32m"
BROWN = "\033[0;33m"
BLUE = "\033[0;34m"
PURPLE = "\033[0;35m"
CYAN = "\033[0;36m"
YELLOW = "\033[1;33m"
BOLD = "\033[1m"
NEGATIVE = "\033[7m"
END = "\033[0m"

_log_level = 3 # 0: nothing, 1: with err , 2: with warn, 3: with info, 4: with debug

########################################################
# utilities log
########################################################

def indent(mess):
  return "\n           ".join(mess.split('\n'))

def myprint(color, level, mess):
  print(color + level.ljust(8) + _sep + indent(mess) + END)

def log_warning(mess):
  if _log_level >= 2: myprint(RED, 'WARNING', mess)

def log_error(mess):
  if _log_level >= 1: myprint(RED + BOLD, 'ERROR', mess)

def log_info(mess):
  if _log_level >= 3: myprint(GREEN, 'INFO', mess)

def log_debug(mess):
  if _log_level >= 4: myprint(BLUE, 'DEBUG', mess)
  return

def caller_name():
  """caller method name"""
  return inspect.currentframe().f_back.f_code.co_name

def caller_name_lineno():
  """caller file name and line numero"""
  import os

  callerframerecord = inspect.stack()[2]
  frame = callerframerecord[0]
  info = inspect.getframeinfo(frame)
  file = os.path.basename(callerframerecord.filename)
  lineno = info.lineno
  res = "file %s lineno %i" % (file, lineno)
  return res

def log_current_name(msg=""):
  nam = inspect.currentframe().f_back.f_code.co_name
  log_info(nam + " : " + msg)

import xyzpy.loggingXyz as LOG
logger = LOG.getLogger()   # used as global

def import_generated_module(mod_path):
  """ Import a Python module given the path of the correpsonding py file.
  Returns the module object itself.
  """
  import os, sys

  # Import generated module:
  ze_dir, f = os.path.split(os.path.abspath(mod_path))
  prevPath = sys.path[:]
  sys.path.append(ze_dir)
  mod_nam = os.path.splitext(f)[0]
  res = {'res_import': None}
  cmd = "import %s as res_import" % mod_nam
  exec(cmd, res)  # need '.' as currentdir in PYTHONPATH to get import OK
  # Restore sys path
  sys.path = prevPath
  return res['res_import']

##################################################################

class UnitUtils():
  """
  Useful stuff for unit tests.
  """
  _stdout_stream = []
  _timeout = 20  # seconds
  _setTimer = True
  _TRUG = [None] * 3
  if os.environ.get("USER") == "ab205030":
    _TRAD2 = ["TRAD_2_adr_simple",
              # "TRAD_2_v192"]
              "myTRAD2",    # Generated with trustpy package from TRUST sources
              "TRAD_2_GENEPI_V17"]
  else:
    _TRAD2 = ["TRAD_2_adr_simple",
              "TRAD_2_v192",
              "TRAD_2_GENEPI_V17"]

  _name = ["trust_packagespy_generated_simple",
           "trust_packagespy_generated_v192_beta",
           "trust_packagespy_generated_GENEPI_V16"]
  _do_not_regenerate = False   # useful for debugging - avoid regenerating each time (this is a bit long with full TRAD2)

  def caller_method_name(self):
    import inspect
    callerframerecord = inspect.stack()[1]
    frame = callerframerecord[0]
    name = frame.f_code.co_name
    return name

  def _generate_python_and_import(self, slot):
    """ Generates 'name.py' in the current test directory
    returns imported module
    robust usage is insert '_generated_' in name of python file
    warning: this is python metaprogrammation """
    if not self._TRUG[slot] is None:
      return

    import xyzpy.classFactoryXyz as CLFX
    import trustpy.trad2_code_gen as T2C
    import trustpy.baseTru as BTRU
    from trustpy.trust_utilities import import_generated_module

    srcdir, currentdir = self._test_dir, os.getcwd()
    pname = os.path.abspath(srcdir + '/' + self._name[slot] + '.py')  # generated file with path in current dir
    if not self._do_not_regenerate:
      th = T2C.Trad2Handler(file_in=os.path.abspath(srcdir + "/" + self._TRAD2[slot]), file_out=pname)
      ok = th.trad2xyz()
      self.assertTrue(ok)
    self.append_path(currentdir)

    # Import generated module (IMPORTANT: set it at class level, not instance!!)
    self.__class__._TRUG[slot] = import_generated_module(pname)

    self.assertIsNotNone(self._TRUG[slot])
    self.assertEqual(self._TRUG[slot].__file__, pname)

    # Register classes
    BTRU.initXyzClasses()
    self._TRUG[slot].initXyzClasses()
    # TRUG_classes = self._TRUG[slot].packagespy_classes
    # CLFX.appendAllXyzClasses(TRUG_classes)

  def generate_python_and_import_simple(self):
    """ Generate and import automatically generated PY package from file TRAD_2_adr_simple
    which is a very reduced version of a complete TRAD_2 file
    """
    self._generate_python_and_import(0)

  def generate_python_and_import_full(self):
    """ Generate and import automatically generated PY package from the complete file of TRUST 
    """
    self._generate_python_and_import(1)

  def generate_python_and_import_GENEPI(self):
    """ Generate and import automatically generated PY package from the complete file 
    from GENEPI: TRAD_2_GENEPI_V16.
    """
    self._generate_python_and_import(2)

  def prune_after_end(self, data_ex):
    """ Produce a new exact same string where all the data after the 'end' keyword is discarded. """
    from trustpy.trust_parser import TRUSTParser, TRUSTStream

    tp = TRUSTParser()
    tp.tokenize(data_ex)
    stream = TRUSTStream(tp)
    j = -1
    for i, t in enumerate(stream.tokLow):
      if t in ["end", "fin"]:
        j = i
        break
    if j>0:
      stream.dropTail(j)
    else:
      # Many datasets do not have 'end' so just remove last blanks
      for i, t in enumerate(stream.tokLow[::-1]):
        if t != '':
          j = i
          break
      if j>0:
        stream.dropTail(len(stream) - j - 1)
    return ''.join(stream.tok)

  def check_str_equality(self, s1, s2, print_on_diff=True):
    """
    Check equality between two (long) strings (with line returns) and produce a diff (as Linux 'diff') if not equal.
    @return a CheckBaseXyz object which has 'check.ok' attribute (boolean), and 'check.why' (string) as diff string. 
    """
    import difflib as DIFF
    from xyzpy.baseXyz import CheckBaseXyz
    s1, s2 = str(s1), str(s2)
    if s1 != s2:
      why = ""
      str1 = s1.split('\n')
      str2 = s2.split('\n')
      res = DIFF.unified_diff(str1, str2)
      res = [i for i in res]      # unified_diff 'generator' object is not subscriptable
      res = '\n  ' + '\n  '.join(res[2:])  # avoid firsts lines
      why = "Diff between the two strings:\n%s" % res
      if print_on_diff:
        print("")
        TRUU.log_error(why)
      return CheckBaseXyz(False, why)
    return CheckBaseXyz(True, "Strings are equal")


