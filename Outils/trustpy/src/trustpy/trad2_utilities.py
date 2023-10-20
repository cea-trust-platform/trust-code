#!/usr/bin/env python
# -*- coding: UTF-8 -*-

""" Take a look at doc/README_dev.md

Module to handle parsing, loading and writing of
    - the XD tags found in the code
    - the TRAD2.org file
    - the loading/writing of the TRAD2 file

Classes TRAD2Attr and TRAD2Block represent pieces of the TRAD2 file.
Class TRAD2Content is the central piece.

Authors: A Bruneton, C Van Wambeke
"""
import os
import trustpy.trust_utilities as TRUU

_XD_TAG = " XD "
_XD_PARAM_TAG = " XD_ADD_P "

def genErr(fname, lineno, msg):
  """ Small handy function for error message generation """
  return TRUU.RED + f"\n\nFile '{fname}',\n   -> line #{lineno+1}, {msg}" + TRUU.END

def convertTyp(typ):
  """ Types declared in XD tags of cpp files might differ from what is officially supported in TRAD_2, so convert:
  """
  mp = {"int": "entier",
        "double": "floattant",
        "flag": "rien",
        "dico": "chaine(into=[])"}
  return mp.get(typ, typ)

class TRAD2Attr:
  """ An attribute of a block in the TRAD2 logic """
  def __init__(self):
    self.nam = ""
    self.typ = ""
    self.synos = []
    self.is_opt = True
    self.desc = ""
    self.info = ["", -1]  # Filename / Lineno

  @classmethod
  def BuildFromTab(cls, tab, fname, lineno, convert=False):
    """ Build from a single array representing a (splitted) line of the TRAD2 or a C++ XD comment
    """
    if len(tab) < 4:
      raise Exception(genErr(fname, lineno, "incomplete 'XD attr' (attribute line) instruction!!")) from None
    a = TRAD2Attr()
    nams, _, syno, opt = [t.lower() for t in tab[:4]]

    # For attributes, we accept '|' in both the first slot and the third one:
    a.nam = nams.split("|")[0]
    syno2 = set((nams+"|"+syno).split("|"))
    try:    syno2.remove(a.nam)
    except: pass
    a.synos = list(syno2)
    a.synos.sort()

    # Type must sometimes preserve upper case ... (chaine=into["X","Y"])
    if tab[1].startswith("chaine="):  a.typ = tab[1]
    else:                             a.typ = tab[1].lower()

    # Types are different if coming from TRAD2 of from C++ ...
    if convert:
      a.typ = convertTyp(a.typ)

    a.desc = ' '.join(tab[4:])
    if opt not in ["0", "1"]:
      raise Exception(genErr(fname, lineno, f"invalid optional flag in 'XD attr' (attribute line) instruction ('{opt}'). It should be 0 or 1!!")) from None
    a.opt = (opt == "1")
    a.info = [fname, lineno+1]
    return a

  def toTRAD2(self):
    """ Output the attribute in the 'TRAD2' format """
    opt = 1 if self.opt else 0
    if len(self.synos) == 0:
      synos = [self.nam]
    else:
      synos = self.synos
    synos_s = "|".join(synos)
    s = f"  attr {self.nam} {self.typ} {synos_s} {opt} {self.desc}\n"
    n = f"{self.info[0]},{self.info[1]}\n"
    return s, n

class TRAD2Block:
  """ A block describing a keyword in the TRAD2 file. """
  def __init__(self):
    self.nam = ""
    self.name_base = ""
    self.synos = []
    self.mode = -123
    self.desc = ""
    self.info = ["", -1]  # Filename / Lineno
    self.attrs = []  # A list of TRAD2Attr

  @classmethod
  def BuildFromTab(cls, tab, fname, lineno):
    """ Build from a single array representing a (splitted) line of the TRAD2 or a C++ XD comment
    """
    base_nam = tab[1].lower()
    if base_nam == "listobj": b = TRAD2BlockList()
    else:                     b = TRAD2Block()
    b.nam, b.name_base, nam2, acco_s = [t.lower() for t in tab[:4]]
    a = None
    try:
      a = int(acco_s)
      if a not in [-3,-2,-1,0,1]:a = None
    except:
      pass
    if a is None:
      raise Exception(genErr(fname, lineno, f"option for curly braces should be an integer in [-3:1], not '{acco_s}'!!")) from None
    b.mode = a
    b.synos = nam2.split("|")
    b.info = [fname, lineno+1]
    b._finishBuild(tab[4:])
    return b

  def _finishBuild(self, tab):
    self.desc = ' '.join(tab)

  def toTRAD2(self):
    """ Output the data in the 'TRAD2' format """
    synos = "|".join(self.synos)
    s = f"{self.nam} {self.name_base} {synos} {self.mode} {self.desc}\n"
    nfo = f"{self.info[0]},{self.info[1]}\n"
    for a in self.attrs:
      s2, nf2 = a.toTRAD2()
      s += s2
      nfo += nf2
    return s, nfo

class TRAD2BlockList(TRAD2Block):
  """ Specific class for 'listobj' entries describing a list.
  """
  def _finishBuild(self, tab):
    """ Override to extract list-relevant data """
    self.classtype = tab[0].lower()
    if tab[1] not in ["-1", "0", "1"]:
      raise Exception(genErr(self.info[0], self.info[1],
                      f"option for comma (for a list) should be -1, 0 or 1, not '{tab[1]}'!!")) from None
    self.comma = int(tab[1])
    self.desc = ' '.join(tab[2:])

  def toTRAD2(self):
    """ Output the data in the 'TRAD2' format - note there is no attribute """
    if len(self.attrs):
      raise Exception(genErr(self.info[0], self.info[1], f"list object description should not have any attribute!!")) from None
    synos = "|".join(self.synos)
    s = f"{self.nam} {self.name_base} {synos} {self.mode} {self.classtype} {self.comma} {self.desc}\n"
    nfo = f"{self.info[0]},{self.info[1]}\n"
    return s, nfo

class TRAD2Content:
  """ Main class - this is a memory version of a TRAD2 file. 
  A TRAD2Content is made of a list of TRAD2Block, each containing a list of TRAD2Attr.
  """
  def __init__(self):
    self.data = []  # A list of TRAD2Block

  @classmethod
  def BuildContentFromTRAD2(cls, trad2, trad2_nfo=None):
    """ Build content from a full TRAD2 file
    """
    res = TRAD2Content()
    with open(trad2) as f:
      lines = f.readlines()
    if not trad2_nfo is None:
      if not os.path.exists(trad2_nfo):
        TRUU.log_warning(f"Invalid INFO file '{trad2_nfo}' \n  -> File does not exist.")
        trad2_nfo = None
      else:
        with open(trad2_nfo) as f:
          lines_nfo = f.readlines()
          if len(lines_nfo) != len(lines):
            raise Exception(f"Invalid INFO file {trad2_nfo} \n  -> not the same number of line as the TRAD2!")
          TRUU.log_info(f"Loaded debug file for TRAD2 -> {trad2_nfo}")
    curr_obj = None
    for lin_n, l in enumerate(lines):
      if l.strip() == "": continue
      l = l.strip()
      tab0 = l.split(" ")
      tab = [t for t in tab0 if t.strip() != '']
      # Skip "ref ..." for now
      if len(tab) >= 2 and tab[0] == "ref":
        continue
      obj = cls._ParseXD(trad2, lin_n, tab, convert=False)
      # Override debug information if provided:
      if not trad2_nfo is None:
        obj.info = lines_nfo[lin_n].split(",")
        obj.info[1] = int(obj.info[1])
      if isinstance(obj, TRAD2Block):
        res.data.append(obj)
        curr_obj = obj
      else:  # attribute
        if curr_obj is None:
          raise Exception(genErr(trad2, lin_n, f"'XD attr' (attribute line) read before main 'XD' line (header line)!"))
        curr_obj.attrs.append(obj)
    return res

  @classmethod
  def BuildFromOrgAndSources(cls, trad2_org, src_dir):
    """ Build content from TRAD2.org file and from a set of C++ sources
    """
    if not os.path.exists(trad2_org):
        raise Exception(f"File '{trad2_org}' not found!")
    if not os.path.exists(src_dir):
        raise Exception(f"Directory '{src_dir}' not found!")
    ret = TRAD2Content()
    ret.data = []   # A list of TRAD2Blocks
    ret.synos = {}  # Synonyms, what a pain ...
    ret.scanSourceFiles(src_dir)
    ret.assemble(trad2_org)
    return ret

  def toTRAD2(self, f_nam_out):
    """ Write everything out in the 'TRAD2' format (and the corresponding .nfo file giving the origin of each line. """
    tot_s, tot_nfo = [], []
    for d in self.data:
      s, nfo = d.toTRAD2()
      tot_s.append(s)
      tot_nfo.append(nfo)
    with open(f_nam_out, "w") as f:
      f.write(''.join(tot_s))
    TRUU.log_info(f"==> Written file '{f_nam_out}'")
    with open(f_nam_out + ".nfo", "w") as f:
      f.write(''.join(tot_nfo))
    TRUU.log_info(f"==> Written file '{f_nam_out}.nfo'")

  def toJSon(self):
    """ This is the future ... :-) """
    pass

  def _parseMacro(self, tag, l):
    """ Generic method for parsing a 'implemente_instanciable' or 'add_synonym()' """
    if tag in l:
      t = l.split(",")
      cls_nam = t[0].split("(")[1].strip()
      v = t[1].split(")")[0].strip()
      s = v.replace('"', '')
      return cls_nam, s

  @classmethod
  def _ParseXD(cls, f_nam, lin_n, tab, convert=False):

    if len(tab) < 5:
      print(tab)
      raise Exception(genErr(f_nam, lin_n, "incomplete 'XD' instruction (header line)!!"))

    if tab[0] == "attr":
      # XD param tag: "// XD attr sutherland bloc_sutherland sutherland 1 Sutherland law for viscosity ..."
      attr = TRAD2Attr.BuildFromTab(tab[1:], f_nam, lin_n, convert=convert)
      return attr
    else:
      # XD main tag:  "// XD fluide_quasi_compressible fluide_dilatable_base fluide_quasi_compressible -1 Quasi-compressible flow with a low ...."
      blk = TRAD2Block.BuildFromTab(tab, f_nam, lin_n)
      return blk

  def _parseXD_ADD_something(self, tag="XD_ADD_P", expected_num_args=2, f_nam="", cpp_meth="ajouter",
                             lin_n=-1, tab=[]):
    """ Parse a line with XD_ADD_P or XD_ADD_DICO -> this implies extracing some 
        information from the C++ call: param.ajouter(...).
        Only used when parsing C++ stuff.
    """
    #  Example
    #   "param.ajouter_flag("P0", &alphaE_); // XD_ADD_P rien Pressure nodes"
    pos = tab.index(tag)
    assert pos >= 0
    if len(tab[pos+1:]) < expected_num_args:
      raise Exception(genErr(f_nam, lin_n, f"incomplete {tag} instruction!!"))
    typ = tab[pos+1].lower()
    if expected_num_args > 1:
      desc = ' '.join(tab[pos+2:])
    else:
      desc = typ
    # Parse C++ parameters passed to the ajouter_ method
    lin_start = ' '.join(tab[:pos])
    if not f".{cpp_meth}" in lin_start:
      raise Exception(genErr(f_nam, lin_n, f"{tag} instruction put on a line where 'Param::{cpp_meth}*' method is not called!!"))
    ob, cb = lin_start.find('('), lin_start.find(')')
    params = lin_start[ob+1:cb]
    tb = params.split(',')
    if len(tb) not in [2,3]:
      raise Exception(genErr(f_nam, lin_n, f"wrong number of arguments to Param::ajouter*() method??"))
    # Param name
    param_nam = tb[0].lower().strip().replace('"', '')
    # Required param?
    opt = "1"
    if len(tb) == 3 and "required" in tb[2].lower(): opt = "0"
    return param_nam, typ, opt, desc

  def _parseXD_ADD_P(self, f_nam, lin_n, tab):
    return self._parseXD_ADD_something(tag="XD_ADD_P", expected_num_args=2, cpp_meth="ajouter",
                                       f_nam=f_nam, lin_n=lin_n, tab=tab)

  def _parseXD_ADD_DICO(self, f_nam, lin_n, tab):
    return self._parseXD_ADD_something(tag="XD_ADD_DICO", expected_num_args=1, cpp_meth="dictionnaire",
                                       f_nam=f_nam, lin_n=lin_n, tab=tab)

  def scanOneCppFile(self, f_name):
    """ Scan one cpp file for the XD tags. Also extract the synonyms given by the 
    'add_synonym' macro.
    """
    # f_n_sh = os.path.split(f_name)[-1]  # Short name of the C++ file
    impl, res, curr_obj = {}, [], None
    #
    # Frist part, scan the file to extract XD stuff
    #
    TRUU.log_debug(f"File: {f_name}")
    with open(f_name, "r") as f:
      ll = f.readlines()

    for lin_n, lin in enumerate(ll):
      lin = lin.strip()
      l2 = lin.lower().strip()
      # For each possible instruction, an example is provided:

      #
      # The first two cases handle synonyms
      #
      if "implemente_instanciable" in l2:
        if lin.startswith("//"): continue
        # "Implemente_instanciable(Terme_Boussinesq_VEF_Face,"Boussinesq_VEF_P1NC",Terme_Boussinesq_base);"
        cls_nam, kw = self._parseMacro("implemente_instanciable", l2)
        kwt = kw.split("|")  # we might have several synos there already...
        impl[cls_nam] = kwt[0]
        for k in kwt[1:]:
          self.synos.setdefault(kwt[0], []).append(k)
      if "add_synonym(" in l2:
        #  "Add_synonym(Terme_Boussinesq_VEF_Face,"Boussinesq_temperature_VEF_Face");"
        if lin.startswith("//"): continue
        cls_nam, s = self._parseMacro("add_synonym", l2)
        if cls_nam not in impl:
          raise Exception(genErr(f_name, lin_n, f"'Add_synonym' macro used before 'Implemente_instanciable'"))
        kw = impl[cls_nam]
        self.synos.setdefault(kw, []).append(s)

      #
      # Now handle XD tags
      #
      if "XD" in lin:
        # Make sure this is a valid XD comment
        tab0 = lin.split(" ")
        tab = [t for t in tab0 if t.strip() != '']
        if "//XD" in tab or "//XD_ADD_P" in tab:
          raise Exception(genErr(f_name, lin_n, f"misformatted XD comment (should have space before XD ...)"))
        if "XD" in tab:
          pos = tab.index("XD")
          # Skip "XD ref ..." for now
          if len(tab[pos:]) >= 3 and tab[pos+1].lower() == "ref":
            continue
          obj = self._ParseXD(f_name, lin_n, tab[pos+1:], convert=True)
          if isinstance(obj, TRAD2Block):
            res.append(obj)
            curr_obj = obj
          else:  # attribute
            if curr_obj is None:
              raise Exception(genErr(f_name, lin_n, f"'XD attr' read before main 'XD' line!"))
            curr_obj.attrs.append(obj)
        #
        # Those last two are only found in C++ file:
        #
        if "XD_ADD_P" in tab:
          if curr_obj is None:
            raise Exception(genErr(f_name, lin_n, f"'XD attr' read before main 'XD' line!"))
          nam1, typ, opt, desc = self._parseXD_ADD_P(f_name, lin_n, tab)
          a = TRAD2Attr.BuildFromTab([nam1, typ, nam1, opt, desc], f_name, lin_n, convert=True)
          curr_obj.attrs.append(a)
        if "XD_ADD_DICO" in tab:
          if curr_obj is None:
            raise Exception(genErr(f_name, lin_n, f"'XD attr' read before main 'XD' line!"))
          nam1, _, _, _ = self._parseXD_ADD_DICO(f_name, lin_n, tab)
          # Ensure the last attr added in the block is a 'dico':
          last_attr = curr_obj.attrs[-1]
          typ = last_attr.typ
          if not typ.startswith("chaine(into=["):  # bof bof ...
            raise Exception(genErr(f_name, lin_n, "'XD_ADD_DICO' read, but no preceding 'XD_ADD_P dico ...' instruction found!!"))
          last_attr.typ = typ.replace(']', f'"{nam1}",]')
    return res

  def scanSourceFiles(self, src_dir):
    """ Scan all C++ source files from a root directory (typically $TRUST_ROOT/src) """
    import glob
    pattern = os.path.join(src_dir, "**", "*.cpp")
    g = glob.glob(pattern, recursive=True)
    self.synos = {}
    for f_nam in g:
      res = self.scanOneCppFile(f_nam)
      for d in res:
        TRUU.log_debug(d.toTRAD2()[0])
      self.data.extend(res)

  def assemble(self, trad2org):
    """
    Concatenate the loading of TRAD2.org and the results of the scan of the C++ files.
    At the same time, complete the second name (after the type) with the potential synonyms (TODO: review this?) 
    """
    # Complete synonym lookup so that it works in all directions:
    lkp = {}
    for k, v in self.synos.items():
      lkp[k] = k
      for vv in v:
        lkp[vv] = k
    TRUU.log_debug("Synonyms are " + str(self.synos))

    # Load TRAD2.org first ...
    torg = TRAD2Content.BuildContentFromTRAD2(trad2org, None)

    # ... append results of scanning the C++ ...
    self.data = torg.data + self.data

    #, and complete the synonyms for the entries found there at the same time
    # (typically 'scheme_euler_explicit')
    for d in self.data:
      nam1, nam2 = d.nam, d.synos[0]
      if nam1 in lkp or nam2 in lkp:
        # there are synonyms, put them with a '|' separator in the second slot after type:
        key = lkp[nam1] if nam1 in lkp else lkp[nam2]
        syn = set([key, nam2])
        syn.update(self.synos[key])
        try:    syn.remove(nam1)
        except: pass
        d.synos = list(syn)

###########################################################################
if __name__ == "__main__":
  import sys
  # TRUU._log_level = 4   # Turn this to 4 to debug!
  if len(sys.argv) > 1:
    outfile = sys.argv[1]
  else:
    outfile = "trustpy/test/myTRAD2"
  tr = os.environ.get("TRUST_ROOT", None)
  if tr is None:
    raise Exception("TRUST_ROOT environment variable is not defined!")
  trad2org = os.path.join(tr, "Outils", "TRIOXDATA", "XTriou", "TRAD_2.org")
  src = os.path.join(tr, "src")
  tg = TRAD2Content.BuildFromOrgAndSources(trad2org, src)
  tg.toTRAD2(outfile)
