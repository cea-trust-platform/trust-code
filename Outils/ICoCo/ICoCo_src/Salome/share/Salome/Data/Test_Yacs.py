import loader
import pilot
import SALOMERuntime
import threading

SALOMERuntime.RuntimeSALOME_setRuntime(True)
runtime=pilot.getRuntime()
xmlLoader = loader.YACSLoader()
mainProc2L = xmlLoader.load("OO3.xml")
exe=pilot.ExecutorSwig()
exe.RunPy(mainProc2L)
