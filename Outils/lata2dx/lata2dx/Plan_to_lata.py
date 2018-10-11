# Script to convert .plan file (obtained using a plane of probes from TRUST) to LATA format
import sys
import os
from optparse import OptionParser
import numpy as np

# Read .plan file and get the different parameters
def get_parameters(PlanFile):
  f = open(PlanFile,'r')
  lines = f.readlines()
  DataFile = lines[1].rstrip()
  Probe = TestCase.replace(DataFile+'_','')
  Nx=int(lines[3].split(' ')[3]) # Number of probes in 1st-direction
  Ny=int(lines[3].split(' ')[4]) # Number of probes in 2nd-direction
  Nelem = int(lines[4].split(' ')[2])/2
  Ncells = int(lines[2*Nelem+8]) # Total number of cells
  Field=lines[2*Nelem + Ncells + 13].split(' ')[0]

  if (lines[2*Nelem+Ncells+14].split(' ')[0]=='type0'):
    comp='1'
    nature='scalar'
  elif (lines[2*Nelem+Ncells+14].split(' ')[0]=='type1'):
    comp='2'
    nature='vector'
  return DataFile,Probe,lines,Nx,Ny,Nelem, Ncells,Field,comp, nature

# Re-write the connection between the different elements
def save_connections(LataFile, Probe):
  nx1=[]
  for i in range((Nx-1)*(Ny-1)):
      nx1.append(0)
  nx1[0]=1
  for i in range((Nx-1)*(Ny-1)):
      nx1[i] = nx1[i-1]+1
      if (i%(Ny-1) == 0 and i >0):
         nx1[i] += 1
  nx2 = [ x+1 for x in nx1 ]
  ny1 = [ x+Ny for x in nx1 ]
  ny2 = [ x+1 for x in ny1 ]

  GridConnections=LataFile+'.'+Probe+'.0.0.elem'
  with open(GridConnections, 'w+') as datafile_id:
      np.savetxt(datafile_id, np.array([nx1, nx2, ny1, ny2]).T, fmt=['%d','%d','%d','%d'])
  return GridConnections

def write_lata_file(LataFile,DataFile,Probe,GridCoordinates,GridConnections,Nelem,Ncells,post_processed_Fields):
  latafileout = open(LataFile,'w')
  latafileout.write('LATA_V2.1\n'+DataFile+'\nTrio_U verbosity=0')
  latafileout.write('\nFormat LITTLE_ENDIAN,INT32,F_INDEXING,C_ORDERING,F_MARKERS_SINGLE,REAL32'+'\nGEOM '+Probe+' type_elem=RECTANGLE')
  latafileout.write('\nCHAMP SOMMETS '+GridCoordinates+' geometrie='+Probe+' size='+str(Nelem)+' composantes=2 format=ASCII,F_MARKERS_NO')
  latafileout.write('\nCHAMP ELEMENTS '+GridConnections+' geometrie='+Probe+' size='+str(Ncells)+' composantes=4 format=ASCII,INT32,F_MARKERS_NO\n')
  latafileout.write(post_processed_Fields)
  latafileout.write('FIN')
  latafileout.close()

def write_coordinates(Nelem,LataFile,Probe):
  GridCoordinates = LataFile+'.'+Probe+'.0.0'
  f = open(GridCoordinates,'w') 
  for i in range(5,Nelem+5):
     t = ' '.join(lines[i].split(' ')[:2])
     f.write(t+'\n')
  f.close()
  return GridCoordinates

def save_field(LataFile,Field,Probe):
  current_time = lines[current_line-4]
  Time = float(current_time.split(' ')[1])
  FieldFile = LataFile+'.'+Field+'.'+Probe+'.'+'%12.10f' % Time
  f = open(FieldFile,'w')
  for i in range(current_line,current_line+Nelem):
      t = ' '.join(lines[i].split(' ')[1:])
      f.write(t+'\n')
  f.close()
  print "Writing the", Field, "field at time=%12.10f" % Time
  return current_time, FieldFile

def add_line_to_lata(post_processed_Fields):
  post_processed_Fields += current_time+'CHAMP '+Field+' '+FieldFile+' geometrie='+Probe+' size='+str(Nelem)+' composantes='+comp+' localisation=SOM nature='+nature+' format=ASCII,F_MARKERS_NO\n'
  return post_processed_Fields

if __name__=="__main__":
  parser = OptionParser()
  parser.set_usage("Convert a .plan file to LATA format.\n  python Plan_to_lata.py ProbeOutputFile.plan [-l]\n or\n  python Plan_to_lata.py ProbeOutputFile [-l]")
  parser.add_option("-l", "--lasttime", action="store_true", dest="lasttime", default=False,
                      help="Store only the last time. By default all times are stored.")
  (opts, args) = parser.parse_args()

  if len(args) == 0:
     parser.print_help()
     print "\nPlease specify a .plan file!"
     exit(1)

  InputFileName = sys.argv[1]
  if InputFileName.endswith('.plan'):
     TestCase = os.path.splitext(os.path.basename(InputFileName))[0]
     PlanFile = InputFileName
  else:
     TestCase = InputFileName
     PlanFile = TestCase+'.plan'
     
  LataFile = TestCase+'.lata'
  if not (os.path.isfile(PlanFile)):
     parser.print_help()
     print "\nError:", PlanFile,"file does not exist!"
     exit(1)
  parser.print_usage()
  # Properties:
  DataFile,Probe,lines,Nx,Ny,Nelem,Ncells,Field,comp,nature = get_parameters(PlanFile)
  NumberOfLines = sum(1 for line in lines )
  
  # Grid
  GridCoordinates = write_coordinates(Nelem,LataFile,Probe)
  GridConnections = save_connections(LataFile,Probe)
  print Probe,"grid has been correctly generated"

  if opts.lasttime:
     current_line = NumberOfLines-(2*Nelem)
  else:
     current_line= 2*Nelem + Ncells + 15

  post_processed_Fields=''
  while current_line < NumberOfLines:
     current_time, FieldFile = save_field(LataFile,Field,Probe)
     post_processed_Fields = add_line_to_lata(post_processed_Fields)
     current_line += 2*Nelem+4

  # Write LataFile
  write_lata_file(LataFile,DataFile,Probe,GridCoordinates,GridConnections,Nelem,Ncells,post_processed_Fields)
  print "\n", PlanFile, "has been successfuly converted to", LataFile
  print "You can open it with: \nvisit -o",LataFile
