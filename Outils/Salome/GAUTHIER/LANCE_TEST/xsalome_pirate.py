# --
# Copyright (C) CEA, EDF
# Author : Erwan ADAM (CEA)
# --

from xutilities_pirate import message

# --
#

def searchFreePort():
    print "Searching a free port for naming service:",
    NSPORT=2810
    limit=NSPORT
    limit=limit+100
    while 1:
        print "%s "%(NSPORT),
        import os
        status = os.system("netstat -ltn | grep -E :%s"%(NSPORT))
        if status:
            home = os.environ['HOME']
            hostname = os.environ['HOSTNAME']
            os.environ['OMNIORB_CONFIG'] = '%s/.omniORB_%s.cfg'%(home, NSPORT)
            f = open(os.environ['OMNIORB_CONFIG'], "w")
            f.write("ORBInitRef NameService=corbaname::%s:%s\n"%(hostname, NSPORT))
            f.close()
            print "- Ok"
            break
        if NSPORT == limit:
            msg  = ""
            msg += "I Can't find a free port to launch omniNames\n"
            msg += "I suggest you to kill the running servers and try again.\n"
            raise msg
        NSPORT=NSPORT+1
        pass
    return NSPORT

# --
#

_x_salome_session = None

def getXSalomeSession(*args, **kwargs):
    global _x_salome_session
    #
    if _x_salome_session:
        return _x_salome_session
    #
    if str(__import__).find('import_hook') >= 0:
        _x_salome_session = __import__('salome')
    else:
        _x_salome_session = XSalomeSession(*args, **kwargs)
        pass
    #
    return getXSalomeSession(*args, **kwargs)

# --
#

class XSalomeSession(object):
    
    def getKRD(self):
        import os
        KERNEL_ROOT_DIR = os.getenv("KERNEL_ROOT_DIR")
        if KERNEL_ROOT_DIR is None:
            from os import system
            status = system('which SALOME_Container > /tmp/.where_is_runSalome 2> /dev/null')
            if status == 0:
                f = open('/tmp/.where_is_runSalome')
                KERNEL_ROOT_DIR = f.read().split('/bin/salome/runSalome')[0]
                from os.path import abspath
                KERNEL_ROOT_DIR = abspath(KERNEL_ROOT_DIR)
                f.close()
                pass
            system('/bin/rm /tmp/.where_is_runSalome')
            pass
        return KERNEL_ROOT_DIR
    
    def __init__(self, modules, study=0, logger=0, sleeping_time=1):
        try:
            from xcontext import setContext
            setContext("clt")
        except ImportError:
            pass
        import sys
        argv_ini = sys.argv
        #
        import sys
        sys.argv  = ['bin/salome/runSalome.py']
        sys.argv += ['--modules=%s'%(",".join(modules))]
        if logger:
            sys.argv += ['--logger']
            pass
        # sys.argv += ['--standalone=pyContainer']
        #
        # --
        # E.A. : sys.path.insert(0, KERNEL_ROOT_DIR+"/bin/salome")
        # because there is multiple runSalome.py in V2_2_4 distribution
        # This will be removed after ...
        #
        KERNEL_ROOT_DIR = self.getKRD()
        if KERNEL_ROOT_DIR:
            sys.path.insert(0, KERNEL_ROOT_DIR+"/bin/salome")
            pass
        #
        try:
            import runSalome
        except ImportError:
            KERNEL_ROOT_DIR = self.getKRD()
            if KERNEL_ROOT_DIR:
                sys.path.insert(0, KERNEL_ROOT_DIR+"/bin/salome")
                pass
            import runSalome
            pass
        self.runSalome = runSalome
        #
        if hasattr(runSalome, "main"):
            if hasattr(runSalome, "searchFreePort"):
                import launchConfigureParser
                if hasattr(launchConfigureParser, "CreateOptionParser"):
                    sys.argv += ['--gui','--show-desktop=0']
                else:
                    sys.argv += ['--gui=0']
                    pass
                pass
            else:
                sys.argv += ['--terminal']
                port = searchFreePort()
                pass
            clt, args = runSalome.main()
            runSalome.clt, runSalome.args = clt, args
            study_manager = clt.Resolve("/myStudyManager")
            from SALOMEDS import StudyManager
            study_manager = study_manager._narrow(StudyManager)
            self.study_manager = study_manager
        else:
            port = searchFreePort()
            #
            clt = runSalome.orbmodule.client()
            #
            runSalome.LoggerServer().run()
            clt.waitLogger("Logger")
            #
            cataServer=runSalome.CatalogServer()
            cataServer.setpath(runSalome.modules_list)
            cataServer.run()
            import SALOME_ModuleCatalog
            clt.waitNS("/Kernel/ModulCatalog",SALOME_ModuleCatalog.ModuleCatalog)
            #
            runSalome.RegistryServer().run()
            clt.waitNS("/Registry")
            #
            if study:
                os.environ["CSF_PluginDefaults"] = os.path.join(os.environ["KERNEL_ROOT_DIR"],
                                                                "share","salome","resources")
                os.environ["CSF_SALOMEDS_ResourcesDefaults"] = os.path.join(os.environ["KERNEL_ROOT_DIR"],
                                                                            "share","salome","resources")
                runSalome.SalomeDSServer().run()
                clt.waitNS("/myStudyManager")
                study_manager = clt.Resolve("/myStudyManager")
                from SALOMEDS import StudyManager
                study_manager = study_manager._narrow(StudyManager)
                self.study_manager = study_manager
                pass
            #
            theComputer = os.getenv("HOSTNAME")
            computerSplitName = theComputer.split('.')
            theComputer = computerSplitName[0]
            #
            runSalome.ContainerCPPServer().run()
            clt.waitNS("/Containers/" + theComputer + "/FactoryServer")
            runSalome.ContainerPYServer().run()
            clt.waitNS("/Containers/" + theComputer + "/FactoryServerPy")
            #
            import pickle
            filedict='/tmp/'+os.getenv('USER')+"_"+str(port)+'_'+'SALOME'+'_pidict'
            process_ids = []
            try:
                fpid=open(filedict, 'r')
                process_ids=pickle.load(fpid)
                fpid.close()
            except:
                pass
            fpid=open(filedict, 'w')
            process_ids.append(runSalome.process_id)
            pickle.dump(process_ids,fpid)
            fpid.close()
            #
            pass
        #
        self.port = runSalome.args['port']
        self.naming_service = clt
        #
        from LifeCycleCORBA import LifeCycleCORBA
        lcc = LifeCycleCORBA(clt.orb)
        self.lcc = lcc
        #
        from time import sleep
        print "You have %s seconds to launch whatever you want ..."%(sleeping_time)
        import os
        print "OMNIORB_CONFIG: %s"%(os.environ['OMNIORB_CONFIG'])
        sleep(sleeping_time)
        print "... finished."
        #
        import sys
        sys.argv = argv_ini
        # --
        # E.A. : New since python 2.4 : Need to import XDATA_CORBA just after
        # runSalome otherwise there are attribute errors in import
        try:
            import XDATA_CORBA
        except ImportError:
            pass
        #
        return
    def __del__(self):
        try:
            port = self.port
        except:
            return
        from os import system
        system("killSalomeWithPort.py %s"%(port))
        return
    pass

# --
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

_orb = None

def getORB():
    global _orb
    if _orb is None:
        from CORBA import ORB_init, ORB_ID
        from sys import argv
        _orb = ORB_init(argv, ORB_ID)
        pass
    return _orb

# --
#

_id2corbaobject = {}
_ior2pythonobject = {}

def register(python_object, corba_object):
    #
    message(python_object, corba_object)
    # --
    _id2corbaobject[id(python_object)] = corba_object
    # --
    try:
        the_ior = getORB().object_to_string(corba_object)
        _ior2pythonobject[the_ior] = python_object
    except TypeError:
        pass
    # --
    return

def isLocal(corba_object):
    the_ior = getORB().object_to_string(corba_object)
    return the_ior in _ior2pythonobject

def getCorbaObjectModuleAndInterface(obj):
    # rep_id shoud be "IDL:MODULE/INTERFACE:X.X"
    rep_id = obj._NP_RepositoryId
    # forget "IDL:" and ":X.X" --> "MODULE/INTERFACE"
    rep_id = rep_id.split(":")[1]
    return tuple(rep_id.split("/"))

def python2corba(python_object, component, xtype=None, depth=0):
    # --
    if python_object is None:
        return None
    # --
    if hasattr(python_object, "_narrow"):
        return python_object
    # --
    # Bug: When a XAttribute has a XObject instance as default value,
    # this value is copy.copy for each instance. So, the instance
    # is not in _id2corbaobject, but has a __corba__object__
    # attribute ...
    if hasattr(python_object, "__corba__object__"):
        return getattr(python_object, "__corba__object__")
    # --
    message(python_object, component._get_interfaceName(), xtype, depth)
    # --
    from xtypes import XMulTypes
    if isinstance(xtype, XMulTypes):
        if len(xtype.xtypes) == 2:
            from xdata import XNone
            if isinstance(xtype.xtypes[0], XNone):
                return python2corba(python_object, component, xtype.xtypes[1], depth+1)
            if isinstance(xtype.xtypes[1], XNone):
                return python2corba(python_object, component, xtype.xtypes[0], depth+1)
            pass
        pass
    # --
    # We must go into xtype before test the python_object id
    # since for instance the object 2 can have a corresponding
    # corba_object but if type(xtype) is XInt, we must return
    # simply 2 instead of the corba object ...
    if depth == 0:
        if xtype:
            idl_type = xtype.getIdlType()
            message("idl_type:", idl_type)
            if idl_type in ("boolean", "long", "double", "string", ):
                return python_object
            if idl_type in ("XDATA_CORBA::longSequence",
                            "XDATA_CORBA::doubleSequence",
                            "XDATA_CORBA::stringSequence"):
                return python_object
            if idl_type in ("XDATA_CORBA::XObjectSequence", ):
                #
                seq = xtype.sequence
                if not seq :
                    seq = [ None ]
                    pass
                l_seq = len(seq)
                #
                l_list = len(python_object)
                corba_object = []
                for i in range(l_list):
                    val = python_object[i]
                    xtype = seq[i%l_seq]
                    message(val, xtype)
                    corba_val = python2corba(val, component, xtype, depth+1)
                    corba_object.append(corba_val)
                    pass
                return corba_object
            pass
        pass
    # --
    # Search if the python_object is known in local ...
    python_object_id = id(python_object)
    if python_object_id in _id2corbaobject:
        message("python_object", python_object, "found in local")
        corba_object = _id2corbaobject[python_object_id]
        message("corba_object:", corba_object)
        return corba_object
    # --
    # CAUTION: keep str(..) otherwise it's crash with tuples
    message("python_object", python_object, "not found in local")
    # --
    from xdata import XObject
    if myissubclass(python_object, XObject):
        message("python_object %s inherit XObject ..."%(python_object))
        if python_object is XObject:
            from XDATA_i import XObject_i
            corba_object = XObject_i
            corba_object.__cmdin__ = "XDATA_CORBA.XObject"
            register(python_object, corba_object)
            return python2corba(python_object, component, xtype, depth)
        cls = python_object
        interface_name = component._get_interfaceName()
        mod = __import__(interface_name.upper())
        try:
            name = "%s"%(cls.__name__)
            corba_object = getattr(mod, name+'_i')
        except AttributeError:
            name = "%s%s"%(cls.__module__, cls.__name__)
            corba_object = getattr(mod, name+'_i')
            pass
        corba_object.__cmdin__ = "%s_CORBA.%s"%(interface_name.upper(), name)
        #
        # E.A. : There is a problem when the same class appears in two
        # modules ... So comment the register step thus the correct corba class
        # is always returned
        #
        # register(python_object, corba_object)
        # return python2corba(python_object, component, xtype, depth)
        return corba_object
    # --
    #
    if myissubclass(python_object, object):
        from xdata import XObject
        return python2corba(XObject, component, xtype, depth)
    # --
    #
    from xcontext import getContext
    context = getContext()
    if context == "clt":
        message("client side ...")
        # --
        # Client side part
        # We should be here only for instance of:
        # int, float, string, list, dict
        if isinstance(python_object, int):
            corba_object = component.newXInt(python_object)
            register(python_object, corba_object)
            return python2corba(python_object, component, xtype, depth)
        if isinstance(python_object, float):
            corba_object = component.newXFloat(python_object)
            register(python_object, corba_object)
            return python2corba(python_object, component, xtype, depth)
        if isinstance(python_object, str):
            corba_object = component.newXString(python_object)
            register(python_object, corba_object)
            return python2corba(python_object, component, xtype, depth)
        if isinstance(python_object, (list, tuple)):
            corba_object = None
            from xtypes import XList
            if isinstance(xtype, XList):
                seq = xtype.sequence
                if seq and len(seq) == 1:
                    from xtypes import XInt, XFloat, XString
                    xtype = seq[0]
                    if isinstance(xtype, XInt):
                        corba_object = component.newXListLongValues(python_object)
                    elif isinstance(xtype, XFloat):
                        corba_object = component.newXListDoubleValues(python_object)
                    elif isinstance(xtype, XString):
                        corba_object = component.newXListStringValues(python_object)
                        pass
                    pass
                pass
            if corba_object is None:
                message('converting the list content from python to corba')
                l = [  python2corba(v, component, None, depth) for v in python_object ]
                corba_object = component.newXListXObjectValues(l)
                pass
            return corba_object
        if isinstance(python_object, dict):
            # --
            # if xtype is None put an empty XDict
            if xtype is None:
                from xdata import XDict
                xtype = XDict()
                pass
            # --
            values = python_object.values()
            values = [ python2corba(v, component, xtype.values, depth+1) for v in values ]
            keys = python_object.keys()
            idl_type = xtype.getIdlType()
            if 0:
                pass
            elif idl_type == "XDATA_CORBA::XDictStringKeys":
                corba_object = component.newXDictStringKeys(keys, values)
            elif idl_type == "XDATA_CORBA::XDictLongKeys":
                corba_object = component.newXDictLongKeys(keys, values)
            elif idl_type == "XDATA_CORBA::XDictDoubleKeys":
                corba_object = component.newXDictDoubleKeys(keys, values)
            else:
                keys = [ python2corba(k, component, None, depth) for k in keys ]
                corba_object = component.newXDictXObjectKeys(keys, values)
                pass
            return corba_object
        raise "??????? %s"%(python_object)
    # --
    message("server side ...")
    # --
    corba_class = None
    # --
    if python_object is type:
        raise Exception('Problem : python_object is type !!')
    elif isinstance(python_object, int):
        from XDATA_i import XInt_i
        corba_class = XInt_i
        corba_class.__cmdin__ = "XDATA_CORBA.XInt"
    elif isinstance(python_object, float):
        from XDATA_i import XFloat_i
        corba_class = XFloat_i
        corba_class.__cmdin__ = "XDATA_CORBA.XFloat"
    elif isinstance(python_object, str):
        from XDATA_i import XString_i
        corba_class = XString_i
        corba_class.__cmdin__ = "XDATA_CORBA.XString"
    elif isinstance(python_object, (list, tuple)):
        corba_class = None
        from xtypes import XList
        if isinstance(xtype, XList):
            seq = xtype.sequence
            if seq and len(seq) == 1:
                from xtypes import XInt, XFloat, XString
                xtype = seq[0]
                if isinstance(xtype, XInt):
                    from XDATA_i import XListLongValues_i
                    corba_class = XListLongValues_i
                    corba_class.__cmdin__ = "XDATA_CORBA.XListLongValues"
                elif isinstance(xtype, XFloat):
                    from XDATA_i import XListDoubleValues_i
                    corba_class = XListDoubleValues_i
                    corba_class.__cmdin__ = "XDATA_CORBA.XListDoubleValues"
                elif isinstance(xtype, XString):
                    from XDATA_i import XListStringValues_i
                    corba_class = XListStringValues_i
                    corba_class.__cmdin__ = "XDATA_CORBA.XListStringValues"
                else:
                    pass
                pass
            pass
        if corba_class is None:
            from XDATA_i import XListXObjectValues_i
            corba_class = XListXObjectValues_i
            corba_class.__cmdin__ = "XDATA_CORBA.XListXObjectValues"
            pass
        pass
    elif isinstance(python_object, dict):
        keys = python_object.keys()
        # --
        # if xtype is None put an empty XDict
        if xtype is None:
            from xdata import XDict
            xtype = XDict()
            pass
        # --
        idl_type = xtype.getIdlType()
        if 0:
            pass
        elif idl_type == "XDATA_CORBA::XDictStringKeys":
            from XDATA_i import XDictStringKeys_i
            corba_class = XDictStringKeys_i
            corba_class.__cmdin__ = "XDATA_CORBA.XDictStringKeys"
        elif idl_type == "XDATA_CORBA::XDictLongKeys":
            from XDATA_i import XDictLongKeys_i
            corba_class = XDictLongKeys_i
            corba_class.__cmdin__ = "XDATA_CORBA.XDictLongKeys"
        elif idl_type == "XDATA_CORBA::XDictDoubleKeys":
            from XDATA_i import XDictDoubleKeys_i
            corba_class = XDictDoubleKeys_i
            corba_class.__cmdin__ = "XDATA_CORBA.XDictDoubleKeys"
        else:
            from XDATA_i import XDictXObjectKeys_i
            corba_class = XDictXObjectKeys_i
            corba_class.__cmdin__ = "XDATA_CORBA.XDictXObjectKeys"
            pass
        pass
    else:
        python_class = python_object.__class__
        message("python_class =", python_class)
        python_module = python_class.__module__
        #
        if python_module == "libMEDClient":
            #
            mod = __import__("libMedCorba_Swig")
            name = python_class.__name__
            #
            if name in ("FIELDDOUBLE", "FIELDINT", ):
                if name == "FIELDDOUBLE":
                    corba_class = getattr(mod, "createCorbaFieldDouble")
                elif name == "FIELDINT":
                    corba_class = getattr(mod, "createCorbaFieldInt")
                    pass
                from libMedCorba_Swig import createCorbaSupport
                support_corba = createCorbaSupport(python_object.getSupport())
                corba_object = corba_class(support_corba, python_object)
            else:
                raise "%s NOT IMPLEMENTED"%(name)
            register(python_object, corba_object)
            return python2corba(python_object, component, xtype, depth)
        #
        corba_class = python2corba(python_class, component, xtype, depth)
        pass
    # --
    if corba_class:
        message("corba_class =", corba_class)
        message("corba_class.__cmdin__ =", corba_class.__cmdin__)
        corba_object = corba_class(python_object, component, xtype, corba_class.__cmdin__)
        corba_object = corba_object._this()
        register(python_object, corba_object)
        return python2corba(python_object, component, xtype, depth)
    raise "??????? %s"%(python_object)

def corba2python(corba_object):
    # --
    if corba_object is None:
        return None
    # --
    if type(corba_object) in (int, float, str, ):
        return corba_object
    # --
    if type(corba_object) in (list, ):
        return [ corba2python(obj) for obj in corba_object ]
    # --
    if type(corba_object) in (tuple, ):
        return tuple([ corba2python(obj) for obj in corba_object ])
    # --
    from XDATA_CORBA import XException
    if isinstance(corba_object, XException):
        e = corba_object
        pmdcn = e.pmdcn
        msg = e.msg
        if not pmdcn:
            e = msg
        else:
            from xutilities_pirate import getRealClassFromString
            cls = getRealClassFromString(pmdcn)
            e = cls(msg)
            pass
        return e
    # --
    message("corba_object =", corba_object)
    # --
    ior = getORB().object_to_string(corba_object)
    if ior in _ior2pythonobject:
        message("ior found in local")
        python_object = _ior2pythonobject[ior]
        message("python_object =", python_object)
        return python_object
    # --
    # This ior is not local
    # --
    message("corba_object", corba_object, "not local", clt=1)
    if hasattr(corba_object, "getCorbaModuleDotInterfaceName"):
        # --
        # get corba interface
        cmdin = corba_object.getCorbaModuleDotInterfaceName()
        cls = cmdin
        message(cls, clt=1)
        from xutilities_pirate import getRealClassFromString
        cls = getRealClassFromString(cls)
        message("corba interface:", cls, clt=1)
        corba_object = corba_object._narrow(cls)
        message("corba_object =", corba_object, clt=1)
        # --
        # get python class
        cls = corba_object.getPythonModuleDotClassName()
        from xutilities_pirate import getRealClassFromString
        cls = getRealClassFromString(cls)
        message("python class:", cls, clt=1)
        #
        if myissubclass(cls, int, float, str):
            python_object = corba_object.value()
            register(python_object, corba_object)
            return corba2python(corba_object)
        #
        if myissubclass(cls, list, tuple):
            message('python list reconstruction ...', clt=1)
            python_object = corba_object.value()
            if cmdin == "XDATA_CORBA.XListXObjectValues":
                python_object = [ corba2python(v) for v in python_object ]
                pass
            if myissubclass(cls, tuple):
                python_object = tuple(python_object)
                pass
            # --
            # CAUTION: Do not register since instance of list
            # are mutable objects and can have been changed on the
            # server ... so we have to reconstruct it each time
            return python_object
        #
        if myissubclass(cls, dict):
            message('python dict reconstruction ...', clt=1)
            python_object = {}
            message('get keys ...', clt=1)
            keys = corba_object.keys()
            message('keys are %s'%keys, clt=1)
            for key in keys:
                message("get corba_value for key:", key, clt=1)
                val = corba_object.value(key)
                message("key, corba_value:", key, val, clt=1)
                val = corba2python(val)
                message("key, python_value:", key, val, clt=1)
                key = corba2python(key)
                python_object[key] = val
                pass
            # --
            # CAUTION: Do not register since instance of dict
            # are mutable objects and can have been changed on the
            # server ... so we have to reconstruct it each time
            # xdictwrappercorbatest.py test the non regression on this
            return python_object
        #
        # from xdata import XObject
        if myissubclass(cls, object):
            python_object = cls.__new__(cls)
            python_object.__corba__object__ = corba_object
            python_object.__corba__component__ = corba_object.getCorbaComponent()
            python_object.__xcontext__ = None
            register(python_object, corba_object)
            return corba2python(corba_object)
        #
        pass
    #
    message("corba_object", corba_object, "not XObject ...", clt=1)
    module, interface = getCorbaObjectModuleAndInterface(corba_object)
    message("module, interface :", module, interface, clt=1)
    if module == "SALOME_MED":
        # If MED corba object, use the libMEDClient from M.T. && A.G.
        # to create local objects ...
        mod = __import__("libMEDClient")
        message("python module :", mod, clt=1)
        cls = getattr(mod, "%sClient"%(interface))
        message("python class :", cls, clt=1)
        python_object = cls(corba_object)
        message("python object :", python_object, clt=1)
        register(python_object, corba_object)
        return corba2python(corba_object)
    #
    return corba_object

# --

_name2services = {}

def getServicesFromComponentName(name):
    return _name2services[name]

_components_names = None

def getComponentsNames():
    global _components_names
    if _components_names is not None:
        return _components_names
    _components_names = []
    from orbmodule import client
    ns = client()
    catalog = ns.Resolve("Kernel/ModulCatalog")
    from SALOME_ModuleCatalog import ModuleCatalog
    catalog = catalog._narrow(ModuleCatalog)
    l = catalog.GetComponentList()
    for name in l:
        if name == "KERNEL": continue
        try:
            mod = __import__('%s_CORBA'%(name))
        except ImportError:
            continue
        try:
            gen = getattr(mod, "%s_Component"%(name))
        except AttributeError:
            continue
        global _name2services
        _name2services[name] = dir(gen)
        _components_names.append(name)
        pass
    return getComponentsNames()

_name2component = {}

def getComponentFromName(name):
    global _name2component
    if name in _name2component:
        return _name2component[name]
    if name is None: return
    from LifeCycleCORBA import LifeCycleCORBA
    lcc = LifeCycleCORBA(getORB())
    component = lcc.FindOrLoadComponent("FactoryServerPy", name)
    mod = __import__("%s_CORBA"%(name))
    c = getattr(mod, "%s_Component"%(name))
    component = component._narrow(c)
    _name2component[name] = component
    return getComponentFromName(name)

_service2component = {}

def getComponentFromService(service):
    global _service2component
    if _service2component.has_key(service):
        return _service2component[service]
    names = getComponentsNames()
    component_name = None
    for name in _components_names:
        services = getServicesFromComponentName(name)
        if service not in services: continue
        component_name = name
        break
    component = getComponentFromName(component_name)
    _service2component[service] = component
    return getComponentFromService(service)

def getComponentFromClass(cls):
    #
    global _component_from_gui
    if _component_from_gui:
        typ = cls
        if hasattr(_component_from_gui, "new%s"%(typ.__name__)):
            return _component_from_gui
        if hasattr(_component_from_gui, "new%s%s"%(typ.__module__, typ.__name__)):
            return _component_from_gui
        pass
    #
    component = None
    for prefix in ['', cls.__module__]:
        s = "_d_%s_mod_cls_sep_%s%s"%(cls.__module__, prefix, cls.__name__)
        s = s.replace(".", "_dot_")
        component = getComponentFromService(s)
        if component: break
        pass
    if component is None:
        raise "No salome component found for the class %s"%(cls)
    return component

_component_name_from_gui = None
_component_from_gui = None

def setComponentFromGUI(name):
    global _component_name_from_gui
    if _component_name_from_gui == name:
        return
    #
    from LifeCycleCORBA import LifeCycleCORBA
    lcc = LifeCycleCORBA(getORB())
    component = lcc.FindOrLoadComponent("FactoryServerPy", name)
    mod = __import__("%s_CORBA"%(name))
    c = getattr(mod, "%s_Component"%(name))
    component = component._narrow(c)
    #
    global _component_from_gui
    _component_from_gui = component
    return
