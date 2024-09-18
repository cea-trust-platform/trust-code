"""
Various utilities.
"""

################################################################
# Logger
################################################################
import logging
class CustomFormatter(logging.Formatter):
    grey = "\x1b[38;20m"
    yellow = "\x1b[33;20m"
    red = "\x1b[31;20m"
    blue = "\x1b[0;36m"
    green = "\x1b[0;32m"
    bold_red = "\x1b[31;1m"
    reset = "\x1b[0m"
    format = "%(levelname)s - [%(filename)s:%(lineno)d] -- %(message)s"

    FORMATS = {
        logging.DEBUG: blue + format + reset,
        logging.INFO: green + format + reset,
        logging.WARNING: yellow + format + reset,
        logging.ERROR: red + format + reset,
        logging.CRITICAL: bold_red + format + reset
    }

    def format(self, record):
        log_fmt = self.FORMATS.get(record.levelno)
        formatter = logging.Formatter(log_fmt)
        return formatter.format(record)

def init_logger():
    logger = logging.getLogger("trustify")
    logger.setLevel(logging.INFO)
    handler = logging.StreamHandler()
    handler.setFormatter(CustomFormatter())
    logger.addHandler(handler)
    return logger

logger = init_logger()

################################################################
# Module dynamic import
################################################################

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
            log_error(why)
        return BoolWithMsg(False, why)
    return BoolWithMsg(True, "Strings are equal")


def change_class_name(s):
    return s.capitalize()

##################################################################
## Class factory to register and get back all generated classes
##################################################################
class ClassFactory:
    _ALL_CLASSES = {}

    @classmethod
    def RegisterClass(cls, cls_nam, cls_obj):
        if cls_nam not in cls._ALL_CLASSES:
            cls._ALL_CLASSES[cls_nam] = cls_obj

    @classmethod
    def GetClassFromName(cls, cls_nam):
        """ Factory method. From a string (cls_nam) return the Python class object
        corresponding to the class found in the current module.
        """
        n = change_class_name(cls_nam)
        cls = cls._ALL_CLASSES.get(n)
        if cls is None:
            raise Exception(f"Class '{cls_nam}' not found!")
        return cls

    @classmethod
    def Exist(cls, cls_nam):
        """ True if class with nam 'cls_nam' is registered """
        n = change_class_name(cls_nam)
        return (n in cls._ALL_CLASSES)

##################################################################
## Various unit tests utils
##################################################################

class UnitUtils:
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

    _do_not_regenerate = False   # useful for debugging - avoid regenerating each time (this is a bit long with full TRAD2)

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
