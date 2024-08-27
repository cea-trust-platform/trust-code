"""
Various utilities.
"""

_log_level = 3 # 0: nothing, 1: with err , 2: with warn, 3: with info, 4: with debug

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
## Various datasets utils
##################################################################

def simplify_successive_blanks(aStr):
  """
  reduce all succesive blanks and new lines to one blank
  remove leadind/trailing blanks
  simplify_successive_blanks('  aa \n  bb   ') -> 'aa bb'
  """
  res = aStr.split()
  return " ".join(res)

def prune_after_end(data_ex):
  """ Produce a new exact same string where all the data after the 'end' keyword is discarded. """
  from trust_parser import TRUSTParser, TRUSTStream

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

def check_str_equality(s1, s2, print_on_diff=True):
  """
  Check equality between two (long) strings (with line returns) and produce a diff (as Linux 'diff') if not equal.
  @return a CheckBaseXyz object which has 'check.ok' attribute (boolean), and 'check.why' (string) as diff string. 
  """
  raise NotImplementedError # TODO
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


def change_class_name(s):
    return s.capitalize()

##################################################################
## Various unit tests utils
##################################################################

class UnitUtils():
  """
  Useful stuff for unit tests.
  """
  _stdout_stream = []
  _TRUG = [None] * 3     # The list of generated modules, as Python module objects
  _TRAD2 = ["TRAD_2_adr_simple",
          # "TRAD_2_v192"]
          "TRAD2_full",    # Generated with trustpy TRAD2 utilities from TRUST sources
          "TRAD_2_GENEPI_V17"]

  _name = ["trustpy_generated_simple",
           "trustpy_generated_v192_beta",
           "trustpy_generated_GENEPI_V16"]

  _do_not_regenerate = True   # useful for debugging - avoid regenerating each time (this is a bit long with full TRAD2)

  def caller_method_name(self):
    import inspect
    callerframerecord = inspect.stack()[1]
    frame = callerframerecord[0]
    name = frame.f_code.co_name
    return name

  def _generate_python_and_import(self, slot):
    """ Generates 'name.py' in the current test directory and stores
    imported module in self._TRUG slot.
    """
    import os
    if not self._TRUG[slot] is None:
      return

    from .trad2_pydantic import generate_pydantic

    srcdir, currentdir = self._test_dir, os.getcwd()
    file_in = os.path.abspath(srcdir + "/" + self._TRAD2[slot])
    file_out = os.path.abspath(srcdir + '/generated/' + self._name[slot] + '.py')  # generated file with path in current dir
    if not self._do_not_regenerate:
      generate_pydantic(file_in, file_out)

    # Import generated module (IMPORTANT: set it at class level, not instance!!)
    self.__class__._TRUG[slot] = import_generated_module(file_out)

    self.assertIsNotNone(self._TRUG[slot])
    self.assertEqual(self._TRUG[slot].__file__, file_out)

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

  def getClassFromName(self, cls_nam, mod=None):
    """ Factory method. From a Python module object and a string (cls_nam) return
    the Python class object corresponding to the class found in the given module.
    """
    ze_mod = mod or self.mod
    cls = ze_mod.__dict__.get(change_class_name(cls_nam))
    if cls is None:
      mod_file = ze_mod.__file__
      raise Exception(f"Class '{cls_nam}' not found in module '{mod_file}'!")
    return cls