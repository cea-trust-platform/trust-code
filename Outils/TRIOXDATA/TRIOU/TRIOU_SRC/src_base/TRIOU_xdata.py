__xdata__name__ = "triou"
__xdata__items__ = [
    "Menu_file",
    "menu_domaine",
    "menu_Pb_base",
    "menu_schema_temps_base",
    "menu_interprete",
    "menu_objet_u",
    "Menu_solver",
    "Menu_preferences",
    "GetExtensionObject",
    "ExportImportObject",
    ]

# ------------

__VERSION__ = "0.2.0"

from xdata import *

class GetExtensionObject(XObject):
    __object__xmethods__ = [
        XMethod('GetExtension',
                out_xtype=XSalomeReference(component="TRIOU",
                                           idl="TRIOU_Extension.idl",
                                           module="TRIOU_CORBA",
                                           interface="Extension",
                                           )
                ),
        ]

    def GetExtension(self):
        from TRIOU_Extension import Extension
        return Extension()._this()

    pass

study_to_cls_to_name_map = {}

class ExportImportObject(XObject):
    __object__xmethods__ = [
        XMethod('ImportDataFile',
                in_xattributes = [ XAttribute("study", xtype=XSalomeReference(component="KERNEL",
                                                                              idl="SALOMEDS.idl",
                                                                              module="SALOMEDS",
                                                                              interface="Study",
                                                                              )),
                                   XAttribute("fileName", xtype=XInputFileName())
                                   ],
                out_xtype=XInt()
                ),
        XMethod('ExportDataFile',
                in_xattributes = [ XAttribute("study", xtype=XSalomeReference(component="KERNEL",
                                                                              idl="SALOMEDS.idl",
                                                                              module="SALOMEDS",
                                                                              interface="Study",
                                                                              )),
                                   XAttribute("fileName", xtype=XOutputFileName())
                                   ],
                out_xtype=XInt()
                ),
        ]

    def ImportDataFile(self, study, fileName):
        print study, fileName
        try:
            from triou import read_file_data
            listclass = read_file_data( fileName )
            print listclass
            from xsalome import python2corba
            from TRIOU_Extension import getEngine
            engine = getEngine()
            amap = study_to_cls_to_name_map.get( study._get_StudyId(), {} )
            for obj in listclass:
                cobj = python2corba(obj, engine)
                cls = obj.__class__.__name__
                if not amap.has_key( cls ):
                    amap[ cls ] = 0
                amap[ cls ] = amap[ cls ] + 1
                name = cls + "_" + str(amap[ cls ])
                study_to_cls_to_name_map[ study._get_StudyId() ] = amap
                engine.PublishInStudy(study, None, cobj, name)
            return 1
        except Exception, e:
            print e
            pass
        return 0

    def ExportDataFile(self, study, fileName):
        print study, fileName
        try:
            from xsalome import corba2python
            listclass = []
            aComp = study.FindComponent( "TRIOU" )
            if aComp is not None:
                anIter = study.NewChildIterator( aComp )
                while anIter.More():
                    aSObj = anIter.Value()
                    anIter.Next()
                    aObject = aSObj.GetObject()
                    if aObject:
                        aPObj = corba2python( aObject )
                        if aPObj:
                            listclass.append( aPObj )
                        pass
                    pass
            from triou import write_file_data
            write_file_data( fileName, listclass )
            return 1
        except Exception, e:
            print e
            pass
        return 0
    pass

def setSettings():
    if in_tui:
        from TRIOUGUI_Extension import setSettings
        setSettings()
        pass
    pass
