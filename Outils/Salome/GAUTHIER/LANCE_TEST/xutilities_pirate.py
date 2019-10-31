# --
# Copyright (C) CEA, EDF
# Author : Erwan ADAM (CEA)
# --

# --
# Define myissubclass function to improve
# the default python one i.e. work :
#  o when arg 1 is not a class
#  o with more than one class to be tested
#

def myissubclass(sub_c, *classes):
    for c in classes:
        try:
            issub = issubclass(sub_c, c)
        except TypeError:
            issub = 0
            pass
        if issub: return 1
        pass
    return 0

# --
#

str_ini = str

def str(object):
    try:
        return str_ini(object)
    except TypeError:
        return "%s object"%(object.__class__.__name__)
    return "???"

# --
#

_isclt = None

def isClt():
    global _isclt
    if _isclt is not None:
        return _isclt
    from xcontext import getContext
    _context = getContext()
    _isclt = _context == "clt"
    return _isclt

# --
#

_verbose = None

def verbose():
    global _verbose
    # verbose has already been called
    if _verbose is not None:
        return _verbose
    # first time
    try:
        from os import getenv
        _verbose = int(getenv('XDATA_VERBOSE'))
    except:
        _verbose = 0
        pass
    #
    return _verbose

def setVerbose(level):
    global _verbose
    _verbose = level
    return

# --
# It seems that inspect is not thread-safe
# and there is a problem when importing stack
# in message function in a thread (which occur
# when importing a python file from gui)
# Idem for os

from inspect import currentframe
from os import system

# --
#

def message(*args, **kwargs):
    # E.A. very strange bug when using with salome !!!
    # E.A. verbose is None in some cases !!???
    try:
        level = verbose()
    except:
        level = 0
        pass
    # print msgs only in verbose mode
    if level == 0 : return
    # verbose level min
    try:
        level_min = kwargs["verbose_level_min"]
    except KeyError:
        level_min = 1
        pass
    if level < level_min : return
    # cls
    try:
        cls = kwargs["cls"]
    except KeyError:
        cls = None
        pass
    if hasattr(cls, '__name__'):
        cls = cls.__name__
        pass
    # clt
    try:
        clt = kwargs["clt"]
    except KeyError:
        clt = None
        pass
    #
    if ( not args ) and ( not kwargs ) :
        print
        return
    # Construction of the message ...
    msg  = "-> "
    # --
    # current frame (here)
    frame = currentframe()
    # --
    # the frame is here --> frame back
    frame = frame.f_back
    # --
    # line number
    line_number = frame.f_lineno
    # --
    # the code contains useful information
    code = frame.f_code
    # --
    # file_name, func_name
    file_name, func_name = code.co_filename, code.co_name
    # --
    if file_name:
        msg += "%s, "%(file_name.split('/')[-1])
        pass
    msg += "%4d, "%(line_number)
    if cls is not None:
        msg += "%s."%(cls)
        pass
    msg += "%s : "%(func_name)
    for arg in args:
        try:
            msg += str(arg)+" "
        except UnicodeEncodeError:
            msg += unicode(arg)+ " "
            pass
        pass
    # --
    #
    if clt or isClt():
        # It's really too long to write in color :(
        # system("tput setaf 1")
        print "    clt " + msg
        # system("tput setaf 0")
    else:
        print msg
        pass
    #
    return

# --
#

def value2text(value):
    try:
        text = value.__repr__()
    except:
        text = str(value)
        pass
    return text

# --
# Transform name in capital name
# toto_titi --> TotoTiti
#
# Caution : if capitalize() is used
# aaAA will be tranform in Aaaa !!!!!
# So I use my own transormation ...
#

__name2capname = {}

def name2capname(name, space=0):
    #
    if space == 0:
        try:
            return __name2capname[name]
        except KeyError:
            pass
        pass
    #
    words = name.split("_")
    capname = ""
    for w in words:
        # capname += w.capitalize()
        capname += w[:1].upper() + w[1:]
        if space : capname += space*' '
        pass
    #
    if space == 0:
        __name2capname[name] = capname
        pass
    #
    return capname

# --
#

def my_import(name):
    import string
    mod = __import__(name)
    components = string.split(name, '.')
    for comp in components[1:]:
        mod = getattr(mod, comp)
        pass
    return mod

def getModule(name):
    module = my_import(name)
    return module

def getClasses(module, base=None):
    list = module.__dict__.values()
    from inspect import isclass
    test = isclass
    list = filter(test, list)
    if base is not None:
        l = []
        for c in list:
            if issubclass(c, base):
                l.append(c)
                pass
            pass
        list = l
        pass
    return list

def getMRO(c, base=None):
    all_classes = list(c.__mro__)
    if base is not None:
        l = []
        for c in all_classes:
            if issubclass(c, base):
                l.append(c)
                pass
            pass
        all_classes = l
        pass
    return tuple(all_classes)

def getRealClassFromString(string):
    l = string.split(".")
    cls_name = l[-1]
    module_name = ".".join(l[:-1])
    mod = getModule(module_name)
    cls = getattr(mod, cls_name)
    return cls

# --

def exception2message(e=None, long=None):
    from __builtin__ import str
    if e:
        cls = e.__class__
        val = str(e)
        msg = "%s: %s"%(cls.__name__, val)
        return msg
    import sys, StringIO, traceback
    type, value, tb = sys.exc_info()
    if isinstance(type, str):
        value = type
        type = None
        pass
    if not long: return str(value)
    s = StringIO.StringIO()
    traceback.print_tb(tb, file=s)
    msg  = "Traceback (most recent call last):\n"
    msg += s.getvalue()
    if type:
        msg += "%s: %s"%(type.__name__, value)
    else:
        msg += "%s"%(value)
        pass
    return msg

# --
#

class DisassembledCodeItem(object):
    def __init__(self, code):
        lasti, instruction, name = None, None, None
        fields = code.split()
        lasti = int(fields[0])
        instruction = fields[1]
        if instruction in [
            'STORE_NAME',
            'STORE_ATTR',
            'STORE_FAST',
            ]:
            name = fields[2]
            pass
        self.lasti = lasti
        self.instruction = instruction
        self.name = name
        return
    pass

class DisassembledCode(object):
    def __init__(self, disco):
        lasti_max = 0
        lasti2item = {}
        lines = disco.split('\n')
        for i in range(len(lines)):
            item = DisassembledCodeItem(lines[i])
            lasti2item[item.lasti] = item
            #
            if i > 0:
                previous_item.next = item
                pass
            previous_item = item
            #
            lasti = item.lasti
            if lasti > lasti_max:
                lasti_max = lasti
                pass
            #
            pass
        self.lasti2item = lasti2item
        self.lasti_max = lasti_max
        return
    pass

code2disco = {}


def Disassemble(co, lasti=-1):
    """Disassemble a code object."""
    import dis
    aString = ""
    code = co.co_code
    n = len(code)
    i = 0
    while i < n:
        c = code[i]
        op = ord(c)
        #
        # In python 2.3.x, dis has no attribute SET_LINENO
        try:
            is_set_lineno = ( op == dis.SET_LINENO )
        except AttributeError:
            is_set_lineno = 0
            pass
        if is_set_lineno:
            i = i+3
            continue
        #
        aList = []
        aList.append(`i`)
        aList.append(dis.opname[op])
        i = i+1
        if op >= dis.HAVE_ARGUMENT:
            oparg = ord(code[i]) + ord(code[i+1])*256
            i = i+2
            if 0:
                pass
            elif op in dis.hasname:
                aList.append(co.co_names[oparg])
            elif op in dis.haslocal:
                aList.append(co.co_varnames[oparg])
                pass
            pass
        aList.append("\n")
        aString += ' '.join(aList)
        pass
    # Remove the last "\n"
    aString = aString[:-1]
    return aString

def getDisassembledCode(code):
    code_id = id(code)
    try:
        return code2disco[code_id]
    except KeyError:
        pass
    aString = Disassemble(code)
    disco = DisassembledCode(aString)
    # --
    # If the code is defined in an exec "string" statement
    # do not register it ...
    #
    if code.co_filename == "<string>":
        return disco
    #
    code2disco[code_id] = disco

    return getDisassembledCode(code)


def getCreationInformations(shift=0):
    # --
    # stacks contains all the frames
    from inspect import currentframe
    frame = currentframe()
    # --
    # the frame is here --> frame back
    frame = frame.f_back
    # the frame is the frame which called the function --> frame back
    frame = frame.f_back
    # --
    while 1:
        #
        code = frame.f_code
        code = getDisassembledCode(code)
        item = code.lasti2item[frame.f_lasti]
        item = item.next
        # --
        next_operation, name = item.instruction, item.name
        # --
        if next_operation == 'LOAD_FAST':
            # -- shift to the next item
            item = item.next
            next_operation, name = item.instruction, item.name
            pass
        # --
        if next_operation == "RETURN_VALUE":
            frame = frame.f_back
            continue
        # --
        if next_operation == "STORE_NAME":
            percent = float(frame.f_lasti) / code.lasti_max
        else:
            percent = None
            pass
        # --
        break
    return next_operation, name, percent
