#!/usr/bin/env python
# -*- coding: UTF-8 -*-

"""
See doc/README_dev.md

This module holds all the logic to generate the Python code from the TRAD2 file.

Authors: A Bruneton, C Van Wambeke
"""

import os
import sys
import pprint as PP

_quiet = True

import trustpy.trust_utilities as TRUU
from trustpy.trust_utilities import PPF, PPFL, indent_n, indent_n_all, tabul, wrap_line, hash6, to_comment, caller_name, \
                            log_error, log_warning, log_info, log_debug, log_current_name, StoreAll

import trustpy.baseTru as BTRU

#######################################################
#### TrustModuleGeneratorHelper
#######################################################
class TrustModuleGeneratorHelper:
  """ Helper class to store TRUST classes to be written in the generated Python module """
  def __init__(self, gen_file="triou_packagespy_generated.py"):
    self._TRIOU_GENERATED_FILE = gen_file
    self._DONE = set()   # Set of already covered classes - helps identifying doublons
    self._PK = []
    self._FILE = []
    self._PK_CLASSES = []
    self._PK_TRIOU_TO_PACKAGESPY = []

  def get_packagespy_classes(self):
    res = self._PK_TRIOU_TO_PACKAGESPY
    TRUU.log_info("get_packagespy_classes\n%s" % res)
    return res

  def append_packagespy_classes(self, aClassName):
    if aClassName in self._PK_TRIOU_TO_PACKAGESPY:
      return
    TRUU.log_debug("append_packagespy_classes\n%s" % aClassName)
    self._PK_TRIOU_TO_PACKAGESPY.append(aClassName)

  def open_PK(self):
    namefile = self._TRIOU_GENERATED_FILE
    if len(self._PK) == 0:
      self._PK_CLASSES.clear()
      self._FILE.append(namefile)
      self._PK.append(open(namefile, "w"))
    else:
      TRUU.log_error("open_PK: file %s open yet" % self._FILE[0])

  def close_PK(self):
    self._PK.pop().close()
    TRUU.log_info("close %s" % self._FILE.pop())

  def PK_write(self, mess):
    self._PK[0].write(mess)

  def def_init(self, cname):
    """useless for now, maybe later"""
    res = """
  
  def __init__(self):
    super(%s, self).__init__()
    self.setIsCast(True)
    self._setAllAttributesList()
  """ % cname
    return res

  def set_default_ClassXyz(self):
    """
    append or modify specific direct classes if necessary
    _TO_PACKAGESPY['XString'] = 'StrXyz'  # example
    etc...
    """
    # self._TO_PACKAGESPY[''] = ''
    return

  def get_Triou2Packagespy_class(self, v):
    if len(self._PK_TRIOU_TO_PACKAGESPY) == 0:
      self.set_default_ClassXyz()

    typ = v.__class__.__name__
    try:
      res = self._TO_PACKAGESPY[typ]
      return res
    except Exception:
      TRUU.log_debug(TRUU.BOLD + "Triou to Packagespy for non elementary unknown class : %s" % typ)
      pass

    # assume XMulTypes etc specific stuff
    try:
      res = v.get_packagespy()
      if not ("Xyz" in res or _TRIOU_SUFFIX in res):
        res = res + _TRIOU_SUFFIX
      TRUU.log_debug(TRUU.BOLD + "END   Triou to Packagespy for class : %s -> %s" % (typ, res))
      return res
    except Exception as e:
      res = 'Todo' + _TRIOU_SUFFIX
      TRUU.log_error("%s\nTriou to Packagespy for non elementary class fixed as '%s'\n%s" % (e, res, v))
      return res

#######################################################
#### Trad2Handler
#######################################################
class Trad2Handler:
  """ Main class for operation on trad_2 file """

  def __init__(self, file_in="TRAD_2", file_nfo_in=None, file_out="trust_packagespy_generated.py"):
    self._index = 0
    self._automatic_classes = {}
    self._automatic_classes_debug = {}
    self._trad2_nam = file_in
    if file_nfo_in is None:
      self._file_nfo_in = file_in + ".nfo"
    self._gen_py_file = file_out
    # Helper class for module generation:
    self._trust_gen = TrustModuleGeneratorHelper(file_out)
    # See computeInherited() method:
    self._inherit_attr = {}

  def get_final_type(self, atype):
    finaltype = atype
    value = TRUU.StoreAll()
    value.atype = atype
    value.finaltyp = atype
    sha = None

    if atype.startswith("chaine") and not atype == "chaine":  # for example chaine(into=["trace","champ_frontiere"])
      sha = hash6(atype)
      # print("sha %s %s %s" % (atype, type(sha), sha))
      finaltype = "chaine_%s" % sha
      value.finaltyp = finaltype

    if atype.startswith("entier"):
      sha = hash6(atype)
      # print("sha %s %s %s" % (atype, type(sha), sha))
      finaltype = "entier_%s" % sha
      value.finaltyp = finaltype

    if atype.startswith("floattant"):
      sha = hash6(atype)
      # print("sha %s %s %s" % (atype, type(sha), sha))
      finaltype = "floattant_%s" % sha
      value.finaltyp = finaltype

    if finaltype in self._automatic_classes:
      if sha is not None:
        if sha in self._automatic_classes_debug:
          # this class (as attribute) is defined yet previouly in another class
          self._automatic_classes_debug[sha].append(value)
        else:
          self._automatic_classes_debug[sha] = [value]
    else:
      log_debug("adding automatic class: %s" % value.finaltyp)
      self._automatic_classes[value.finaltyp] = value

    return value

  #############################################################################
  def generate_packagespy_code(self, data):
    """
    @param data : a list of DataTrad2 objects
    """
    from trustpy.trad2_utilities import TRAD2BlockList
    log_debug("trad2xyz %i self.generate_packagespy_code\n%s" % (self._index, data.toTRAD2()[0]))
    self._index = self._index + 1
    if isinstance(data, TRAD2BlockList):
      ares = self.generate_packagespy_code_ListOf(data)
    else:
      ares = self.generate_packagespy_code_ConstrainBase(data)
    return ares

  #############################################################################
  def generate_packagespy_code_ListOf(self, data):
    """ 
    Generate set of classes which inherit from the base classes defined for TRUST and packagepy,
    e.g. ListOfBase_Tru, which itself inherits from ListOfBaseXyz
    """
    titclass = "\nclass %s%s(ListOfBase_Tru): #####\n" % (data.nam, BTRU._TRIOU_SUFFIX)
    res = ""
    tmp = data.desc
    res += '"""\n' + wrap_line(tmp) + '\n"""'
    res += "\n# From: line %i, in file '%s'" % (data.info[1], data.info[0])

    # append brace mode
    res += "\n_braces = %i" % data.mode

    # append mode
    res += "\n_comma = %i" % data.comma
    res += '\n_allowedClasses = ["%s_Tru"]' % data.classtype
    # append synonym if any,
    # avoid registering 'nul' which is typically set for 'objet_lecture' children, corresponding to raw blocks
    # between '{' '}' - those blocks are never named, so the synonym makes no sense.
    if data.synos[0] != data.nam and data.synos[0] != "nul":
      res += "\n_synonyms = ['%s_Tru']" % data.synos[0]
    else:
      res += "\n_synonyms = []"

    # Debug information for nice error display:
    res += "\n_infoMain = %s" % PP.pformat(data.info)

    res = titclass + indent_n_all(res, 2)
    res += "\n##### end %s_Tru class\n" % data.nam
    ares = StoreAll()
    ares.code = res
    ares.name = data.nam
    ares.depends = []
    return ares

  def totry(self, msg):
    """
    just for debug Problem class definition order
    """
    if True:
      return msg   # skip this fonctionality
    else:
      res = "\ntry:"
      res += "\n  " + msg[1:]
      res += "\nexcept Exception as e:"
      res += '\n  print("Problem class definition unknown YET : %s" % e)'
      return res

  #############################################################################
  def generate_packagespy_code_ConstrainBase(self, data):
    """
    Generate set of classes which will inherit from the base classes defined in packagepy, 
    e.g. ConstrainBase_Tru, which itself inherits from ConstrainBaseXyz
    """
    titclass = "\nclass %s%s(TO_DO_inheritage): #####\n" % (data.nam, BTRU._TRIOU_SUFFIX)
    tmp = data.desc
    res  = 'r"""\n' + wrap_line(tmp) + '\n"""'
    res += "\n# From: line %i, in file '%s'" % (data.info[1], data.info[0])

    # append mode (with or without braces)
    res += "\n_braces = %i" % data.mode
    # append potential read_type override (see also Base_common_Tru._read_type and trust_hacks.py)
    if self.isBaseOrDeriv(data.nam):
      res += "\n_read_type = True  # This class and its children needs to read their type when parsed in the dataset!"
    # append synonym if any
    # avoid registering 'nul' which is typically set for 'objet_lecture' children, corresponding to raw blocks
    # between '{' '}' - those blocks are never named, so the synonym makes no sense.
    if len(data.synos) and data.synos[0] != data.nam and data.synos[0] != "nul":
      lst_syno = [f"{t}_Tru" for t in data.synos]
      res += "\n_synonyms = %s" % PP.pformat(lst_syno)
    else:
      res += "\n_synonyms = []"

    aHelp, tmp_syno, nfo_dic = {}, {}, {}
    depends, opt_lst, suppr = [], [], []
    tmp = ""
    for a in data.attrs:
      aname = a.nam
      #
      # Handling lambda!! It can not stay as Python attribute ...
      #
      if aname == "lambda":
        aname = "Lambda"
      ##
      if a.typ == "suppress_param":
        suppr.append(aname)
        continue
      value = self.get_final_type(a.typ) # one attribute branch or leaf
      # append attribute informations about parent-in-tree class
      value.fromclass = data.nam
      value.aname = aname
      if self._inherit_attr.get(value.finaltyp, False):
        finaltyp = "<inherited>" + value.finaltyp
      else:
        finaltyp = value.finaltyp
      finaltyp += BTRU._TRIOU_SUFFIX
      tmp += "  ('%s', '%s'),   #\n" % (aname, finaltyp)
      tmp1 = "(%s) %s" % (a.typ, a.desc)
      aHelp[aname] = (wrap_line(tmp1), "")
      # Fills list of synonyms if they are different from main name:
      lst_syno = a.synos
      if len(lst_syno) > 0:
        tmp_syno[aname] = lst_syno
      # Handle optionality
      try:
        if a.opt == 1:
          opt_lst.append(aname)
      except:
        pass
      # Handle debug info
      nfo_dic[aname] = a.info
    ## end for attr loop

    if tmp != "":
      tmp = "\n_attributesList = [\n" + tmp + "]"
    else:
      tmp = "\n_attributesList = []"
    if len(tmp_syno):
      tmps = PP.pformat(tmp_syno)[1:-1]
      tmps = indent_n_all(tmps, 1)
      tmp_syno = "\n_attributesSynonyms = {\n %s\n}" % tmps
    else:
      tmp_syno = "\n_attributesSynonyms = {}"
    tmp_sup = "\n_suppressed = %s" % PP.pformat(suppr)
    res += tmp
    res += tmp_sup
    res += tmp_syno
    res += "\n_optionals = set(%s)" % PP.pformat(opt_lst)

    # Generate origin information (for debug when an exception is raised)
    res += "\n_infoMain = %s" % PP.pformat(data.info)
    res += "\n_infoAttr = %s" % PP.pformat(nfo_dic)

    inheritage = None
    if data.name_base not in ["listobj", "listobj_impl", "objet_u"]:
      depends.append(data.name_base)
      inheritage = data.name_base
      # append parent inheritage _attributesList
      tmp = f"\n_attributesList = check_append_list('{data.nam}_Tru', _attributesList, {data.name_base}_Tru._attributesList, _suppressed)"
      res += tmp
      # append parent inheritage _attributesSynonyms
      tmp = f"\n_attributesSynonyms = check_append_dict('{data.nam}_Tru', _attributesSynonyms, {data.name_base}_Tru._attributesSynonyms)"
      res += tmp
      # append parent inheritage _infoAttr
      tmp = f"\n_infoAttr = check_append_dict('{data.nam}_Tru', _infoAttr, {data.name_base}_Tru._infoAttr)"
      res += tmp
      # append parent inheritage _optionals
      tmp = f"\n_optionals.update({data.name_base}_Tru._optionals)"
      res += tmp

    # generate help:
    if len(aHelp.keys()) > 0:
      tmp = "\n_helpDict = {\n"
      tmp1 = ""
      for k, v in aHelp.items():
        tmp2 = PP.pformat(v, width=100)
        tmp1 += "'%s':\n%s,\n" % (k, indent_n_all(tmp2, 6))
      # tmp1 = PP.pformat(aHelp, width=120)[1:-1]
      tmp += indent_n_all(tmp1, 2)[0:-2]
      tmp += "}"
    else:
      tmp = "\n_helpDict = {}"
    res += tmp

    # append mere inheritage_helpDict
    if inheritage is not None:
      # tmp = "\n_helpDict += %s_Tru._helpDict" % data.name_base
      # tmp = "\n_helpDict = {**_helpDict, **%s_Tru._helpDict}" % data.name_base
      tmp = "\n_helpDict = check_append_dict('%s_Tru', _helpDict, %s_Tru._helpDict)" % (data.nam, data.name_base)
      res +=tmp

    if inheritage is not None:
      titclass = titclass.replace('TO_DO_inheritage', inheritage + BTRU._TRIOU_SUFFIX)
    else:
      titclass = titclass.replace('TO_DO_inheritage', "ConstrainBase_Tru")
    res = titclass + indent_n_all(res, 2)

    res += "\n##### end %s_Tru class\n" % data.nam

    ares = StoreAll()
    ares.code = res
    ares.name = data.nam
    ares.depends = depends
    return ares

  def generate_packagespy_code_automatic(self):
    log_debug("starting generate_packagespy_code_automatic %s" % str(list(self._automatic_classes.keys())))
    i = 0
    self._trust_gen.PK_write("\n\n##### automatic classes\n")
    self._automatic_classes_REAL = {}
    for k, v in self._automatic_classes.items():
      if k in self._trust_gen._PK_CLASSES:  # done yet...
        log_debug(f"class '{k}' done yet, no automatic")
        continue
      if k + BTRU._TRIOU_SUFFIX in self.cls_from_btru:
        log_info(f"class '{k}' already defined in baseTru.py, no automatic generation")
        continue
      tmp = self.get_packagespy_code_automatic(k, v, i)
      self._trust_gen.PK_write(tmp)
      self._automatic_classes_REAL[k] = v
      i += 1
      self._automatic_classes = self._automatic_classes_REAL
    return

  def get_into(self, atype):
    # print("get_into atype '%s'" % atype)
    spl = atype.split("into=")
    if len(spl) == 1:
      return ""
    else:
      res = spl[1]
      res = res.split("]")
      return res[0] + "]"

  def get_into_int(self, atype):
    # print("get_into_int atype '%s'" % atype)
    spl = atype.split("into=")
    if len(spl) == 1:
      return ""
    else:
      res = spl[1]
      res = res.split("]")
      res = res[0] + "]"
      res = res.replace('"', '')
      res = res.replace("'", '')
      return res

  def get_max_int(self, atype):
    # print("get_max atype '%s'" % atype)
    spl = atype.split("max=")
    if len(spl) == 1:
      return ""
    else:
      res = spl[1]
      res = res.split(")")
      return "[0, %i]" % int(res[0])

  def get_packagespy_code_automatic(self, key, v, i):
    atype = v.atype
    inh = ""
    clsattr = ""
    if atype.startswith("chaine"):
      inh = "BaseChaine_Tru"
      clsattr = self.get_into(v.atype)
      if clsattr != "":
        inh = "BaseChaineInList_Tru"
        clsattr = "\n_allowedList = " + clsattr
        clsattr += "\n_defaultValue = _allowedList[0]"

    elif atype.startswith("entier"):
      inh = "BaseEntier_Tru"
      clsattr = self.get_into_int(v.atype)
      if clsattr != "":
        inh = "BaseEntierInList_Tru"
        clsattr = "\n_allowedList = " + clsattr
        clsattr += "\n_defaultValue = _allowedList[0]"
      else:
        clsattr = self.get_max_int(v.atype)
        if clsattr != "":
          inh = "BaseEntierInRange_Tru"
          clsattr = "\n_allowedRange = " + clsattr
          clsattr += "\n_defaultValue = _allowedRange[0]"

    elif atype.startswith("ref_"):
      # ref_* classes - by default map them to a string
      # [ABN] TODO we could elaborate and check for example that a 'ref_domaine' actually points
      # to a properly defined 'domain' object in the dataset ...
      inh = "BaseChaine_Tru"

    else:
      # Standard mappings:
      mp = {"floattant":   "BaseFloattant_Tru",
            "rien":        "Rien_Tru",
            "listchaine":  "ListOfChaine_Tru",
            "listchainef": "ListOfChaineDim_Tru",
            "list":        "ListOfFloat_Tru",
            "listf":       "ListOfFloatDim_Tru",
            "listentier":  "ListOfInt_Tru",
            "listentierf": "ListOfIntDim_Tru"
            }
      inh = mp.get(atype, "")

    if inh == "":
      msg = 'TOFIX no heritage for %s.%s -> %s' % (v.fromclass, v.aname, v.atype)
      log_warning(msg) # may be something TODO with Adrien
      # if v.atype + BTRU._TRIOU_SUFFIX in BTRU.trust_class_names:
      # else:
      #   log_error(msg) # sure something TODO with Adrien
      inh = "Todo" + BTRU._TRIOU_SUFFIX
      clsattr = "\n_defaultValue = '%s'" % msg

    res = "\nclass %s_Tru(%s):" % (key, inh)
    res +='\n  """\n  automatic class %i key %s from %s.%s ->\n  %s\n  """' % (i, key, v.fromclass, v.aname, v.atype)
    res += indent_n_all(clsattr, 2)
    res += "\n  pass"
    res += "\n##### end %s_Tru class\n" % key

    return res

  #############################################################################
  def check_dependencies(self, storeLoop):
    res = []
    depth = 0 # recurse depht
    for name in list(storeLoop.keys()):
      if name in self.cls_from_btru: continue
      depthseq = ""
      self.check_dependencies_recurse(storeLoop, name, depth, depthseq, res)
    return res

  def check_dependencies_recurse(self, storeLoop, name, depth, depthseq, res):
    # All classes already defined in baseTru are OK:
    if name + BTRU._TRIOU_SUFFIX in self.cls_from_btru:
      return
    depthseq += name + " -> "
    if depth >= 5:
      log_error("check_dependencies recurse depht > %i:\n%s" % (depth, depthseq))
      return
    try:
      ares = storeLoop[name]
      ok = True
    except Exception as e:
      msg = PP.pformat(sorted(list(storeLoop.keys())))
      log_error("check_dependencies search %s unknown existing items\n%s" % (name, msg))
      ok = False
    if not ok:
      raise Exception("check_dependencies unknown item name '%s'" % name)

    if ares.done is True:
      return
    # check dependencies
    for namedepend in ares.depends:
      self.check_dependencies_recurse(storeLoop, namedepend, depth+1, depthseq, res)
    res.append(name)
    ares.done = True
    return

  def isBaseOrDeriv(self, cls_nam):
    if cls_nam.endswith("_deriv") or cls_nam.endswith("_base") or cls_nam == "class_generic":
      return True
    return False

  def computeInherited(self, res):
    """ Fills the member self._inherit_attr indicating for each class name whether
    it should be marked as '<inherited>' when used as an attribute of a keyword
    """
    self._inherit_attr = {"objet_u": False,
                          "listobj": False,
                          "listobj_impl": False}
    # res is a list of TRAD2Block - convert this to a dict
    resd = {}
    for d in res: resd[d.nam] = d

    # Helper recursive function:
    def _is_mother_inh(data):
      if data.name_base in self._inherit_attr:
        return self._inherit_attr[data.name_base]
      else:
        try:
          return _is_mother_inh(resd[data.name_base])
        except KeyError as e:
          log_error(f"Could not get inheritance for class '{data.nam}'!")
          log_error(f"XD tags say it inherits from '{data.name_base}' but I can't find this one!")
          sys.exit(-1)

    for data in res:
      nam = data.nam
      # Follow the class hierarchy as long as the information is not registered in _inherit_attr:
      if nam in self._inherit_attr: continue
      if self.isBaseOrDeriv(nam):
        self._inherit_attr[nam] = True
        continue
      self._inherit_attr[nam] = _is_mother_inh(data)

  def trad2xyz(self, only_names=None, quiet=False):
    """
    Main entry point. 
    From reading TRAD_2_cv (as renamed TRAD_2 from trust data classes)
    generates the Python module (e.g. trust_packagespy_generated.py) which will represent the data model described by
    the TRAD_2 entries.
    """
    ok = False
    _quiet = quiet

    log_current_name("BEGIN")
    mod_gen = self._trust_gen
    mod_gen.open_PK()

    tmp = '''#!/usr/bin/env python
# -*- coding: UTF-8 -*-

"""
Automatic generation with method 'trad2xy' of file
  %s
From initial file definition from TRUST compilation
  %s
  ->
  %s
"""

from trustpy.baseTru import *          # ConstrainBase_Tru, BaseChaine_Tru, BaseEntier_Tru, BaseFloattant_Tru etc
from xyzpy.intFloatListXyz import *    # StrXyz, StrInList,IntXyz, FloatXyz, TodoXyz ...
from trustpy.trust_utilities import *

  ''' % (os.path.realpath(__file__), os.path.realpath(self._trad2_nam), os.path.realpath(self._gen_py_file))
    mod_gen.PK_write(tmp)

    # Generates the TRAD2Content object containing the data from the TRAD_2:
    from trustpy.trad2_utilities import TRAD2Content
    res = TRAD2Content.BuildContentFromTRAD2(self._trad2_nam, trad2_nfo=self._file_nfo_in)

    # Generates inheritance list for each class (since we need to spot class
    # deriving from '_base', '_deriv' or 'class_generic' to mark them as '<inherited>' in the attribute list
    self.computeInherited(res.data)

    storeLoop = {}
    self.cls_from_btru = set([c.__name__ for c in BTRU.packagespy_classes])
    for data in res.data:
      nam = data.nam
      if nam in mod_gen._DONE:
        log_warning(f"Class '{nam}' is duplicated in TRAD2 - please fix!!")
        continue
      # generate packagespy code if class not already defined in baseTru
      if nam + BTRU._TRIOU_SUFFIX in self.cls_from_btru:
        log_info(f"class '{nam}' already defined in baseTru.py - skipping")
        continue
      ares = self.generate_packagespy_code(data)
      ares.done = False    # used in check_dependencies
      storeLoop[ares.name] = ares

      mod_gen._DONE.add(nam)
      mod_gen._PK_CLASSES.append(nam)

    # manage file write order by dependencies
    ordernames = self.check_dependencies(storeLoop)

    for name in ordernames:
      ares = storeLoop[name]
      mod_gen.PK_write(ares.code)

    # write self._automatic_classes useful class
    self.generate_packagespy_code_automatic()

    tmp = sorted([nam + BTRU._TRIOU_SUFFIX for nam in mod_gen._PK_CLASSES])

    tmp.append("########## self._automatic_classes")
    for k in self._automatic_classes:
      tmp.append(k + BTRU._TRIOU_SUFFIX)

    #if False:   # do tha non implicit, to avoid mystery 'appendXyzClass class known yet', use BTRU.initXyzClasses() if miss
    #  # Make class from BTRU visible:
    #  tmp.append("########## make classes from baseTru visible:")
    #  for c in BTRU.packagespy_classes:
    #   if c not in mod_gen._PK_CLASSES and c not in self._automatic_classes:
    #     tmp.append(c.__name__)

    mod_gen.PK_write('''
    
def initXyzClasses():
  """case of CLFX.reset() done may be useful to redo appendAllXyzClasses"""
  if "%s" not in CLFX.getAllXyzClassesNames():
    CLFX.appendAllXyzClasses(packagespy_classes)

# useful for future import on this file
packagespy_classes = [
  %s
]

# needs explicit class factory appends for *_generated_*.py, as precaution
# avoid automatic as implicit
# initXyzClasses()  # avoid this automatic as implicit

  ''' % (tmp[0], ",\n  ".join(tmp)))

    mod_gen.PK_write("\n# number of classes: %s" % len(mod_gen._PK_CLASSES))
    mod_gen.PK_write("\n# number of automatic classes: %s" % len(self._automatic_classes))
    mod_gen.PK_write("\n# number of automatic identical classes: %s" % len(self._automatic_classes_debug))
    log_info("number of classes: %s" % len(mod_gen._PK_CLASSES))
    log_info("number of automatic classes: %s" % len(self._automatic_classes))
    log_info("number of automatic identical classes: %s" % len(self._automatic_classes_debug))

    hack_file = os.path.abspath(os.path.dirname(__file__)) + "/trust_hacks.py"
    mod_gen.PK_write(f"""

###################### HACKS ####################################
# Those hacks can be amended editing the file
# {hack_file}
# which is simply appended here
#################################################################

""")
    with open(hack_file) as f:
      mod_gen.PK_write(f.read())

    mod_gen.close_PK()
    ok = True
    return ok

###########################################################################
if __name__ == "__main__":
    import sys, os
    if len(sys.argv) <= 2:
      print("Usage: ./trad2_utilities.py <TRAD2_path> <path_for_generated_module.py>")
      sys.exit(-1)
    # Turn down log level a bit:
    TRUU._log_level = 2
    fout = sys.argv[2]
    th = Trad2Handler(file_in=sys.argv[1], file_out=fout)
    th.trad2xyz()
    print("Generation OK -> %s" % fout)
