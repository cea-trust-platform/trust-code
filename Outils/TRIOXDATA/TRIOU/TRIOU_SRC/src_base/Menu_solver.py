__xdata__name__ = "Solver"
__xdata__items__ = [
    "Execute",
    "Link_ref",
    "UnLink_ref",
    "verify",
    ]



def verifyihm():
    #from xmetaclass import getXObjects
    #list=getXObjects()
    from xtree import getMainXTree
    from xdata import XObject
    list=getMainXTree().children
    listclass=[]
    for l in  list:
        if isinstance(l.node, XObject):
            listclass.append(l.node)
            pass
        pass
    for l in list:
        if isinstance(l.node, XObject):
            try:
        
                l.node.verify(listclass)
            except:
                print "pb   ",l.node,l
                l.status="NoValid"
                pass
            pass
        
        pass
    pass
def linkref(mode):
    from xtree import getMainXTree
    from xdata import XObject
    list=getMainXTree().children
    listclass=[]
    for l in  list:
        if isinstance(l.node, XObject):
            listclass.append(l.node)
            pass
        pass
    # print listclass
    from xmetaclass import getXObjects
    list=getXObjects()
    # pour l'instant on se limite aux classes de la racine
    from triou import isAttributeLink
    for obj in list: 
        for ref in obj.__class__.getAllInitXAttributes():
            if isAttributeLink(ref):
                val=getattr(obj,ref.name)
                if val:
                    from types import StringType
                    if (mode==0):
                        if type(val) is StringType:
                            print "modif ",obj,ref.name," object:",obj.name_u,isAttributeLink(ref)
                            from triou import trouve_class_list
                            val2=trouve_class_list(val,listclass)
                            setattr(obj,ref.name,val2)
                            pass
                        pass
                    else:
                        if not type(val) is StringType:
                            print "modif ",obj,ref.name," object:",obj.name_u,isAttributeLink(ref)
                            from triou import trouve_class_list
                            #val2=trouve_class_list(val,listclass)
                            setattr(obj,ref.name,val.name_u)
                            pass
                        pass
                    pass
                pass
            pass
        pass
    pass
def createDialog(parent, target):
    if target == "Execute":
        from TRIOUGUI_Extension import OnGUIEvent
        OnGUIEvent(6001)
        return
    if target=="Link_ref":
        linkref(0)
        return
    if target=="UnLink_ref":
        linkref(1)
        return
    if target=="verify":
        verifyihm()
        return
    msg=target+" cas non prevu"
    raise Exception(msg)
    return
