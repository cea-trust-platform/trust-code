#!/usr/bin/env python
# -*- coding: UTF-8 -*-

"""
Contains also KISS colorized logger to get minimum log
instead packagespy logger (using import more complex logging)
"""

import pprint as PP
import inspect  # this_function_name = inspect.currentframe().f_code.co_name

class StoreAll(object):
  """used as dict to EZ store everything, with EZ print"""
  def __repr__(self):
    return "%s" % PP.pformat(self.__dict__)


_INFOS = StoreAll() # used as global to EZ store general variables

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

def indent_n(mess, nb=2):
  """indent lines but not first line"""
  ii = "\n" + " "*nb
  res = ii.join(mess.split('\n'))
  return res

def indent_n_all(mess, nb=2):
  """idem indent_n plus indent first line"""
  res = indent_n(mess, nb)
  ii = " "*nb
  return ii + res

def wrap_line(aStr, lmax=80, smart=True ):
  """
  wrap too long lines to words or '.'
  smart True as replace NL,NL2,\' etc
  """
  res = ""
  lg = 0
  tmp = aStr
  if smart:
    tmp = tmp.replace(" NL2 ", "\n\n")  # 2 '\n' is too much TODO?
    tmp = tmp.replace(" NL ", "\n")
    tmp = tmp.replace(r"\'" , "'")
  tmp = tmp.replace(". ", ".\n")
  for c in tmp:
    if c == "\n":
      lg = 0
    if lg > lmax and c == " ":
      res += " \n" # choose not remove existing whitespace
      lg = 0
    else:
      res += c
      lg += 1
  return res

def filter_docstring(aStr):
  """
  de-indent final string
  as supposed docstrings indentation
  """
  res = aStr.replace("\n  ", "\n")
  if res[0] == "\n" : res = res[1:]
  return res

def simplify_successive_blanks(aStr):
  """
  reduce all succesive blanks and new lines to one blank
  remove leadind/trailing blanks
  simplify_successive_blanks('  aa \n  bb   ') -> 'aa bb'
  """
  res = aStr.split()
  return " ".join(res)

PP_WIDTH = 128*3  # pformat(something, width=PP_WIDTH)

def PPP(v):  # as shortcut
  PP.pprint(v, width=PP_WIDTH)

def PPF(v):  # as shortcut
  return PP.pformat(v, width=PP_WIDTH)

def PPFL(v):  # as shortcut for list
  """
  avoid 'dict_keys([...])' or else to get '[...]'
  """
  return PP.pformat(list(v), width=PP_WIDTH)

def PPFC(v):  # as shortcut for list of classes names
  res = ["%4i %s" % (i, n.__name__) for i, n in enumerate(v)]
  return PP.pformat(res, width=PP_WIDTH)

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

def hash6(aStr):
  import hashlib

  res = hashlib.sha256(bytes(aStr,encoding="utf-8")).hexdigest()
  # print("hash|", type(res), res)
  return res[0:6]

def tabul(alist):
  tt = "\n    "
  return tt + tt.join(alist)

def to_comment(aStr):
  """append '# ' before all lines"""
  tt = "\n# "
  return tt + tt.join(aStr.split("\n"))

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

if __name__ == "__main__":
  print("INFO:    hello %s" % __file__)
  pass
