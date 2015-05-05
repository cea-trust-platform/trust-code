__xdata__name__ = "TRIOU"
__xdata__items__ = [
    "Solver executable",
    ]

def createDialog(parent, target):
    if target == "Solver executable":
        from TRIOUGUI_Extension import OnGUIEvent
        OnGUIEvent(51)
        return
    return
