#Fichier de plot correspondant a la visu "test slice"
SetDatabaseCorrelationOptions(3,2)
vd=VectorAttributes()
vd.SetUseStride(1)
SetDefaultPlotOptions(vd)
annotation=GetAnnotationAttributes()
annotation.SetUserInfoFlag(0)
SetAnnotationAttributes(annotation)


try:
  execfile('config_visit.py')
  print "user config loaded"
except:
  pass
res=OpenDatabase("FTD_all_VEF/post1.lata")
if res==0: print "can't read data file FTD_all_VEF/post1.lata ";quit()
ActivateDatabase("FTD_all_VEF/post1.lata")
ok=AddPlot("Mesh","DOM")
if (ok==0): 1/0
ok=DrawPlots()
if (ok==0): 1/0
ActivateDatabase("FTD_all_VEF/post1.lata")
ok=AddPlot("Pseudocolor","INDICATRICE_INTERF_ELEM_DOM")
if (ok==0): 1/0
ok=DrawPlots()
if (ok==0): 1/0
ActivateDatabase("FTD_all_VEF/post1.lata")
ok=AddPlot("Vector","VITESSE_ELEM_DOM")
if (ok==0): 1/0
ok=DrawPlots()
if (ok==0): 1/0
AddOperator("Slice",1)
s=SliceAttributes()
s.originType = s.Point
s.originPoint=(0.,0.02,0.)
s.normal=(0.,1.,0)
s.project2d=0
SetOperatorOptions(s,0,1)
ok=DrawPlots()
if (ok==0): 1/0
s=SaveWindowAttributes()
s.SetFormat(4)
SetSaveWindowAttributes(s)
cycles=[str(TimeSliderGetNStates()-1)]
num=-1
for state in cycles:
  num+=1
  state_reel=eval(state)
  if (state_reel<0): state_reel+=TimeSliderGetNStates()
  ok=SetTimeSliderState(state_reel)
  if (ok==0): 1/0
  Query("Time")
  time=GetQueryOutputValue()

  name=SaveWindow()
  print name
  cmd="mv "+name+" ./.tmp/fic_001_%d.png"%num
  import os
  print cmd
  os.system(cmd)
#SaveSession("visit_001.session")
#quit()
