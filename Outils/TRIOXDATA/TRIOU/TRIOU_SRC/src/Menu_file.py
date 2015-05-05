__xdata__name__ = "File"

__xdata__items__ = [
    "Import data file",
    "Export data file",
    ]

def createDialog(parent, target):
    if target == "Import data file":
        from TRIOUGUI_Extension import OnGUIEvent
        OnGUIEvent(91)
        return
    if target == "Export data file":
        from TRIOUGUI_Extension import OnGUIEvent
        OnGUIEvent(92)
        return
    pass
