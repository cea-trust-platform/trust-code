#!/usr/bin/env python
# -*- coding: utf-8 -*-

import os
import os.path as OP
import sys
import inspect
from io import StringIO  # Python 3
import pprint as PP

import trustpy.trust_utilities as TRUU

verbose = False

class UnittestCustomMethodsTrust():
  """
  methods for inheritage in unittest TestCase class in trustpy test files
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

  def _TRUG_clear(self):
    """to be clear..."""
    self._TRUG.clear()
    self._TRUG.append(None)
    self._TRUG.append(None)

  def redirect_stdout(self):
    """
    redirect logging trace to StringIO
    situation have to be restored calling restore_stdout()
    to shunt verbosity code in test
    EZ use 'def test_000' as 'SetUp' with 'redirect_stdout'
    EZ use 'def test_999' as 'TearDown' with 'restore_stdout'
    """
    temp_out = StringIO()  # Create the in-memory "file"
    sys.stdout = temp_out  # Replace default stdout (terminal) with our stream
    self._stdout_stream.append(temp_out)

  def restore_stdout(self):
    """
    The original sys.stdout is kept in a special dunder named sys.__stdout__.
    So this restore the original output stream to the terminal.
    to shunt verbosity code in test
    EZ use 'def test_000' as 'SetUp' with 'redirect_stdout'
    EZ use 'def test_999' as 'TearDown' with 'restore_stdout'
    """
    try:

      temp_out = self._stdout_stream.pop()
    except:
      temp_out = sys.__stdout__

    if temp_out != sys.__stdout__: # supposedly a StringIO
      sys.stdout = sys.__stdout__
      callerframerecord = inspect.stack()[1]
      filename = OP.basename(callerframerecord.filename)
      # filename = [OP.basename(i.filename) for i in inspect.stack()]
      name_stdout = "%s.log" % OP.splitext(OP.basename(filename))[0]
      with open(name_stdout, "w") as f:
        f.write(temp_out.getvalue())
      # print("\nEND tests, log in file %s" % (name_stdout))
    return

  def print_begin_test(self):
    print("\nBEGIN %s" % inspect.currentframe().f_back.f_code.co_name)

  def caller_file_name(self):
    callerframerecord = inspect.stack()[1]
    filename = OP.basename(callerframerecord.filename)
    return filename

  def caller_method_name(self):
    callerframerecord = inspect.stack()[1]
    frame = callerframerecord[0]
    name = frame.f_code.co_name
    return name

  def caller_name_lineno(self):
    """
    caller file name and line numero
    returns 'file test_500_Triou_All.py caller test_999 lineno 79' for example
    """
    callerframerecord = inspect.stack()[1]
    filename = OP.basename(callerframerecord.filename)
    frame = callerframerecord[0]
    info = inspect.getframeinfo(frame)
    lineno = info.lineno
    name = frame.f_code.co_name
    res = "file %s caller %s lineno %i" % (filename, name, lineno)
    return res

  def get_import_paths(self):
    """to debug failing import"""
    res = "current dir %s\n" % os.getenv('PWD')
    tmp = os.getenv('PATH', "")
    res += "os.path \n%s\n" % PP.pformat(tmp.split(":"))
    tmp = sys.path
    res += "sys.path \n%s\n" % PP.pformat(tmp)
    tmp = os.getenv('PYTHONPATH', "")
    res += "PYTHONPATH \n%s\n" % PP.pformat(tmp.split(":"))
    return res

  def append_path(self, aDir):
    """to avoid failing import"""
    old = [i for i in sys.path]
    sys.path.append(aDir)
    return old

  def restore_path(self, aDir):
    """to reset previous append_path method"""
    if sys.path[-1] == aDir:
      sys.path.pop(-1)
    else:
      raise Exception("unexpected restore trailing path %s" % aDir)
    return

  def show_launch_timer(self, widget, timeout=None, verbose=False):
    """
    timer for test widgets PyQt5
    assure widget.show() and salomepy.onceQApplication singleton before
    timeout in seconds
    """
    from salomepy.onceQApplication import OnceQApplication
    from PyQt5.QtCore import QTimer

    app = OnceQApplication()
    timer = QTimer()
    timer.timeout.connect(widget.close)
    widget.show()
    if self._setTimer:  # timer in milliseconds
      if timeout is None:
        timer.start(self._timeout*1000)
      else:
        if verbose: print("show_launch_timer timeout %i" % timeout)
        timer.start(int(timeout*1000))
    app.exec_()

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
      if verbose: print("generated file %s" % pname)
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

  def display_treeview(self, model, title="display_treeview", timeout=60):
    import xyzpy.guiXyz.model_view_default as MVD
    # model._setAllAttributesList()
    # model.resetExpectedItems()
    # duplicate model if subtree
    newModel = model.duplicate()
    # item._parentAsAttribute = None  # better for get root in parent tree, and modifications
    controller = MVD.create_controller(newModel)
    view = MVD.create_treeview(controller, title=title)
    self.show_launch_timer(view, timeout=timeout)

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
        TRUU.log_error(why)
      return CheckBaseXyz(False, why)
    return CheckBaseXyz(True, "Strings are equal")

