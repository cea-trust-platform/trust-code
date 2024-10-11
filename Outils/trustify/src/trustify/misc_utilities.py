"""
Various utilities.
"""

################################################################
# Logger
################################################################
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

import logging
class CustomFormatter(logging.Formatter):
    format = "%(levelname)s - [%(filename)s:%(lineno)d] -- %(message)s"

    FORMATS = {
        logging.DEBUG: BLUE + format + END,
        logging.INFO: GREEN + format + END,
        logging.WARNING: YELLOW + format + END,
        logging.ERROR: RED + format + END,
        logging.CRITICAL: RED + format + END
    }

    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record)

def init_logger():
    import os
    logger = logging.getLogger("trustify")
    if os.getenv("TRUSTIFY_DEBUG"):
        logger.setLevel(logging.DEBUG)
    else:
        logger.setLevel(logging.INFO)
    handler = logging.StreamHandler()
    handler.setFormatter(CustomFormatter())
    logger.addHandler(handler)
    return logger

logger = init_logger()

################################################################
# Exception
################################################################
class TrustifyException(Exception):
    """ Raised whenever we encounter an error in the parsing process more tokens. """
    def __init__(self, msg="Error!"):
        Exception.__init__(self, msg)

################################################################
# Module dynamic import
################################################################

def import_generated_module(mod_path):
    """ Import a Python module given the path of the correpsonding py file.
    Returns the module object itself.
    """
    import os, sys

    # Import generated module:
    ze_dir, mod_nam = mod_path.parents[0], mod_path.stem
    prevPath = sys.path[:]
    logger.debug(f"Importing module '{mod_nam}' from path '{ze_dir}' ...")
    sys.path.append(str(ze_dir))
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
    from trustify.trust_parser import TRUSTParser, TRUSTStream

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

class BoolWithMsg:
    def __init__(self, ok, why=""):
        self.ok = ok
        self.why = why

def check_str_equality(s1, s2, print_on_diff=True):
    """
    Check equality between two (long) strings (with line returns) and produce a diff (as Linux 'diff') if not equal.
    @return a CheckBaseXyz object which has 'check.ok' attribute (boolean), and 'check.why' (string) as diff string.
    """
    import difflib as DIFF
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
            logger.error(why)
        return BoolWithMsg(False, why)
    return BoolWithMsg(True, "Strings are equal")

##################################################################
## Class factory to register and get back all generated classes
## be it pydantic classes (the ones from the data model) or Parser classes.
##################################################################
class ClassFactory:
    # TODO un peu le bazar cette classe ...

    _PARSER_SUFFIX = "_Parser"
    _ALL_CLASSES = {}
    _SYNO_ORIG_NAME = {}  # Dict providing for a synonym (like 'lire_med') the **list** of original names ('read_med')
                          # Why a list? Because some synonyms are duplicated! Example: 'negligeable' might refer to original
                          # class 'convection_negligeable' or 'diffusion_negligeable'
                          # See method ConstrainBase_Parser._ReadClassName() and ClassFactory.BuildSynonymMap()

    @classmethod
    def ToPydName(cls, nam):
        return nam.lower().capitalize()

    @classmethod
    def ToParserName(cls, nam):
        return cls.ToPydName(nam) + cls._PARSER_SUFFIX

    @classmethod
    def RegisterClass(cls, cls_nam, cls_obj):
        # print("reg", cls_nam, cls_obj)
        if cls_nam not in cls._ALL_CLASSES:
            cls._ALL_CLASSES[cls_nam] = cls_obj

    @classmethod
    def GetPydClassFromName(cls, cls_nam):
        """ Factory method. From a string (cls_nam) return the Python class object
        corresponding to the class found in the current module.
        """
        n = cls.ToPydName(cls_nam)
        cls = cls._ALL_CLASSES.get(n)
        if cls is None:
            raise TrustifyException(f"ClassFactory - pydantic class '{cls_nam}' not found!")
        return cls

    @classmethod
    def GetParserClassFromName(cls, cls_nam):
        """ Factory method. From a string (cls_nam) return the Python class object
        corresponding to the class found in the current module.
        """
        from trustify.base import Abstract_Parser
        n = cls.ToParserName(cls_nam)
        cls = cls._ALL_CLASSES.get(n)
        if cls is None:
            raise TrustifyException(f"ClassFactory - parser class '{n}' not found!")
        assert issubclass(cls, Abstract_Parser)
        return cls

    @classmethod
    def GetPydNamFromParserName(cls, pars_nam):
        assert pars_nam.endswith(cls._PARSER_SUFFIX), f"Class name not ending with {cls._PARSER_SUFFIX}"
        pyd_nam = pars_nam[:-len(cls._PARSER_SUFFIX)]
        return pyd_nam

    @classmethod
    def GetPydFromParser(cls, pars_cls):
        """ From a parser class, return the equivalent Pydantic schema class.
        For example from 'Interprete_Parser', returns 'Interprete' """
        import pydantic
        pyd_nam = cls.GetPydNamFromParserName(pars_cls.__name__)
        ret = ClassFactory.GetPydClassFromName(pyd_nam)
        assert issubclass(ret, pydantic.BaseModel), f"class '{ret.__class__.__name_}' is not a child of pydantic.BaseModel"
        return ret

    @classmethod
    def GetParserFromPyd(cls, pyd_cls):
        """ From a pydantic schema class, return the equivalent parser class.
        For example from 'Interprete', returns 'Interprete_Parser' """
        from trustify.base import Abstract_Parser
        ret = cls.GetParserClassFromName(pyd_cls.__name__)
        assert issubclass(ret, Abstract_Parser), f"class '{ret.__class__.__name_}' is not a child of Abstract_Parser"
        return ret

    @classmethod
    def Exist(cls, cls_nam):
        """ True if class with nam 'cls_nam' is registered """
        return (cls_nam in cls._ALL_CLASSES)

    @classmethod
    def GetAllConstrainBaseParser(cls):
        """ Get all registered children of ContrainBase_Parser """
        ret = []
        from trustify.base import ConstrainBase_Parser
        for c in cls._ALL_CLASSES.values():
            if issubclass(c, ConstrainBase_Parser):
                ret.append(c)
        return ret

    @classmethod
    def GetAllConstrainBasePyd(cls):
        """ Get all registered children of ContrainBase """
        ret = []
        obju = cls.GetPydClassFromName("Objet_u")
        for c in cls._ALL_CLASSES.values():
            if issubclass(c, obju):
                ret.append(c)
        return ret

    @classmethod
    def BuildSynonymMap(cls):
        """ Build multi-map from synonyms to root names.
        Motivating example: 'convection { negligeable } / diffusion { negligleable }'
        This must be called once all classes have been registered. See also ConstrainBase_Parser._SYNO_ORIG_NAME
        definition in base.py 
        """
        cls._SYNO_ORIG_NAME = {}
        for c in cls.GetAllConstrainBasePyd():
            for s in c._synonyms[None]:  # None is the key for the synonym of the class itself (not its attributes)
                cls._SYNO_ORIG_NAME.setdefault(s, []).append(c)

##################################################################
## Various unit tests utils
##################################################################

class UnitUtils:
    """
    Useful stuff for unit tests.
    """
    _TRUG = { }     # The list of generated modules, as Python module objects
    _TRAD2 = {"simple": "TRAD_2_adr_simple",   # a very reduced version of a complete TRAD_2 file
              "full"  : "TRAD2_full",          # Generated with trad2_utilities from TRUST sources, around v1.9.2 of TRUST
              "genepi": "TRAD_2_GENEPI_V17",   # From GENEPI
              "custom": "myTRAD2"
              }

    _NO_REGENERATE = False   # useful for debugging - avoid regenerating each time (this is a bit long with full TRAD2)

    def generate_python_and_import(self, slot):
        """ Generates Python files (Pydantic and parsing) from TRAD2 in the current test directory and stores
        imported module in self._TRUG slot.
        """
        import os
        import pathlib
        if not self._TRUG.get(slot) is None:
            return

        from trustify.trad2_pydantic import generate_pyd_and_pars

        tstdir = pathlib.Path(self._test_dir)
        file_in = tstdir / "trad2" / self._TRAD2[slot]
        file_nfo_in = tstdir / "trad2" / (self._TRAD2[slot] + ".nfo")
        if not file_nfo_in.exists():
            file_nfo_in = None
        file_pars = tstdir / "generated" /  (file_in.stem + '_pars.py')
        if not self._NO_REGENERATE:
            file_pyd = tstdir / "generated" /  (file_in.stem + '_pyd.py')
            generate_pyd_and_pars(file_in, file_nfo_in, file_pyd, file_pars)

        # Import generated module (IMPORTANT: set it at class level, not instance!!)
        self.__class__._TRUG[slot] = import_generated_module(file_pars)

        self.assertIsNotNone(self._TRUG[slot])
        self.assertEqual(self._TRUG[slot].__file__, str(file_pars))

#####################################
# Typing related helper methods
#####################################

def break_type(typ):
    """ For a type like 'Optional[Annotated[List[toto], "fixed_size"]]' :
    @return true if Optional, false otherwise
    @return a list of types, e.g. [list, toto] corresponding to the nesting of types provided and always as Python std types (not typing module)
    @return anything extra put as annotation if any (e.g. "fixed_size"), None otherwise
    """
    from typing import get_origin, get_args, Union, Annotated, Literal

    orig = get_origin(typ)
    # Non-'typing' type
    if orig is None:
        return False, [typ], None
    # Handle 'Optional[toto]' which is actually an alias for 'Union[toto, None]':
    opt = orig is Union
    if opt:
        args = get_args(typ)
        typ = args[0]
    orig = get_origin(typ)

    # Handle any annotation
    ann = None
    if orig is Annotated:
        typ, ann = get_args(typ)
        orig = get_origin(typ)

    # Break down nested type into a list:
    typs = []
    while orig is not None:
        typs.append(orig)
        args = get_args(typ)
        typ = args[0]
        orig = get_origin(typ)
    # Add most nested type (a builtin) or Literal arguments:
    if len(typs) and typs[-1] is Literal:
        typs.extend(args)
    else:
        typs.append(typ)

    return opt, typs, ann

def strip_optional(typ):
    from typing import get_origin, get_args, Union

    orig = get_origin(typ)
    # Non-'typing' type
    if orig is None:
        return typ

    if orig is Union:
        return get_args(typ)[0]
    return typ

