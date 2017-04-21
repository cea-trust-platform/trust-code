
## from xdatagui import *
from xdata import *
from xutilities import message


from xqt import *

class ChangeToTypeQPopupMenu(QPopupMenu):
    def __init__(self, parent, popup, cls, selection):
        QPopupMenu.__init__(self, parent)
        popup_parent = popup.parent()
        from xdataobjectexplorer import XDataObjectExplorer
        if isinstance(popup_parent, XDataObjectExplorer):
            explorer = popup_parent
            explorer_item = explorer.selectedItems()[0]
            xtree = explorer_item.xtree
            self.xtree = xtree
            pass
        self.popup = popup
        self.selection = selection
        self.item_ids = {}
        from xmainmenubar import DynamicTip
        if pyqt_module_name == "qt":
            self.t = DynamicTip(self)
            pass
        # --
        if cls:
            for c in cls.__bases__:
                if isinstance(c, str):
                    from xutilities import getRealClassFromString
                    c = getRealClassFromString(c)
                    pass
                self.insertTypeSubmenu(c)
                pass
            pass
        # --
        return
    if pyqt_module_name != "qt":
        def event(self, event):
            if event.type() == QEvent.ToolTip:
                pos = event.pos()
                # pos.setY(pos.y()-self.header().height())
                r, text = self.tip(pos)
                if text:
                    QToolTip.showText(event.globalPos(), text)
                    pass
                pass
            return QPopupMenu.event(self, event)
        pass
    def tip( self, p ):
        pos_id = None
        if pyqt_module_name == "qt":
            for key in range(self.count()):
                r = self.itemGeometry(key)
                if r.contains(p):
                    pos_id = key
                    break
                pass
            pass
        else:
            action = self.actionAt(p)
            if action:
                pos_id = self.actions().index(action)
                pass
            r = None
            pass
        if pos_id is None:
            return QRect( 0,0, -1,-1 ), None
        if 1:
            if 1:
                item_id = self.idAt(pos_id)
                message(item_id, cls=ChangeToTypeQPopupMenu)
                try:
                    target = self.item_ids[item_id]
                except KeyError, AttributeError:
                    return QRect( 0,0, -1,-1 ), None
                message(item_id, target, cls=ChangeToTypeQPopupMenu)
                text = target.getToolTip(target)
                message(text, cls=ChangeToTypeQPopupMenu)
                return r, text
            pass
        return QRect( 0,0, -1,-1 ), None
    def insertTypeSubmenu(self, c):
        #
        #
        bases_dict = {}
        from xdata import getXClasses
        for cls in getXClasses():
            if not issubclass(cls, c): continue
            if c not in cls.__bases__: continue
            cls_name = cls.__name__
            if cls_name in bases_dict: continue
            bases_dict[cls_name] = cls
            pass
        if not bases_dict: 
             item_id = self.insertItem(c.__name__, self.actionActivated)
             self.item_ids[item_id] = c
	     return
        # --
        keys = bases_dict.keys()
        keys.sort()
        bases = [ bases_dict[k] for k in keys ]
        # --
        from xdatagui import getMainWindow
        c_self_menu = ChangeToTypeQPopupMenu(getMainWindow(), self, None, self.selection)
        c_self_menu.xtree = self.xtree
        self.insertItem("%s sub-classes ..."%(c.__name__), c_self_menu)
        #
	item_id=c_self_menu.insertItem(c.__name__, c_self_menu.actionActivated)
	c_self_menu.item_ids[item_id] = c
		     
        for c in bases:
            c_self_menu.insertTypeSubmenu(c)
            pass
        #
        return
    def actionActivated(self, item_id):
        cls = self.item_ids[item_id]
        print "AAA", self, cls,cls.name_,self.selection.__instance__name__
#        from polygon import Square
#        exec "s = Square(123)"
	from triou import change_type
	s = change_type(self.selection,cls)
        
        xtree = self.xtree
        
        if xtree.depth == 1:
            s.__instance__name__=self.selection.__instance__name__
        else:
            #
            parent_branch = [xtree]
            while 1:
                test = parent_branch[0]
                xattr = test.xattribute
                parent_branch.insert(0, test.parent)
                if xattr: break
                pass
            #
            node = parent_branch[0].node
            cmd = "node"
            for p in parent_branch[1:]:
                name = p.name
                if name[0] == '[':
                    cmd += "%s"%(name)
                else:
                    cmd += ".%s"%(name)
                    pass
                pass
            cmd += " = s"
            try:
                exec cmd
            except:
                from xdatagui import getMainWindow
                from xutilities import verbose
                getMainWindow().displayException(long=verbose())
                pass
            pass
        
        from xmetaclass import __xobjects__
        __xobjects__.remove(s)
        
        if xtree.depth == 1:
            __xobjects__[__xobjects__.index(self.selection)] = s
            pass

        from xdatagui import getMainWindow
        getMainWindow().updateObjectBrowser()
        return
    pass

